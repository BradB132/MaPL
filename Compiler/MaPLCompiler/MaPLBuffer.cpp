//
//  MaPLBuffer.cpp
//  MaPLCompiler
//
//  Created by Brad Bambara on 2/1/22.
//

#include "MaPLBuffer.h"

#include <stdlib.h>
#include <string>
#include <regex>

#include "MaPLFile.h"
#include "MaPLVariableStack.h"

MaPLBuffer::MaPLBuffer(MaPLFile *parentFile) :
    _parentFile(parentFile)
{
    // Avoid several vector resizes by initially reserving a larger chunk of memory.
    _bytes.reserve(64);
    _annotations.reserve(16);
}

void MaPLBuffer::appendBytes(const void *bytes, size_t byteSize) {
    const uint8_t *copyPointer = (const uint8_t *)bytes;
    for (size_t i = 0; i < byteSize; i++) {
        _bytes.push_back(*(copyPointer+i));
    }
}

void MaPLBuffer::prependBytes(const void *bytes, size_t byteSize) {
    _bytes.insert(_bytes.begin(), byteSize, 0);
    overwriteBytes(bytes, byteSize, 0);
}

void MaPLBuffer::overwriteBytes(const void *bytes, size_t byteSize, size_t overwriteLocation) {
    memcpy((&_bytes[0])+overwriteLocation, bytes, byteSize);
}

void MaPLBuffer::appendInstruction(MaPLInstruction instruction) {
    if (instruction == MaPLInstruction_error) {
        _parentFile->logError(NULL, "Internal compiler error. Encountered error instruction.");
    }
    appendBytes(&instruction, sizeof(instruction));
}

MaPLMemoryAddress MaPLBuffer::calculateMemoryAddressOffset(MaPLBufferAnnotationType annotationType,
                                                           MaPLMemoryAddress endOfDependencies) {
    MaPLMemoryAddress minimumMemoryAddress;
    bool hasMinimumMemoryAddress = false;
    for (const MaPLBufferAnnotation &annotation : _annotations) {
        // Ignore any annotations that appear before the bytes we're copying.
        if (annotation.type != annotationType ||
            annotation.byteLocation < endOfDependencies) {
            continue;
        }
        MaPLMemoryAddress annotationAddress = *((MaPLMemoryAddress *)(&_bytes[annotation.byteLocation]));
        if (hasMinimumMemoryAddress) {
            if (annotationAddress < minimumMemoryAddress) {
                minimumMemoryAddress = annotationAddress;
            }
        } else {
            minimumMemoryAddress = annotationAddress;
            hasMinimumMemoryAddress = true;
        }
    }
    if (hasMinimumMemoryAddress) {
        return minimumMemoryAddress;
    }
    return 0;
}

MaPLMemoryAddress MaPLBuffer::calculatePrimitiveMemoryAddressOffset(MaPLVariableStack *variableStack, MaPLMemoryAddress endOfDependencies) {
    return variableStack->getMaximumPrimitiveMemoryUsed() - calculateMemoryAddressOffset(MaPLBufferAnnotationType_PrimitiveVariableAddressDeclaration, endOfDependencies);
}

MaPLMemoryAddress MaPLBuffer::calculateAllocatedMemoryAddressOffset(MaPLVariableStack *variableStack, MaPLMemoryAddress endOfDependencies) {
    return variableStack->getMaximumAllocatedMemoryUsed() - calculateMemoryAddressOffset(MaPLBufferAnnotationType_AllocatedVariableIndexDeclaration, endOfDependencies);
}

void MaPLBuffer::appendBuffer(MaPLBuffer *otherBuffer,
                              MaPLMemoryAddress endOfDependencies,
                              MaPLMemoryAddress primitiveMemoryAddressOffset,
                              MaPLMemoryAddress allocatedMemoryIndexOffset) {
    // Append all bytes from the other buffer.
    size_t previousSize = _bytes.size();
    const std::vector<uint8_t> &otherBytes = otherBuffer->getBytes();
    _bytes.insert(_bytes.end(), otherBytes.begin()+endOfDependencies, otherBytes.end());
    
    for (const MaPLBufferAnnotation &annotation : otherBuffer->getAnnotations()) {
        // Ignore any annotations that appear before the bytes we're copying.
        if (annotation.byteLocation < endOfDependencies ||
            annotation.type == MaPLBufferAnnotationType_EndOfDependencies) {
            continue;
        }
        
        MaPLBufferAnnotation copiedAnnotation = annotation;
        copiedAnnotation.byteLocation += previousSize - endOfDependencies;
        
        if (copiedAnnotation.type == MaPLBufferAnnotationType_PrimitiveVariableAddressDeclaration ||
            copiedAnnotation.type == MaPLBufferAnnotationType_PrimitiveVariableAddressReference) {
            *((MaPLMemoryAddress *)(&_bytes[copiedAnnotation.byteLocation])) += primitiveMemoryAddressOffset;
        } else if (annotation.type == MaPLBufferAnnotationType_AllocatedVariableIndexDeclaration ||
                   annotation.type == MaPLBufferAnnotationType_AllocatedVariableIndexReference) {
            *((MaPLMemoryAddress *)(&_bytes[copiedAnnotation.byteLocation])) += allocatedMemoryIndexOffset;
        }
        
        _annotations.push_back(copiedAnnotation);
    }
}

void MaPLBuffer::appendLiteral(const MaPLLiteral &literal, MaPLFile *file, antlr4::Token *errToken) {
    switch (literal.type.primitiveType) {
        case MaPLPrimitiveType_Char:
            appendInstruction(MaPLInstruction_char_literal);
            appendBytes(&(literal.charValue), sizeof(literal.charValue));
            break;
        case MaPLPrimitiveType_Int32:
            appendInstruction(MaPLInstruction_int32_literal);
            appendBytes(&(literal.int32Value), sizeof(literal.int32Value));
            break;
        case MaPLPrimitiveType_Int64:
            appendInstruction(MaPLInstruction_int64_literal);
            appendBytes(&(literal.int64Value), sizeof(literal.int64Value));
            break;
        case MaPLPrimitiveType_UInt32:
            appendInstruction(MaPLInstruction_uint32_literal);
            appendBytes(&(literal.uInt32Value), sizeof(literal.uInt32Value));
            break;
        case MaPLPrimitiveType_UInt64:
            appendInstruction(MaPLInstruction_uint64_literal);
            appendBytes(&(literal.uInt64Value), sizeof(literal.uInt64Value));
            break;
        case MaPLPrimitiveType_Float32:
            appendInstruction(MaPLInstruction_float32_literal);
            appendBytes(&(literal.float32Value), sizeof(literal.float32Value));
            break;
        case MaPLPrimitiveType_Float64:
            appendInstruction(MaPLInstruction_float64_literal);
            appendBytes(&(literal.float64Value), sizeof(literal.float64Value));
            break;
        case MaPLPrimitiveType_String:
            appendInstruction(MaPLInstruction_string_literal);
            appendString(literal.stringValue, file, errToken);
            break;
        case MaPLPrimitiveType_Boolean:
            appendInstruction(literal.booleanValue ? MaPLInstruction_literal_true : MaPLInstruction_literal_false);
            break;
        case MaPLPrimitiveType_Pointer:
            appendInstruction(MaPLInstruction_literal_null);
            break;
        default: break;
    }
}

void MaPLBuffer::appendString(std::string string, MaPLFile *file, antlr4::Token *errToken) {
    // Replace escaped chars.
    std::smatch match;
    std::regex escapeChars("\\\\(.)");
    auto searchStart = cbegin(string);
    while (std::regex_search(searchStart, cend(string), match, escapeChars)) {
        const char *replaceString = "";
        switch (match[1].str()[0]) {
            case 'a':
                replaceString = "\a";
                break;
            case 'b':
                replaceString = "\b";
                break;
            case 'e':
                replaceString = "\e";
                break;
            case 'f':
                replaceString = "\f";
                break;
            case 'n':
                replaceString = "\n";
                break;
            case 'r':
                replaceString = "\r";
                break;
            case 't':
                replaceString = "\t";
                break;
            case 'v':
                replaceString = "\v";
                break;
            case '"':
                replaceString = "\"";
                break;
            case '\\':
                replaceString = "\\";
                break;
            default:
                file->logError(errToken, "Invalid escape sequence '"+match[0].str()+"' specified in string. Accepted escape sequences are: \\a, \\b, \\e, \\f, \\n, \\r, \\t, \\v, \\\", \\\\.");
                return;
        }
        string.replace(match[0].first, match[0].second, replaceString);
        searchStart = match[0].first+1;
    }
    
    const char* cString = string.c_str();
    size_t length = strlen(cString);
    return appendBytes(cString, length+1);
}

std::vector<uint8_t> MaPLBuffer::getBytes() {
    return _bytes;
}

size_t MaPLBuffer::getByteCount() {
    return _bytes.size();
}

void MaPLBuffer::addAnnotation(MaPLBufferAnnotationType annotationType, const std::string &text) {
    _annotations.push_back({ (MaPLMemoryAddress)_bytes.size(), annotationType, text });
}

void MaPLBuffer::resolveControlFlowAnnotations(MaPLBufferAnnotationType type, bool jumpToEnd) {
    if (_annotations.size() == 0) {
        return;
    }
    // Iterate backwards through the list so we can remove annotations without adjusting array index.
    size_t i = _annotations.size()-1;
    while (true) {
        MaPLBufferAnnotation annotation = _annotations[i];
        if (annotation.type == type) {
            // Set the cursor move using the appropriate instruction.
            MaPLInstruction cursorInstruction = jumpToEnd ? MaPLInstruction_cursor_move_forward : MaPLInstruction_cursor_move_back;
            overwriteBytes(&cursorInstruction, sizeof(cursorInstruction), annotation.byteLocation);
            
            // 'byteLocation' describes the beginning of the relevant sequence of bytes.
            // Add the size of the Instruction and CursorMove to find the end of that sequence.
            size_t byteEndLocation = annotation.byteLocation + sizeof(MaPLInstruction) + sizeof(MaPLBytecodeLength);
            MaPLBytecodeLength cursorMove;
            if (jumpToEnd) {
                cursorMove = _bytes.size()-byteEndLocation;
            } else {// Jump to beginning.
                cursorMove = byteEndLocation;
            }
            overwriteBytes(&cursorMove, sizeof(cursorMove), annotation.byteLocation+sizeof(MaPLInstruction));
            _annotations.erase(_annotations.begin()+i);
        }
        
        if (i == 0) {
            break;
        }
        i--;
    }
}

std::vector<MaPLBufferAnnotation> MaPLBuffer::getAnnotations() {
    return _annotations;
}

MaPLBufferAnnotation MaPLBuffer::getEndOfDependenciesAnnotation() {
    for (const MaPLBufferAnnotation &annotation : _annotations) {
        if (annotation.type == MaPLBufferAnnotationType_EndOfDependencies) {
            return annotation;
        }
    }
    return { 0, MaPLBufferAnnotationType_EndOfDependencies };
}

void MaPLBuffer::resolveSymbolsWithTable(const std::map<std::string, MaPLSymbol> &symbolTable) {
    for (const MaPLBufferAnnotation &annotation : _annotations) {
        if (annotation.type == MaPLBufferAnnotationType_FunctionSymbol) {
            MaPLSymbol symbol = symbolTable.at(annotation.text);
            overwriteBytes(&symbol, sizeof(symbol), annotation.byteLocation);
        }
    }
}

void MaPLBuffer::zeroDebugLines() {
    if (_annotations.size() == 0) {
        return;
    }
    // Iterate backwards through the list so we can remove annotations without adjusting array index.
    size_t i = _annotations.size()-1;
    while (true) {
        MaPLBufferAnnotation annotation = _annotations[i];
        if (annotation.type == MaPLBufferAnnotationType_DebugLine) {
            MaPLLineNumber zeroLine = 0;
            overwriteBytes(&zeroLine, sizeof(zeroLine), annotation.byteLocation);
            _annotations.erase(_annotations.begin()+i);
        }
        if (i == 0) {
            break;
        }
        i--;
    }
}

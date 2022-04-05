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

MaPLBuffer::MaPLBuffer() {
    // Avoid several vector resizes by initially reserving a larger chunk of memory.
    _bytes.reserve(64);
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
    const uint8_t *copyPointer = (const uint8_t *)bytes;
    for (size_t i = 0; i < byteSize; i++) {
        _bytes[overwriteLocation+i] = *(copyPointer+i);
    }
}

void MaPLBuffer::appendInstruction(MaPLInstruction instruction) {
    appendBytes(&instruction, sizeof(instruction));
}

void MaPLBuffer::appendBuffer(MaPLBuffer *otherBuffer,
                              MaPLMemoryAddress primitiveMemoryAddressOffset,
                              MaPLMemoryAddress allocatedMemoryIndexOffset) {
    // Append all bytes from the other buffer.
    size_t previousSize = _bytes.size();
    std::vector<u_int8_t> otherBytes = otherBuffer->getBytes();
    _bytes.insert(_bytes.end(), otherBytes.begin(), otherBytes.end());
    
    for (MaPLBufferAnnotation annotation : otherBuffer->getAnnotations()) {
        annotation.byteLocation += previousSize;
        if (annotation.type == MaPLBufferAnnotationType_PrimitiveVariableAddress) {
            _bytes[annotation.byteLocation] += primitiveMemoryAddressOffset;
        } else if (annotation.type == MaPLBufferAnnotationType_AllocatedVariableIndex) {
            _bytes[annotation.byteLocation] += allocatedMemoryIndexOffset;
        }
        _annotations.push_back(annotation);
    }
}

void MaPLBuffer::appendLiteral(const MaPLLiteral &literal) {
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
            appendString(literal.stringValue);
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

void MaPLBuffer::appendString(std::string string) {
    // Replace escaped chars.
    std::smatch match;
    std::regex escapeChars("\\\\([abefnrtv\"\\\\])");
    auto searchStart = cbegin(string);
    while (std::regex_search(searchStart, cend(string), match, escapeChars)) {
        const char *replaceString = "";
        switch (*match[1].first.base()) {
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
            default: break;
        }
        string.replace(match[0].first, match[0].second, replaceString);
        searchStart = match[0].first;
    }
    
    const char* cString = string.c_str();
    size_t length = strlen(cString);
    return appendBytes(cString, length+1);
}

std::vector<u_int8_t> MaPLBuffer::getBytes() {
    return _bytes;
}

size_t MaPLBuffer::getByteCount() {
    return _bytes.size();
}

void MaPLBuffer::addAnnotation(MaPLBufferAnnotationType annotationType, const std::string &text) {
    _annotations.push_back({ _bytes.size(), annotationType, text });
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
            size_t byteEndLocation = annotation.byteLocation + sizeof(MaPLInstruction) + sizeof(MaPLCursorMove);
            MaPLCursorMove cursorMove;
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

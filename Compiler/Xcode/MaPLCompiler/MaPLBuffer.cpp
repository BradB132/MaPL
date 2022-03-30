//
//  MaPLBuffer.cpp
//  MaPLCompiler
//
//  Created by Brad Bambara on 2/1/22.
//

#include "MaPLBuffer.h"
#include <stdlib.h>
#include <string.h>

MaPLBuffer::MaPLBuffer(size_t bufferCapacity) :
    _bufferCapacity(bufferCapacity),
    _byteCount(0)
{
    _bytes = (u_int8_t *)malloc(bufferCapacity);
}

MaPLBuffer::~MaPLBuffer() {
    if (_bytes) {
        free(_bytes);
    }
}

bool MaPLBuffer::appendBytes(const void *bytes, size_t byteSize) {
    size_t appendedSize = _byteCount + byteSize;
    if (appendedSize > _bufferCapacity) {
        u_int8_t *reallocBytes = (u_int8_t *)realloc(_bytes, appendedSize*2);
        if (!reallocBytes) {
            return false;
        }
        _bytes = reallocBytes;
        _bufferCapacity = appendedSize*2;
    }
    memcpy((u_int8_t *)_bytes+_byteCount, bytes, byteSize);
    _byteCount += byteSize;
    return true;
}

bool MaPLBuffer::appendInstruction(MaPLInstruction instruction) {
    return appendBytes(&instruction, sizeof(instruction));
}

bool MaPLBuffer::appendBuffer(MaPLBuffer *otherBuffer, MaPLMemoryAddress variableByteIncrement) {
    size_t previousByteCount = _byteCount;
    size_t otherBufferCount = otherBuffer->getByteCount();
    if (variableByteIncrement) {
        // If this append operation requires variable byte offsets, make a copy of the data so it can be edited.
        u_int8_t copiedBytes[otherBufferCount];
        memcpy(copiedBytes, otherBuffer->getBytes(), otherBufferCount);
        
        // Increment byte offsets everywhere a variable was referenced.
        for (const MaPLBufferAnnotation &annotation : otherBuffer->getAnnotations()) {
            if (annotation.type == MaPLBufferAnnotationType_VariableOffset) {
                MaPLMemoryAddress variableByteOffset = *((MaPLMemoryAddress *)(copiedBytes+annotation.byteLocation));
                variableByteOffset += variableByteIncrement;
                memcpy(copiedBytes+annotation.byteLocation, &variableByteOffset, sizeof(variableByteOffset));
            }
        }
        if (!appendBytes(copiedBytes, otherBufferCount)) {
            return false;
        }
    } else {
        if (!appendBytes(otherBuffer->getBytes(), otherBufferCount)) {
            return false;
        }
    }
    for (MaPLBufferAnnotation annotation : otherBuffer->getAnnotations()) {
        annotation.byteLocation += previousByteCount;
        addAnnotation(annotation);
    }
    return true;
}

bool MaPLBuffer::appendLiteral(const MaPLLiteral &literal) {
    switch (literal.type.primitiveType) {
        case MaPLPrimitiveType_Int8:
            return appendInstruction(MaPLInstruction_literal_int8) &&
                   appendBytes(&(literal.int8Value), sizeof(literal.int8Value));
        case MaPLPrimitiveType_Int16:
            return appendInstruction(MaPLInstruction_literal_int16) &&
                   appendBytes(&(literal.int16Value), sizeof(literal.int16Value));
        case MaPLPrimitiveType_Int32:
            return appendInstruction(MaPLInstruction_literal_int32) &&
                   appendBytes(&(literal.int32Value), sizeof(literal.int32Value));
        case MaPLPrimitiveType_Int64:
            return appendInstruction(MaPLInstruction_literal_int64) &&
                   appendBytes(&(literal.int64Value), sizeof(literal.int64Value));
        case MaPLPrimitiveType_UInt8:
            return appendInstruction(MaPLInstruction_literal_uint8) &&
                   appendBytes(&(literal.uInt8Value), sizeof(literal.uInt8Value));
        case MaPLPrimitiveType_UInt16:
            return appendInstruction(MaPLInstruction_literal_uint16) &&
                   appendBytes(&(literal.uInt16Value), sizeof(literal.uInt16Value));
        case MaPLPrimitiveType_UInt32:
            return appendInstruction(MaPLInstruction_literal_uint32) &&
                   appendBytes(&(literal.uInt32Value), sizeof(literal.uInt32Value));
        case MaPLPrimitiveType_UInt64:
            return appendInstruction(MaPLInstruction_literal_uint64) &&
                   appendBytes(&(literal.uInt64Value), sizeof(literal.uInt64Value));
        case MaPLPrimitiveType_Float32:
            return appendInstruction(MaPLInstruction_literal_float32) &&
                   appendBytes(&(literal.float32Value), sizeof(literal.float32Value));
        case MaPLPrimitiveType_Float64:
            return appendInstruction(MaPLInstruction_literal_float64) &&
                   appendBytes(&(literal.float64Value), sizeof(literal.float64Value));
        case MaPLPrimitiveType_String:
            return appendInstruction(MaPLInstruction_literal_string) &&
                   appendString(literal.stringValue);
        case MaPLPrimitiveType_Boolean:
            return appendInstruction(literal.booleanValue ? MaPLInstruction_literal_boolean_true : MaPLInstruction_literal_boolean_false);
        case MaPLPrimitiveType_Pointer:
            return appendInstruction(MaPLInstruction_literal_null);
        default: return false;
    }
}

bool MaPLBuffer::appendString(const std::string &string) {
    const char* cString = string.c_str();
    size_t length = strlen(cString);
    return appendBytes(cString, length+1);
}

u_int8_t *MaPLBuffer::getBytes() {
    return _bytes;
}

size_t MaPLBuffer::getByteCount() {
    return _byteCount;
}

void MaPLBuffer::addAnnotation(const MaPLBufferAnnotation &annotation) {
    _annotations.push_back(annotation);
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
            memcpy(_bytes+annotation.byteLocation, &cursorInstruction, sizeof(cursorInstruction));
            
            // 'byteLocation' describes the beginning of the relevant sequence of bytes.
            // Add the size of the Instruction and CursorMove to find the end of that sequence.
            size_t byteEndLocation = annotation.byteLocation + sizeof(MaPLInstruction) + sizeof(MaPLCursorMove);
            MaPLCursorMove cursorMove;
            if (jumpToEnd) {
                cursorMove = _byteCount-byteEndLocation;
            } else {// Jump to beginning.
                cursorMove = byteEndLocation;
            }
            memcpy(_bytes+(annotation.byteLocation+sizeof(MaPLInstruction)), &cursorMove, sizeof(cursorMove));
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
            memcpy(_bytes+annotation.byteLocation, &symbol, sizeof(symbol));
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
            memcpy(_bytes+annotation.byteLocation, &zeroLine, sizeof(zeroLine));
            _annotations.erase(_annotations.begin()+i);
        }
        if (i == 0) {
            break;
        }
        i--;
    }
}

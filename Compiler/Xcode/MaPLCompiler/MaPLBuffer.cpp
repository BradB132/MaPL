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

bool MaPLBuffer::appendByte(u_int8_t byte) {
    return appendBytes(&byte, sizeof(u_int8_t));
}

bool MaPLBuffer::appendBuffer(MaPLBuffer *otherBuffer, MaPL_Index variableByteIncrement) {
    size_t previousByteCount = _byteCount;
    size_t otherBufferCount = otherBuffer->getByteCount();
    if (variableByteIncrement) {
        // If this append operation requires variable byte offsets, make a copy of the data so it can be edited.
        u_int8_t copiedBytes[otherBufferCount];
        memcpy(copiedBytes, otherBuffer->getBytes(), otherBufferCount);
        
        // Increment byte offsets everywhere a variable was referenced.
        for (MaPLBufferAnnotation annotation : otherBuffer->getAnnotations()) {
            if (annotation.type == MaPLBufferAnnotationType_VariableOffset) {
                MaPL_Index variableByteOffset = *((MaPL_Index *)(copiedBytes+annotation.byteLocation));
                variableByteOffset += variableByteIncrement;
                memcpy(copiedBytes+annotation.byteLocation, &variableByteOffset, sizeof(MaPL_Index));
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
    // Iterate backwards through the list so we can remove annotations as needed.
    for (size_t i = _annotations.size()-1; i >= 0; i--) {
        MaPLBufferAnnotation annotation = _annotations[i];
        if (annotation.type != type) {
            continue;
        }
        // Set the cursor move using the appropriate instruction.
        MaPL_Instruction cursorInstruction = jumpToEnd ? MAPL_BYTE_CURSOR_MOVE_FORWARD : MAPL_BYTE_CURSOR_MOVE_BACK;
        memcpy(_bytes+annotation.byteLocation, &cursorInstruction, sizeof(cursorInstruction));
        
        // 'byteLocation' describes the beginning of the relevant sequence of bytes.
        // Add the size of the instruction and index to find the end of that sequence.
        size_t byteEndLocation = annotation.byteLocation + sizeof(MaPL_Instruction) + sizeof(MaPL_Index);
        MaPL_Index offset;
        if (jumpToEnd) {
            offset = _byteCount-byteEndLocation;
        } else {// Jump to beginning.
            offset = byteEndLocation;
        }
        memcpy(_bytes+(annotation.byteLocation+sizeof(MaPL_Instruction)), &offset, sizeof(offset));
        _annotations.erase(_annotations.begin()+i);
    }
}

std::vector<MaPLBufferAnnotation> MaPLBuffer::getAnnotations() {
    return _annotations;
}

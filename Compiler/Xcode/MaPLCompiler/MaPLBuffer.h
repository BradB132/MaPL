//
//  MaPLBuffer.h
//  MaPLCompiler
//
//  Created by Brad Bambara on 2/1/22.
//

#ifndef MaPLBuffer_h
#define MaPLBuffer_h

#include <stdio.h>

/**
 * A wrapper for an array of bytes. Allows for easily concatinating bytes while compiling the bytecode.
 */
class  MaPLBuffer {
public:
    
    /**
     * @param bufferCapacity The initial capacity of the buffer. This param can be 0 but a more accurate guess will incur lower cost from reallocating more bytes.
     */
    MaPLBuffer(size_t bufferCapacity);
    ~MaPLBuffer();
    
    /**
     * Appends new bytes onto the buffer.
     *
     * @param bytes The new bytes that will be appended onto the buffer.
     * @param byteSize The number of bytes that should be appended from the `bytes` parameter.
     *
     * @return Boolean indicates success or failure of the append operation.
     */
    bool appendBytes(const void *bytes, size_t byteSize);
    
    /**
     * Appends a single new byte onto the buffer.
     *
     * @param byte The new byte that will be appended onto the buffer.
     *
     * @return Boolean indicates success or failure of the append operation.
     */
    bool appendByte(u_int8_t byte);
    
    /**
     * @return The bytes stored in this buffer.
     */
    u_int8_t *getBytes();
    
    /**
     * @return The number of bytes in the buffer returned via the getBytes` method.
     */
    size_t getByteCount();
    
private:
    
    u_int8_t *_bytes;
    size_t _byteCount;
    size_t _bufferCapacity;
};

#endif /* MaPLBuffer_h */
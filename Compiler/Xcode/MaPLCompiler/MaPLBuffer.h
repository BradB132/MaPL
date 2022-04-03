//
//  MaPLBuffer.h
//  MaPLCompiler
//
//  Created by Brad Bambara on 2/1/22.
//

#ifndef MaPLBuffer_h
#define MaPLBuffer_h

#include <stdio.h>
#include <vector>

#include "MaPLBytecodeConstants.h"
#include "MaPLCompilerHelpers.h"

enum MaPLBufferAnnotationType {
    MaPLBufferAnnotationType_Break,
    MaPLBufferAnnotationType_Continue,
    MaPLBufferAnnotationType_PrimitiveVariableOffset,
    MaPLBufferAnnotationType_AllocatedVariableOffset,
    MaPLBufferAnnotationType_FunctionSymbol,
    MaPLBufferAnnotationType_DebugLine,
};

struct MaPLBufferAnnotation {
    size_t byteLocation;
    MaPLBufferAnnotationType type;
    std::string text;
};

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
     * @param byteSize The number of bytes that should be appended from the @c bytes parameter.
     *
     * @return Boolean indicates success or failure of the append operation.
     */
    bool appendBytes(const void *bytes, size_t byteSize);
    
    /**
     * Appends a single instruction onto the buffer.
     *
     * @param instruction The new instruction that will be appended onto the buffer.
     *
     * @return Boolean indicates success or failure of the append operation.
     */
    bool appendInstruction(MaPLInstruction instruction);
    
    /**
     * Appends the contents of another MaPLBuffer, including annotations, onto the buffer.
     *
     * @param otherBuffer The other buffer that will have all contents copied into this buffer.
     * @param primitiveVariableByteOffset How much increment should be applied to the @c byteOffset for all primitive variable declarations.
     * @param allocatedVariableByteOffset How much increment should be applied to the @c byteOffset for all allocated variable declarations.
     *
     * @return Boolean indicates success or failure of the append operation.
     */
    bool appendBuffer(MaPLBuffer *otherBuffer,
                      MaPLMemoryAddress primitiveVariableByteOffset,
                      MaPLMemoryAddress allocatedVariableByteOffset);
    
    /**
     * Appends the literal value onto the buffer.
     *
     * @param literal The literal value that will be appended onto the buffer.
     *
     * @return Boolean indicates success or failure of the append operation.
     */
    bool appendLiteral(const MaPLLiteral &literal);
    
    /**
     * Appends the bytes from a string onto the buffer.
     *
     * @param string The string value that will be appended onto the buffer.
     *
     * @return Boolean indicates success or failure of the append operation.
     */
    bool appendString(const std::string &string);
    
    /**
     * @return The bytes stored in this buffer.
     */
    u_int8_t *getBytes();
    
    /**
     * @return The number of bytes in the buffer returned via the @c getBytes method.
     */
    size_t getByteCount();
    
    /**
     * Adds a single annotation.
     */
    void addAnnotation(const MaPLBufferAnnotation &annotation);
    
    /**
     * Resolves all matching control flow ("Break" or "Continue") annotations. Resolved annotatinons are removed from this buffer.
     *
     * @param type The type of annotations that will be resolved.
     * @param jumpToEnd If the resolution of this @c type of annotation will move the cursor to the end or beginning of the buffer.
     */
    void resolveControlFlowAnnotations(MaPLBufferAnnotationType type, bool jumpToEnd);
    
    /**
     * @return A list of all added annotations.
     */
    std::vector<MaPLBufferAnnotation> getAnnotations();
    
    /**
     * Uses a mapping of symbol descriptors and symbol values to update this buffer anywhere there's a symbol annotation.
     */
    void resolveSymbolsWithTable(const std::map<std::string, MaPLSymbol> &symbolTable);
    
    /**
     * Assigns a value of 0 to all debug lines. This is useful in the case where bytecode from multiple files are concatenated,
     * and the line numbers from the previous file are no longer meaningful.
     */
    void zeroDebugLines();
    
private:
    
    u_int8_t *_bytes;
    size_t _byteCount;
    size_t _bufferCapacity;
    std::vector<MaPLBufferAnnotation> _annotations;
};

#endif /* MaPLBuffer_h */

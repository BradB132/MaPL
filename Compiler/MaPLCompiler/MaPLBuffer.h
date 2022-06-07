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

class MaPLFile;

enum MaPLBufferAnnotationType {
    MaPLBufferAnnotationType_Break,
    MaPLBufferAnnotationType_Continue,
    MaPLBufferAnnotationType_PrimitiveVariableAddress,
    MaPLBufferAnnotationType_AllocatedVariableIndex,
    MaPLBufferAnnotationType_FunctionSymbol,
    MaPLBufferAnnotationType_DebugLine,
};

struct MaPLBufferAnnotation {
    MaPLMemoryAddress byteLocation;
    MaPLBufferAnnotationType type;
    std::string text;
};

/**
 * A wrapper for an array of bytes. Allows for easily concatinating bytes while compiling the bytecode.
 */
class  MaPLBuffer {
public:
    
    MaPLBuffer(MaPLFile *parentFile);
    
    /**
     * Appends new bytes onto the buffer.
     *
     * @param bytes The new bytes that will be appended onto the buffer.
     * @param byteSize The number of bytes that should be appended from the @c bytes parameter.
     */
    void appendBytes(const void *bytes, size_t byteSize);
    
    /**
     * Prepends new bytes onto the start of the buffer.
     *
     * @param bytes The new bytes that will be prepended onto the start of the buffer.
     * @param byteSize The number of bytes that should be prepended from the @c bytes parameter.
     */
    void prependBytes(const void *bytes, size_t byteSize);
    
    /**
     * Overwrites existing bytes in the buffer with new values.
     *
     * @param bytes The new bytes that will be written into the buffer.
     * @param byteSize The number of bytes that should be written from the @c bytes parameter.
     * @param overwriteLocation The index within the buffer to start overwriting.
     */
    void overwriteBytes(const void *bytes, size_t byteSize, size_t overwriteLocation);
    
    /**
     * Appends a single instruction onto the buffer.
     *
     * @param instruction The new instruction that will be appended onto the buffer.
     */
    void appendInstruction(MaPLInstruction instruction);
    
    /**
     * Appends the contents of another MaPLBuffer, including annotations, onto the buffer.
     *
     * @param otherBuffer The other buffer that will have all contents copied into this buffer.
     * @param primitiveMemoryAddressOffset How much increment should be applied to the memory address of all primitive variables referenced in the appended bytecode.
     * @param allocatedMemoryIndexOffset How much increment should be applied to the memory index of all allocated variables referenced in the appended bytecode.
     */
    void appendBuffer(MaPLBuffer *otherBuffer,
                      MaPLMemoryAddress primitiveMemoryAddressOffset,
                      MaPLMemoryAddress allocatedMemoryIndexOffset);
    
    /**
     * Appends the literal value onto the buffer.
     *
     * @param literal The literal value that will be appended onto the buffer.
     * @param file A file used for logging errors.
     * @param errToken A token used for logging errors.
     */
    void appendLiteral(const MaPLLiteral &literal, MaPLFile *file, antlr4::Token *errToken);
    
    /**
     * Appends the bytes from a string onto the buffer.
     *
     * @param string The string value that will be appended onto the buffer.
     * @param file A file used for logging errors.
     * @param errToken A token used for logging errors.
     */
    void appendString(std::string string, MaPLFile *file, antlr4::Token *errToken);
    
    /**
     * @return The bytes stored in this buffer.
     */
    std::vector<u_int8_t> getBytes();
    
    /**
     * @return The number of bytes in the buffer returned via the @c getBytes method.
     */
    size_t getByteCount();
    
    /**
     * Adds a single annotation to the byte position at the end of this buffer.
     */
    void addAnnotation(MaPLBufferAnnotationType annotationType, const std::string &text = "");
    
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
    
    MaPLFile *_parentFile;
    std::vector<u_int8_t> _bytes;
    std::vector<MaPLBufferAnnotation> _annotations;
};

#endif /* MaPLBuffer_h */

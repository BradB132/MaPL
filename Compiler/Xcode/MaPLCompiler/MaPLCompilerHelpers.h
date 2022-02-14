//
//  MaPLCompilerHelpers.h
//  MaPLCompiler
//
//  Created by Brad Bambara on 2/14/22.
//

#ifndef MaPLCompilerHelpers_h
#define MaPLCompilerHelpers_h

#include <stdio.h>
#include <string>
#include "MaPLParser.h"

class MaPLFile;

/**
 * Describes all literal types, and some states where the types are still ambiguous.
 */
typedef enum {
    MaPLPrimitiveType_Int8,
    MaPLPrimitiveType_Int16,
    MaPLPrimitiveType_Int32,
    MaPLPrimitiveType_Int64,
    MaPLPrimitiveType_UInt8,
    MaPLPrimitiveType_UInt16,
    MaPLPrimitiveType_UInt32,
    MaPLPrimitiveType_UInt64,
    MaPLPrimitiveType_Float32,
    MaPLPrimitiveType_Float64,
    MaPLPrimitiveType_String,
    MaPLPrimitiveType_Boolean,
    MaPLPrimitiveType_Pointer,
    
    // Numeric literals can be intepreted differently depending on surrounding context.
    MaPLPrimitiveType_SignedInt_AmbiguousSize,
    MaPLPrimitiveType_Int_AmbiguousSizeAndSign,
    MaPLPrimitiveType_Float_AmbiguousSize,
    
    MaPLPrimitiveType_InvalidType,
} MaPLPrimitiveType;

/**
 * Describes the full type of any MaPL variable, parameter, or return type. This includes the name of the #type if it's not a primitive.
 */
typedef struct {
    // Specifies the type of primitive value. All objects are represented as "Pointer" primitives.
    MaPLPrimitiveType type;
    // If @c type is a pointer, this is the name of the #type that the pointer implements.
    std::string pointerType;
} MaPLType;

/**
 * @return @c true if the primitive type is an ambiguous numeric type.
 */
bool isAmbiguousNumericType(MaPLPrimitiveType returnType);

/**
 * @return @c true if the primitive type is a non-ambiguous floating point type.
 */
bool isFloat(MaPLPrimitiveType returnType);

/**
 * @return @c true if the primitive type is a non-ambiguous signed integer type.
 */
bool isSignedInt(MaPLPrimitiveType returnType);

/**
 * @return @c true if the primitive type is a non-ambiguous unsigned integer type.
 */
bool isUnsignedInt(MaPLPrimitiveType returnType);

/**
 * @return @c true if the primitive type is numeric, including ambiguous types.
 */
bool isNumeric(MaPLPrimitiveType returnType);

/**
 * @return @c true if a #type is declared with the matching name within the file, or any dependencies of that file.
 */
bool typeExists(MaPLFile *file, std::string type);

/**
 * @return The parse tree node which represents the API function if a matching function exists, otherwise @c NULL.
 */
MaPLParser::ApiFunctionContext *findFunction(MaPLFile *file,
                                             std::string type,
                                             std::string name,
                                             std::vector<MaPLType> parameterTypes);

/**
 * @return The parse tree node which represents the API subscript if a matching subscript exists, otherwise @c NULL.
 */
MaPLParser::ApiSubscriptContext *findSubscript(MaPLFile *file,
                                               std::string type,
                                               MaPLType indexType);

/**
 * @return The parse tree node which represents the API property if a matching property exists, otherwise @c NULL.
 */
MaPLParser::ApiPropertyContext *findProperty(MaPLFile *file,
                                             std::string type,
                                             std::string name);

#endif /* MaPLCompilerHelpers_h */

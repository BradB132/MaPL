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
    MaPLPrimitiveType_SignedInt_AmbiguousSize, // Example: "-1".
    MaPLPrimitiveType_Int_AmbiguousSizeAndSign, // Example: "1".
    MaPLPrimitiveType_Float_AmbiguousSize, //  Example: "1.5".
    
    MaPLPrimitiveType_Void,
    
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
bool isAmbiguousNumericType(MaPLPrimitiveType type);

/**
 * @return @c true if the primitive type is a non-ambiguous floating point type.
 */
bool isFloat(MaPLPrimitiveType type);

/**
 * @return @c true if the primitive type is a non-ambiguous signed integer type.
 */
bool isSignedInt(MaPLPrimitiveType type);

/**
 * @return @c true if the primitive type is a non-ambiguous unsigned integer type.
 */
bool isUnsignedInt(MaPLPrimitiveType type);

/**
 * @return @c true if the primitive type is an integer type (signed or unsigned). Includes ambiguous integers.
 */
bool isIntegral(MaPLPrimitiveType type);

/**
 * @return @c true if the primitive type is numeric, including ambiguous types.
 */
bool isNumeric(MaPLPrimitiveType type);

/**
 * @return A MaPLType as described by a type node in the parse tree.
 */
MaPLType typeForTypeContext(MaPLParser::TypeContext *typeContext);

/**
 * @return The parse tree node which represents the API type, if a matching type exists. Otherwise @c NULL.
 */
MaPLParser::ApiDeclarationContext *findType(MaPLFile *file, std::string type);

/**
 * @return The parse tree node which represents the API function, if a matching function exists. Otherwise @c NULL.
 */
MaPLParser::ApiFunctionContext *findFunction(MaPLFile *file,
                                             std::string type,
                                             std::string name,
                                             std::vector<MaPLType> parameterTypes);

/**
 * @return The parse tree node which represents the API subscript, if a matching subscript exists. Otherwise @c NULL.
 */
MaPLParser::ApiSubscriptContext *findSubscript(MaPLFile *file,
                                               std::string type,
                                               MaPLType indexType);

/**
 * @return The parse tree node which represents the API property, if a matching property exists. Otherwise @c NULL.
 */
MaPLParser::ApiPropertyContext *findProperty(MaPLFile *file,
                                             std::string type,
                                             std::string name);

#endif /* MaPLCompilerHelpers_h */

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
#include "MaPLBytecodeConstants.h"

class MaPLFile;

/**
 * Describes all literal types, and some states where the types are still ambiguous.
 */
enum MaPLPrimitiveType {
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
};

/**
 * Describes the full type of any MaPL variable, parameter, or return type. This includes the name of the #type if it's not a primitive.
 */
struct MaPLType {
    // Specifies the type of primitive value. All objects are represented as "Pointer" primitives.
    MaPLPrimitiveType primitiveType;
    // If @c primitiveType is a pointer, this is the name of the #type that the pointer implements.
    std::string pointerType;
};

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
 * @return @c true if @c expressionType can be assigned to a variable of @c assignToType without error. @c false if @c assignToType is ambiguous.
 */
bool isAssignable(MaPLFile *file, MaPLType expressionType, MaPLType assignToType);

/**
 * @return The number of bytes required for each of the primitive types. @c 0 if type is invalid, void, or ambiguous.
 */
MaPL_Index byteSizeOfType(MaPLPrimitiveType type);

/**
 * @return A human-readable string that describes the type.
 */
std::string descriptorForType(MaPLType type);

/**
 * @return The instruction byte that indicates an assignment for the corresponding primitive type.
 */
MaPL_Instruction assignmentInstructionForPrimitive(MaPLPrimitiveType type);

/**
 * @return The instruction byte that indicates an typecast from the corresponding primitive type.
 */
MaPL_Instruction typecastFromInstructionForPrimitive(MaPLPrimitiveType type);

/**
 * @return The instruction byte that indicates an typecast to the corresponding primitive type.
 */
MaPL_Instruction typecastToInstructionForPrimitive(MaPLPrimitiveType type);

/**
 * @return A list of type names that both @c type1 and @c type2 inherit from. Empty if the types share no common ancestor.
 */
std::vector<std::string> mutualAncestorTypes(MaPLFile *file, std::string type1, std::string type2);

/**
 * @return @c true if @c possibleAncestorType is an ancestor of @c type.
 */
bool inheritsFromType(MaPLFile *file, std::string type, std::string possibleAncestorType);

/**
 * @return A MaPLType as described by a type node in the parse tree.
 */
MaPLType typeForTypeContext(MaPLParser::TypeContext *typeContext);

/**
 * @param file The MaPLFile to use as the root of the search.
 * @param type The string name of the #type.
 * @param excludingType A type node to exclude from the search. This is useful when checking for duplicate symbols. Pass NULL to exclude nothing.
 *
 * @return The parse tree node which represents the API type, if a matching type exists. Otherwise @c NULL.
 */
MaPLParser::ApiDeclarationContext *findType(MaPLFile *file, std::string type, MaPLParser::ApiDeclarationContext *excludingType);

/**
 * @param file The MaPLFile to use as the root of the search.
 * @param type The string name of the #type. Use an empty string for globals.
 * @param name The name of the function.
 * @param parameterTypes A list of data types corresponding to the type of each parameter. Types must not be ambiguous.
 *
 * @return The parse tree node which represents the API function, if a matching function exists. Otherwise @c NULL.
 */
MaPLParser::ApiFunctionContext *findFunction(MaPLFile *file,
                                             std::string type,
                                             std::string name,
                                             std::vector<MaPLType> parameterTypes);

/**
 * @param file The MaPLFile to use as the root of the search.
 * @param type The string name of the #type.
 * @param indexType The data type of parameter used to index into this subscript.
 * @param excludingSubscript A subscript node to exclude from the search. This is useful when checking for duplicate symbols. Pass NULL to exclude nothing.
 *
 * @return The parse tree node which represents the API subscript, if a matching subscript exists. Otherwise @c NULL.
 */
MaPLParser::ApiSubscriptContext *findSubscript(MaPLFile *file,
                                               std::string type,
                                               MaPLType indexType,
                                               MaPLParser::ApiSubscriptContext *excludingSubscript);

/**
 * @param file The MaPLFile to use as the root of the search.
 * @param type The string name of the #type. Use an empty string for globals.
 * @param name The name of the property.
 * @param excludingProperty A property node to exclude from the search. This is useful when checking for duplicate symbols. Pass NULL to exclude nothing.
 *
 * @return The parse tree node which represents the API property, if a matching property exists. Otherwise @c NULL.
 */
MaPLParser::ApiPropertyContext *findProperty(MaPLFile *file,
                                             std::string type,
                                             std::string name,
                                             MaPLParser::ApiPropertyContext *excludingProperty);

/**
 * Logs an error describing how to clarify an ambiguous literal expression.
 */
void logAmbiguousLiteralError(MaPLFile *file, MaPLPrimitiveType type, antlr4::Token *token);

/**
 * Function for logging errors. In order to maintain consistent output, all errors should go through this function.
 */
void logError(MaPLFile *file, antlr4::Token *token, const std::string &msg);

#endif /* MaPLCompilerHelpers_h */

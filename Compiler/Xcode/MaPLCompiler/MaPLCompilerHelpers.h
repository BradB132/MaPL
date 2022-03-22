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
 * Describes all primitive types, and some states where the types are still ambiguous.
 */
enum MaPLPrimitiveType {
    MaPLPrimitiveType_Uninitialized = 0,
    
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
    
    MaPLPrimitiveType_TypeError,
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
 * Describes the value of a literal expression.
 */
struct MaPLLiteral {
    // The type of the expression.
    MaPLType type;
    // The value of the expression.
    union {
        int8_t int8Value;
        int16_t int16Value;
        int32_t int32Value;
        int64_t int64Value;
        u_int8_t uInt8Value;
        u_int16_t uInt16Value;
        u_int32_t uInt32Value;
        u_int64_t uInt64Value;
        float_t float32Value;
        double_t float64Value;
        bool booleanValue;
    };
    // String value (C++ disallows declaring this as part of a union).
    std::string stringValue;
};

/**
 * Describes how variadic parameters should be interpreted when searching for functions.
 */
enum MaPLParameterStrategy {
    // Candidate functions with variadic params can successfully match longer parameter lists. Parameters can be ambiguous types.
    MaPLParameterStrategy_Flexible,
    // Only matches functions that use variadic params. List of params must exactly match function params.
    MaPLParameterStrategy_Exact_IncludeVariadicParams,
    // Only matches functions that don't use variadic params. List of params must exactly match function params.
    MaPLParameterStrategy_Exact_NoVariadicParams,
};

/**
 * @return @c true if the primitive type is an ambiguous numeric type.
 */
bool isAmbiguousNumericType(MaPLPrimitiveType type);

/**
 * @return @c true if the primitive type is a non-ambiguous floating point type.
 */
bool isConcreteFloat(MaPLPrimitiveType type);

/**
 * @return @c true if the primitive type is a non-ambiguous signed integer type.
 */
bool isConcreteSignedInt(MaPLPrimitiveType type);

/**
 * @return @c true if the primitive type is a non-ambiguous unsigned integer type.
 */
bool isConcreteUnsignedInt(MaPLPrimitiveType type);

/**
 * @return @c true if the primitive type is an integer type (signed or unsigned). Includes ambiguous integers.
 */
bool isIntegral(MaPLPrimitiveType type);

/**
 * @return @c true if the primitive type is numeric, including ambiguous types.
 */
bool isNumeric(MaPLPrimitiveType type);

/**
 * @return @c false if the primitive type is ambiguous, void, uninitialized, or error.
 */
bool isConcreteType(MaPLPrimitiveType type);

/**
 * @return @c true if @c expressionType can be assigned to a variable of @c assignToType without error. @c false if @c assignToType is ambiguous.
 */
bool isAssignable(MaPLFile *file, const MaPLType &expressionType, const MaPLType &assignToType);

/**
 * @return @c true if an expression of type @c type can be an operand for an operator of type @c operatorType.
 */
bool assignOperatorIsCompatibleWithType(MaPLFile *file, size_t operatorType, MaPLPrimitiveType type, antlr4::Token *token);

/**
 * @return The number of bytes required for each of the primitive types. @c 0 if type is invalid, void, or ambiguous.
 */
MaPLMemoryAddress byteSizeOfType(MaPLPrimitiveType type);

/**
 * @return A human-readable string that describes the primitive.
 */
std::string descriptorForPrimitive(MaPLPrimitiveType type);

/**
 * @return A human-readable string that describes the type.
 */
std::string descriptorForType(const MaPLType &type);

/**
 * @return A human-readable string that describes the signature of a function.
 */
std::string descriptorForFunction(const std::string &name, const std::vector<MaPLType> &parameterTypes, bool hasVariadicParams);

/**
 * @return A human-readable string that describes the signature of a function.
 */
std::string descriptorForFunction(MaPLParser::ApiFunctionContext *function);

/**
 * @return A human-readable string that describes the symbol for a function.
 */
std::string descriptorForSymbol(const std::string &typeName,
                                const std::string &symbolName,
                                const std::vector<MaPLType> &parameterTypes,
                                bool hasVariadicParams);

/**
 * @return The instruction byte that indicates a typecast from the corresponding primitive type.
 */
MaPLInstruction typecastFromInstructionForPrimitive(MaPLPrimitiveType type);

/**
 * @return The instruction byte that indicates a typecast to the corresponding primitive type.
 */
MaPLInstruction typecastToInstructionForPrimitive(MaPLPrimitiveType type);

/**
 * @return The instruction byte that indicates an equality comparison corresponding to @c type.
 */
MaPLInstruction equalityInstructionForPrimitive(MaPLPrimitiveType type);

/**
 * @return The instruction byte that indicates an inequality comparison corresponding to @c type.
 */
MaPLInstruction inequalityInstructionForPrimitive(MaPLPrimitiveType type);

/**
 * @return The instruction byte that indicates a less than comparison corresponding to @c type.
 */
MaPLInstruction lessThanInstructionForPrimitive(MaPLPrimitiveType type);

/**
 * @return The instruction byte that indicates a less than or equal comparison corresponding to @c type.
 */
MaPLInstruction lessThanOrEqualInstructionForPrimitive(MaPLPrimitiveType type);

/**
 * @return The instruction byte that indicates a greater than comparison corresponding to @c type.
 */
MaPLInstruction greaterThanInstructionForPrimitive(MaPLPrimitiveType type);

/**
 * @return The instruction byte that indicates a greater than or equal comparison corresponding to @c type.
 */
MaPLInstruction greaterThanOrEqualInstructionForPrimitive(MaPLPrimitiveType type);

/**
 * @return The instruction byte that indicates an assignment for the corresponding primitive type.
 */
MaPLInstruction assignmentInstructionForPrimitive(MaPLPrimitiveType type);

/**
 * @return The instruction byte that indicates a subscript assignment corresponding to @c type.
 */
MaPLInstruction assignSubscriptInstructionForPrimitive(MaPLPrimitiveType type);

/**
 * @return The instruction byte that indicates a property assignment corresponding to @c type.
 */
MaPLInstruction assignPropertyInstructionForPrimitive(MaPLPrimitiveType type);

/**
 * @return The instruction byte that represents an operator-assign.
 */
MaPLInstruction operatorAssignInstructionForTokenType(size_t tokenType, MaPLPrimitiveType primitiveType);

/**
 * @return The instruction byte that indicates a variable value corresponding to @c type.
 */
MaPLInstruction variableInstructionForPrimitive(MaPLPrimitiveType type);

/**
 * @return The instruction byte that indicates a numeric literal value corresponding to @c type.
 */
MaPLInstruction numericLiteralInstructionForPrimitive(MaPLPrimitiveType type);

/**
 * @return The instruction byte that indicates a parameter type corresponding to @c type.
 */
MaPLInstruction parameterTypeInstructionForPrimitive(MaPLPrimitiveType type);

/**
 * @return A list of type names that both @c type1 and @c type2 inherit from. Empty if the types share no common ancestor.
 */
std::vector<std::string> mutualAncestorTypes(MaPLFile *file, const std::string &type1, const std::string &type2);

/**
 * @return @c true if @c possibleAncestorType is an ancestor of @c type.
 */
bool inheritsFromType(MaPLFile *file, const std::string &type, const std::string &possibleAncestorType);

/**
 * @return @c true if the #type declarations in @c file contain an inheritance cycle.
 */
bool findInheritanceCycle(MaPLFile *file);

/**
 * A set of paths to files which appear multiple times in the dependency graph of @c file.
 */
std::set<std::filesystem::path> findDuplicateDependencies(MaPLFile *file);

/**
 * @return @c true if @c node is within the scope of a "for", "while", or "do while" loop.
 */
bool isInsideLoopScope(antlr4::tree::ParseTree *node);

/**
 * @return A @c MaPLType as described by a type node in the parse tree.
 */
MaPLType typeForTypeContext(MaPLParser::TypeContext *typeContext);

/**
 * @return The value of @c literal after being cast to @c castType.
 */
MaPLLiteral castLiteralToType(const MaPLLiteral &literal, const MaPLType &castType);

/**
 * @return The object expression that terminates the chain of expressions for which @c rootExpression is the root node.
 */
MaPLParser::ObjectExpressionContext *terminalObjectExpression(MaPLParser::ObjectExpressionContext *rootExpression);

/**
 * @return The beginning of the object expression, excluding only the termal expression, for which @c rootExpression is the root node.
 */
MaPLParser::ObjectExpressionContext *prefixObjectExpression(MaPLParser::ObjectExpressionContext *rootExpression);

/**
 * @param file The MaPLFile to use as the root of the search.
 * @param type The string name of the #type.
 * @param excludingType A type node to exclude from the search. This is useful when checking for duplicate symbols. Pass NULL to exclude nothing.
 *
 * @return The parse tree node which represents the API type, if a matching type exists. Otherwise @c NULL.
 */
MaPLParser::ApiDeclarationContext *findType(MaPLFile *file, const std::string &type, MaPLParser::ApiDeclarationContext *excludingType);

/**
 * @param file The MaPLFile to use as the root of the search.
 * @param type The string name of the #type. Use an empty string for globals.
 * @param name The name of the function.
 * @param parameterTypes A list of data types corresponding to the type of each parameter. Types must not be ambiguous if @c parameterStrategy is exact.
 * @param parameterStrategy The strategy that this function will use with comparing @c parameterTypes against the parameters declared in the API.
 * @param excludingFunction A function node to exclude from the search. This is useful when checking for duplicate symbols. Pass NULL to exclude nothing.
 *
 * @return The parse tree node which represents the API function, if a matching function exists. Otherwise @c NULL.
 */
MaPLParser::ApiFunctionContext *findFunction(MaPLFile *file,
                                             const std::string &type,
                                             const std::string &name,
                                             const std::vector<MaPLType> &parameterTypes,
                                             MaPLParameterStrategy parameterStrategy,
                                             MaPLParser::ApiFunctionContext *excludingFunction);

/**
 * @param file The MaPLFile to use as the root of the search.
 * @param type The string name of the #type.
 * @param indexType The data type of parameter used to index into this subscript.
 * @param excludingSubscript A subscript node to exclude from the search. This is useful when checking for duplicate symbols. Pass NULL to exclude nothing.
 *
 * @return The parse tree node which represents the API subscript, if a matching subscript exists. Otherwise @c NULL.
 */
MaPLParser::ApiSubscriptContext *findSubscript(MaPLFile *file,
                                               const std::string &type,
                                               const MaPLType &indexType,
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
                                             const std::string &type,
                                             const std::string &name,
                                             MaPLParser::ApiPropertyContext *excludingProperty);

#endif /* MaPLCompilerHelpers_h */

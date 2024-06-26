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
#include <stdint.h>

#include "MaPLParser.h"
#include "MaPLBytecodeConstants.h"

class MaPLFile;

/**
 * Describes all primitive types, and some states where the types are still ambiguous.
 */
enum MaPLPrimitiveType {
    MaPLPrimitiveType_Uninitialized = 0,
    
    MaPLPrimitiveType_Char,
    MaPLPrimitiveType_Int32,
    MaPLPrimitiveType_Int64,
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
 * Describes the full type of any MaPL variable, parameter, or return type.
 */
struct MaPLType {
    // Specifies the type of primitive value. All objects are represented as "Pointer" primitives.
    MaPLPrimitiveType primitiveType;
    // If @c primitiveType is a pointer, this is the name of the #type that the pointer implements.
    std::string pointerType;
    // If @c primitiveType is a pointer and has generics, this is the mapping of generic descriptors to types.
    std::vector<MaPLType> generics;
    
    bool operator== (const MaPLType &otherType) const;
    bool operator!= (const MaPLType &otherType) const;
};

/**
 * Describes the value of a literal expression.
 */
struct MaPLLiteral {
    // The type of the expression.
    MaPLType type;
    // The value of the expression.
    union {
        uint8_t charValue;
        int32_t int32Value;
        int64_t int64Value;
        uint32_t uInt32Value;
        uint64_t uInt64Value;
        float float32Value;
        double float64Value;
        bool booleanValue;
    };
    // String value (C++ disallows declaring this as part of a union).
    std::string stringValue;
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
std::string descriptorForFunction(const std::string &typeName,
                                  const std::string &name,
                                  const std::vector<MaPLType> &parameterTypes,
                                  bool hasVariadicParams);

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
 * @return The instruction byte that indicates a function invocation for the corresponding primitive type.
 */
MaPLInstruction functionInvocationInstructionForPrimitive(MaPLPrimitiveType type);

/**
 * @return The instruction byte that indicates a subscript invocation for the corresponding primitive type.
 */
MaPLInstruction subscriptInvocationInstructionForPrimitive(MaPLPrimitiveType type);

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
 * @return The instruction byte that indicates numeric addition corresponding to @c type.
 */
MaPLInstruction additionInstructionForPrimitive(MaPLPrimitiveType type);

/**
 * @return The instruction byte that indicates numeric subtraction corresponding to @c type.
 */
MaPLInstruction subtractionInstructionForPrimitive(MaPLPrimitiveType type);

/**
 * @return The instruction byte that indicates numeric multiplication corresponding to @c type.
 */
MaPLInstruction multiplicationInstructionForPrimitive(MaPLPrimitiveType type);

/**
 * @return The instruction byte that indicates numeric division corresponding to @c type.
 */
MaPLInstruction divisionInstructionForPrimitive(MaPLPrimitiveType type);

/**
 * @return The instruction byte that indicates modulo corresponding to @c type.
 */
MaPLInstruction moduloInstructionForPrimitive(MaPLPrimitiveType type);

/**
 * @return The instruction byte that indicates numeric negation corresponding to @c type.
 */
MaPLInstruction numericNegationInstructionForPrimitive(MaPLPrimitiveType type);

/**
 * @return The instruction byte that indicates bitwise negation corresponding to @c type.
 */
MaPLInstruction bitwiseNegationInstructionForPrimitive(MaPLPrimitiveType type);

/**
 * @return The instruction byte that indicates bitwise "and" corresponding to @c type.
 */
MaPLInstruction bitwiseAndInstructionForPrimitive(MaPLPrimitiveType type);

/**
 * @return The instruction byte that indicates bitwise "or" corresponding to @c type.
 */
MaPLInstruction bitwiseOrInstructionForPrimitive(MaPLPrimitiveType type);

/**
 * @return The instruction byte that indicates bitwise "xor" corresponding to @c type.
 */
MaPLInstruction bitwiseXorInstructionForPrimitive(MaPLPrimitiveType type);

/**
 * @return The instruction byte that indicates bitwise shift left corresponding to @c type.
 */
MaPLInstruction bitwiseShiftLeftInstructionForPrimitive(MaPLPrimitiveType type);

/**
 * @return The instruction byte that indicates bitwise shift right corresponding to @c type.
 */
MaPLInstruction bitwiseShiftRightInstructionForPrimitive(MaPLPrimitiveType type);

/**
 * @return The instruction byte that indicates a ternary conditional corresponding to @c type.
 */
MaPLInstruction ternaryConditionalInstructionForPrimitive(MaPLPrimitiveType type);

/**
 * @return An array containing a de-duplicated list of the dependent files of @c file. List is depth-first order.
 */
std::vector<MaPLFile *> flattenedDependencies(MaPLFile *file);

/**
 * @return A mapping of all symbols names to symbol values for APIs declared within @c files.
 */
std::map<std::string, MaPLSymbol> symbolTableForFiles(const std::vector<MaPLFile *> files);

/**
 * @return @c true if @c node is within the scope of a "for", "while", or "do while" loop.
 */
bool isInsideLoopScope(antlr4::tree::ParseTree *node);

/**
 * If an object expression is the last expression in a chain of object expressions, and that chain comprises an imperative expression,
 * then the runtime can safely disregard the return value of the chain because the function is invoked, but the value is never read.
 *
 * @return @c true if @c objectExpression is a terminal imperative object expression.
 */
bool isTerminalImperativeObjectExpression(MaPLParser::ObjectExpressionContext *objectExpression);

/**
 * @return A @c MaPLType as described by a @c pointerType node in the parse tree.
 */
MaPLType typeForPointerType(MaPLParser::PointerTypeContext *pointerTypeContext);

/**
 * @return A @c MaPLPrimitiveType as described by a type node in the parse tree.
 */
MaPLPrimitiveType primitiveTypeForTypeContext(MaPLParser::TypeContext *typeContext);

/**
 * @return A @c MaPLType as described by a type node in the parse tree.
 */
MaPLType typeForTypeContext(MaPLParser::TypeContext *typeContext);

/**
 * @return @c true only if @c typeName exactly matches the name of a primitive type.
 */
bool typeNameMatchesPrimitiveType(const std::string &typeName);

/**
 * @return The value of @c literal after being cast to @c castType.
 */
MaPLLiteral castLiteralToType(const MaPLLiteral &literal, const MaPLType &castType, MaPLFile *file, antlr4::Token *token);

/**
 * @return An integer representing the log2 of the literal value. Returns @c 0 if the literal value is not a power of 2.
 */
uint8_t bitShiftForLiteral(const MaPLLiteral &literal);

/**
 * @return The object expression that terminates the chain of expressions for which @c rootExpression is the root node.
 */
MaPLParser::ObjectExpressionContext *terminalObjectExpression(MaPLParser::ObjectExpressionContext *rootExpression);

/**
 * @return The beginning of the object expression, excluding only the termal expression, for which @c rootExpression is the root node.
 */
MaPLParser::ObjectExpressionContext *prefixObjectExpression(MaPLParser::ObjectExpressionContext *rootExpression);

#endif /* MaPLCompilerHelpers_h */

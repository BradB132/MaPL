//
//  MaPLDecompiler.cpp
//  MaPLTester
//
//  Created by Brad Bambara on 6/14/22.
//

#include "MaPLDecompiler.h"

#include <stdlib.h>
#include <string.h>

#include "MaPLRuntime.h"
#include "MaPLBytecodeConstants.h"

struct MaPLDecompilerContext {
    const u_int8_t* scriptBuffer;
    MaPLBytecodeLength cursorPosition = 0;
    uint32_t indent = 0;
};

void evaluateStatement(MaPLDecompilerContext *context);

void printIndent(MaPLDecompilerContext *context) {
    for (uint32_t i = 0; i < context->indent; i++) {
        printf("\t");
    }
}

void printLineNumber(MaPLDecompilerContext *context, MaPLBytecodeLength byteLength = 1) {
    printIndent(context);
    if (byteLength <= 1) {
        printf("%u: ", context->cursorPosition);
    } else {
        printf("%u-%u: ", context->cursorPosition, context->cursorPosition+(byteLength-1));
    }
}

MaPLInstruction printInstruction(MaPLDecompilerContext *context) {
    MaPLInstruction instruction = (MaPLInstruction)context->scriptBuffer[context->cursorPosition];
    const char *instructionStr;
    switch (instruction) {
        case MaPLInstruction_placeholder:
            instructionStr = "placeholder";
            break;
        case MaPLInstruction_int32_literal:
            instructionStr = "int32_literal";
            break;
        case MaPLInstruction_int32_variable:
            instructionStr = "int32_variable";
            break;
        case MaPLInstruction_int32_add:
            instructionStr = "int32_add";
            break;
        case MaPLInstruction_int32_subtract:
            instructionStr = "int32_subtract";
            break;
        case MaPLInstruction_int32_divide:
            instructionStr = "int32_divide";
            break;
        case MaPLInstruction_int32_multiply:
            instructionStr = "int32_multiply";
            break;
        case MaPLInstruction_int32_modulo:
            instructionStr = "int32_modulo";
            break;
        case MaPLInstruction_int32_numeric_negation:
            instructionStr = "int32_numeric_negation";
            break;
        case MaPLInstruction_int32_bitwise_and:
            instructionStr = "int32_bitwise_and";
            break;
        case MaPLInstruction_int32_bitwise_or:
            instructionStr = "int32_bitwise_or";
            break;
        case MaPLInstruction_int32_bitwise_xor:
            instructionStr = "int32_bitwise_xor";
            break;
        case MaPLInstruction_int32_bitwise_negation:
            instructionStr = "int32_bitwise_negation";
            break;
        case MaPLInstruction_int32_bitwise_shift_left:
            instructionStr = "int32_bitwise_shift_left";
            break;
        case MaPLInstruction_int32_bitwise_shift_right:
            instructionStr = "int32_bitwise_shift_right";
            break;
        case MaPLInstruction_int32_function_invocation:
            instructionStr = "int32_function_invocation";
            break;
        case MaPLInstruction_int32_subscript_invocation:
            instructionStr = "int32_subscript_invocation";
            break;
        case MaPLInstruction_int32_ternary_conditional:
            instructionStr = "int32_ternary_conditional";
            break;
        case MaPLInstruction_int32_typecast:
            instructionStr = "int32_typecast";
            break;
        case MaPLInstruction_float32_literal:
            instructionStr = "float32_literal";
            break;
        case MaPLInstruction_float32_variable:
            instructionStr = "float32_variable";
            break;
        case MaPLInstruction_float32_add:
            instructionStr = "float32_add";
            break;
        case MaPLInstruction_float32_subtract:
            instructionStr = "float32_subtract";
            break;
        case MaPLInstruction_float32_divide:
            instructionStr = "float32_divide";
            break;
        case MaPLInstruction_float32_multiply:
            instructionStr = "float32_multiply";
            break;
        case MaPLInstruction_float32_modulo:
            instructionStr = "float32_modulo";
            break;
        case MaPLInstruction_float32_numeric_negation:
            instructionStr = "float32_numeric_negation";
            break;
        case MaPLInstruction_float32_function_invocation:
            instructionStr = "float32_function_invocation";
            break;
        case MaPLInstruction_float32_subscript_invocation:
            instructionStr = "float32_subscript_invocation";
            break;
        case MaPLInstruction_float32_ternary_conditional:
            instructionStr = "float32_ternary_conditional";
            break;
        case MaPLInstruction_float32_typecast:
            instructionStr = "float32_typecast";
            break;
        case MaPLInstruction_string_literal:
            instructionStr = "string_literal";
            break;
        case MaPLInstruction_string_variable:
            instructionStr = "string_variable";
            break;
        case MaPLInstruction_string_concat:
            instructionStr = "string_concat";
            break;
        case MaPLInstruction_string_function_invocation:
            instructionStr = "string_function_invocation";
            break;
        case MaPLInstruction_string_subscript_invocation:
            instructionStr = "string_subscript_invocation";
            break;
        case MaPLInstruction_string_ternary_conditional:
            instructionStr = "string_ternary_conditional";
            break;
        case MaPLInstruction_string_typecast:
            instructionStr = "string_typecast";
            break;
        case MaPLInstruction_literal_null:
            instructionStr = "literal_null";
            break;
        case MaPLInstruction_pointer_variable:
            instructionStr = "pointer_variable";
            break;
        case MaPLInstruction_pointer_null_coalescing:
            instructionStr = "pointer_null_coalescing";
            break;
        case MaPLInstruction_pointer_function_invocation:
            instructionStr = "pointer_function_invocation";
            break;
        case MaPLInstruction_pointer_subscript_invocation:
            instructionStr = "pointer_subscript_invocation";
            break;
        case MaPLInstruction_pointer_ternary_conditional:
            instructionStr = "pointer_ternary_conditional";
            break;
        case MaPLInstruction_literal_true:
            instructionStr = "literal_true";
            break;
        case MaPLInstruction_literal_false:
            instructionStr = "literal_false";
            break;
        case MaPLInstruction_boolean_variable:
            instructionStr = "boolean_variable";
            break;
        case MaPLInstruction_boolean_function_invocation:
            instructionStr = "boolean_function_invocation";
            break;
        case MaPLInstruction_boolean_subscript_invocation:
            instructionStr = "boolean_subscript_invocation";
            break;
        case MaPLInstruction_boolean_ternary_conditional:
            instructionStr = "boolean_ternary_conditional";
            break;
        case MaPLInstruction_boolean_typecast:
            instructionStr = "boolean_typecast";
            break;
        case MaPLInstruction_logical_equality_char:
            instructionStr = "logical_equality_char";
            break;
        case MaPLInstruction_logical_equality_int32:
            instructionStr = "logical_equality_int32";
            break;
        case MaPLInstruction_logical_equality_int64:
            instructionStr = "logical_equality_int64";
            break;
        case MaPLInstruction_logical_equality_uint32:
            instructionStr = "logical_equality_uint32";
            break;
        case MaPLInstruction_logical_equality_uint64:
            instructionStr = "logical_equality_uint64";
            break;
        case MaPLInstruction_logical_equality_float32:
            instructionStr = "logical_equality_float32";
            break;
        case MaPLInstruction_logical_equality_float64:
            instructionStr = "logical_equality_float64";
            break;
        case MaPLInstruction_logical_equality_boolean:
            instructionStr = "logical_equality_boolean";
            break;
        case MaPLInstruction_logical_equality_string:
            instructionStr = "logical_equality_string";
            break;
        case MaPLInstruction_logical_equality_pointer:
            instructionStr = "logical_equality_pointer";
            break;
        case MaPLInstruction_logical_inequality_char:
            instructionStr = "logical_inequality_char";
            break;
        case MaPLInstruction_logical_inequality_int32:
            instructionStr = "logical_inequality_int32";
            break;
        case MaPLInstruction_logical_inequality_int64:
            instructionStr = "logical_inequality_int64";
            break;
        case MaPLInstruction_logical_inequality_uint32:
            instructionStr = "logical_inequality_uint32";
            break;
        case MaPLInstruction_logical_inequality_uint64:
            instructionStr = "logical_inequality_uint64";
            break;
        case MaPLInstruction_logical_inequality_float32:
            instructionStr = "logical_inequality_float32";
            break;
        case MaPLInstruction_logical_inequality_float64:
            instructionStr = "logical_inequality_float64";
            break;
        case MaPLInstruction_logical_inequality_boolean:
            instructionStr = "logical_inequality_boolean";
            break;
        case MaPLInstruction_logical_inequality_string:
            instructionStr = "logical_inequality_string";
            break;
        case MaPLInstruction_logical_inequality_pointer:
            instructionStr = "logical_inequality_pointer";
            break;
        case MaPLInstruction_logical_less_than_char:
            instructionStr = "logical_less_than_char";
            break;
        case MaPLInstruction_logical_less_than_int32:
            instructionStr = "logical_less_than_int32";
            break;
        case MaPLInstruction_logical_less_than_int64:
            instructionStr = "logical_less_than_int64";
            break;
        case MaPLInstruction_logical_less_than_uint32:
            instructionStr = "logical_less_than_uint32";
            break;
        case MaPLInstruction_logical_less_than_uint64:
            instructionStr = "logical_less_than_uint64";
            break;
        case MaPLInstruction_logical_less_than_float32:
            instructionStr = "logical_less_than_float32";
            break;
        case MaPLInstruction_logical_less_than_float64:
            instructionStr = "logical_less_than_float64";
            break;
        case MaPLInstruction_logical_less_than_equal_char:
            instructionStr = "logical_less_than_equal_char";
            break;
        case MaPLInstruction_logical_less_than_equal_int32:
            instructionStr = "logical_less_than_equal_int32";
            break;
        case MaPLInstruction_logical_less_than_equal_int64:
            instructionStr = "logical_less_than_equal_int64";
            break;
        case MaPLInstruction_logical_less_than_equal_uint32:
            instructionStr = "logical_less_than_equal_uint32";
            break;
        case MaPLInstruction_logical_less_than_equal_uint64:
            instructionStr = "logical_less_than_equal_uint64";
            break;
        case MaPLInstruction_logical_less_than_equal_float32:
            instructionStr = "logical_less_than_equal_float32";
            break;
        case MaPLInstruction_logical_less_than_equal_float64:
            instructionStr = "logical_less_than_equal_float64";
            break;
        case MaPLInstruction_logical_greater_than_char:
            instructionStr = "logical_greater_than_char";
            break;
        case MaPLInstruction_logical_greater_than_int32:
            instructionStr = "logical_greater_than_int32";
            break;
        case MaPLInstruction_logical_greater_than_int64:
            instructionStr = "logical_greater_than_int64";
            break;
        case MaPLInstruction_logical_greater_than_uint32:
            instructionStr = "logical_greater_than_uint32";
            break;
        case MaPLInstruction_logical_greater_than_uint64:
            instructionStr = "logical_greater_than_uint64";
            break;
        case MaPLInstruction_logical_greater_than_float32:
            instructionStr = "logical_greater_than_float32";
            break;
        case MaPLInstruction_logical_greater_than_float64:
            instructionStr = "logical_greater_than_float64";
            break;
        case MaPLInstruction_logical_greater_than_equal_char:
            instructionStr = "logical_greater_than_equal_char";
            break;
        case MaPLInstruction_logical_greater_than_equal_int32:
            instructionStr = "logical_greater_than_equal_int32";
            break;
        case MaPLInstruction_logical_greater_than_equal_int64:
            instructionStr = "logical_greater_than_equal_int64";
            break;
        case MaPLInstruction_logical_greater_than_equal_uint32:
            instructionStr = "logical_greater_than_equal_uint32";
            break;
        case MaPLInstruction_logical_greater_than_equal_uint64:
            instructionStr = "logical_greater_than_equal_uint64";
            break;
        case MaPLInstruction_logical_greater_than_equal_float32:
            instructionStr = "logical_greater_than_equal_float32";
            break;
        case MaPLInstruction_logical_greater_than_equal_float64:
            instructionStr = "logical_greater_than_equal_float64";
            break;
        case MaPLInstruction_logical_and:
            instructionStr = "logical_and";
            break;
        case MaPLInstruction_logical_or:
            instructionStr = "logical_or";
            break;
        case MaPLInstruction_logical_negation:
            instructionStr = "logical_negation";
            break;
        case MaPLInstruction_int64_literal:
            instructionStr = "int64_literal";
            break;
        case MaPLInstruction_int64_variable:
            instructionStr = "int64_variable";
            break;
        case MaPLInstruction_int64_add:
            instructionStr = "int64_add";
            break;
        case MaPLInstruction_int64_subtract:
            instructionStr = "int64_subtract";
            break;
        case MaPLInstruction_int64_divide:
            instructionStr = "int64_divide";
            break;
        case MaPLInstruction_int64_multiply:
            instructionStr = "int64_multiply";
            break;
        case MaPLInstruction_int64_modulo:
            instructionStr = "int64_modulo";
            break;
        case MaPLInstruction_int64_numeric_negation:
            instructionStr = "int64_numeric_negation";
            break;
        case MaPLInstruction_int64_bitwise_and:
            instructionStr = "int64_bitwise_and";
            break;
        case MaPLInstruction_int64_bitwise_or:
            instructionStr = "int64_bitwise_or";
            break;
        case MaPLInstruction_int64_bitwise_xor:
            instructionStr = "int64_bitwise_xor";
            break;
        case MaPLInstruction_int64_bitwise_negation:
            instructionStr = "int64_bitwise_negation";
            break;
        case MaPLInstruction_int64_bitwise_shift_left:
            instructionStr = "int64_bitwise_shift_left";
            break;
        case MaPLInstruction_int64_bitwise_shift_right:
            instructionStr = "int64_bitwise_shift_right";
            break;
        case MaPLInstruction_int64_function_invocation:
            instructionStr = "int64_function_invocation";
            break;
        case MaPLInstruction_int64_subscript_invocation:
            instructionStr = "int64_subscript_invocation";
            break;
        case MaPLInstruction_int64_ternary_conditional:
            instructionStr = "int64_ternary_conditional";
            break;
        case MaPLInstruction_int64_typecast:
            instructionStr = "int64_typecast";
            break;
        case MaPLInstruction_float64_literal:
            instructionStr = "float64_literal";
            break;
        case MaPLInstruction_float64_variable:
            instructionStr = "float64_variable";
            break;
        case MaPLInstruction_float64_add:
            instructionStr = "float64_add";
            break;
        case MaPLInstruction_float64_subtract:
            instructionStr = "float64_subtract";
            break;
        case MaPLInstruction_float64_divide:
            instructionStr = "float64_divide";
            break;
        case MaPLInstruction_float64_multiply:
            instructionStr = "float64_multiply";
            break;
        case MaPLInstruction_float64_modulo:
            instructionStr = "float64_modulo";
            break;
        case MaPLInstruction_float64_numeric_negation:
            instructionStr = "float64_numeric_negation";
            break;
        case MaPLInstruction_float64_function_invocation:
            instructionStr = "float64_function_invocation";
            break;
        case MaPLInstruction_float64_subscript_invocation:
            instructionStr = "float64_subscript_invocation";
            break;
        case MaPLInstruction_float64_ternary_conditional:
            instructionStr = "float64_ternary_conditional";
            break;
        case MaPLInstruction_float64_typecast:
            instructionStr = "float64_typecast";
            break;
        case MaPLInstruction_uint32_literal:
            instructionStr = "uint32_literal";
            break;
        case MaPLInstruction_uint32_variable:
            instructionStr = "uint32_variable";
            break;
        case MaPLInstruction_uint32_add:
            instructionStr = "uint32_add";
            break;
        case MaPLInstruction_uint32_subtract:
            instructionStr = "uint32_subtract";
            break;
        case MaPLInstruction_uint32_divide:
            instructionStr = "uint32_divide";
            break;
        case MaPLInstruction_uint32_multiply:
            instructionStr = "uint32_multiply";
            break;
        case MaPLInstruction_uint32_modulo:
            instructionStr = "uint32_modulo";
            break;
        case MaPLInstruction_uint32_bitwise_and:
            instructionStr = "uint32_bitwise_and";
            break;
        case MaPLInstruction_uint32_bitwise_or:
            instructionStr = "uint32_bitwise_or";
            break;
        case MaPLInstruction_uint32_bitwise_xor:
            instructionStr = "uint32_bitwise_xor";
            break;
        case MaPLInstruction_uint32_bitwise_negation:
            instructionStr = "uint32_bitwise_negation";
            break;
        case MaPLInstruction_uint32_bitwise_shift_left:
            instructionStr = "uint32_bitwise_shift_left";
            break;
        case MaPLInstruction_uint32_bitwise_shift_right:
            instructionStr = "uint32_bitwise_shift_right";
            break;
        case MaPLInstruction_uint32_function_invocation:
            instructionStr = "uint32_function_invocation";
            break;
        case MaPLInstruction_uint32_subscript_invocation:
            instructionStr = "uint32_subscript_invocation";
            break;
        case MaPLInstruction_uint32_ternary_conditional:
            instructionStr = "uint32_ternary_conditional";
            break;
        case MaPLInstruction_uint32_typecast:
            instructionStr = "uint32_typecast";
            break;
        case MaPLInstruction_uint64_literal:
            instructionStr = "uint64_literal";
            break;
        case MaPLInstruction_uint64_variable:
            instructionStr = "uint64_variable";
            break;
        case MaPLInstruction_uint64_add:
            instructionStr = "uint64_add";
            break;
        case MaPLInstruction_uint64_subtract:
            instructionStr = "uint64_subtract";
            break;
        case MaPLInstruction_uint64_divide:
            instructionStr = "uint64_divide";
            break;
        case MaPLInstruction_uint64_multiply:
            instructionStr = "uint64_multiply";
            break;
        case MaPLInstruction_uint64_modulo:
            instructionStr = "uint64_modulo";
            break;
        case MaPLInstruction_uint64_bitwise_and:
            instructionStr = "uint64_bitwise_and";
            break;
        case MaPLInstruction_uint64_bitwise_or:
            instructionStr = "uint64_bitwise_or";
            break;
        case MaPLInstruction_uint64_bitwise_xor:
            instructionStr = "uint64_bitwise_xor";
            break;
        case MaPLInstruction_uint64_bitwise_negation:
            instructionStr = "uint64_bitwise_negation";
            break;
        case MaPLInstruction_uint64_bitwise_shift_left:
            instructionStr = "uint64_bitwise_shift_left";
            break;
        case MaPLInstruction_uint64_bitwise_shift_right:
            instructionStr = "uint64_bitwise_shift_right";
            break;
        case MaPLInstruction_uint64_function_invocation:
            instructionStr = "uint64_function_invocation";
            break;
        case MaPLInstruction_uint64_subscript_invocation:
            instructionStr = "uint64_subscript_invocation";
            break;
        case MaPLInstruction_uint64_ternary_conditional:
            instructionStr = "uint64_ternary_conditional";
            break;
        case MaPLInstruction_uint64_typecast:
            instructionStr = "uint64_typecast";
            break;
        case MaPLInstruction_char_literal:
            instructionStr = "char_literal";
            break;
        case MaPLInstruction_char_variable:
            instructionStr = "char_variable";
            break;
        case MaPLInstruction_char_add:
            instructionStr = "char_add";
            break;
        case MaPLInstruction_char_subtract:
            instructionStr = "char_subtract";
            break;
        case MaPLInstruction_char_divide:
            instructionStr = "char_divide";
            break;
        case MaPLInstruction_char_multiply:
            instructionStr = "char_multiply";
            break;
        case MaPLInstruction_char_modulo:
            instructionStr = "char_modulo";
            break;
        case MaPLInstruction_char_bitwise_and:
            instructionStr = "char_bitwise_and";
            break;
        case MaPLInstruction_char_bitwise_or:
            instructionStr = "char_bitwise_or";
            break;
        case MaPLInstruction_char_bitwise_xor:
            instructionStr = "char_bitwise_xor";
            break;
        case MaPLInstruction_char_bitwise_negation:
            instructionStr = "char_bitwise_negation";
            break;
        case MaPLInstruction_char_bitwise_shift_left:
            instructionStr = "char_bitwise_shift_left";
            break;
        case MaPLInstruction_char_bitwise_shift_right:
            instructionStr = "char_bitwise_shift_right";
            break;
        case MaPLInstruction_char_function_invocation:
            instructionStr = "char_function_invocation";
            break;
        case MaPLInstruction_char_subscript_invocation:
            instructionStr = "char_subscript_invocation";
            break;
        case MaPLInstruction_char_ternary_conditional:
            instructionStr = "char_ternary_conditional";
            break;
        case MaPLInstruction_char_typecast:
            instructionStr = "char_typecast";
            break;
        case MaPLInstruction_unused_return_function_invocation:
            instructionStr = "unused_return_function_invocation";
            break;
        case MaPLInstruction_char_assign:
            instructionStr = "char_assign";
            break;
        case MaPLInstruction_int32_assign:
            instructionStr = "int32_assign";
            break;
        case MaPLInstruction_int64_assign:
            instructionStr = "int64_assign";
            break;
        case MaPLInstruction_uint32_assign:
            instructionStr = "uint32_assign";
            break;
        case MaPLInstruction_uint64_assign:
            instructionStr = "uint64_assign";
            break;
        case MaPLInstruction_float32_assign:
            instructionStr = "float32_assign";
            break;
        case MaPLInstruction_float64_assign:
            instructionStr = "float64_assign";
            break;
        case MaPLInstruction_boolean_assign:
            instructionStr = "boolean_assign";
            break;
        case MaPLInstruction_string_assign:
            instructionStr = "string_assign";
            break;
        case MaPLInstruction_pointer_assign:
            instructionStr = "pointer_assign";
            break;
        case MaPLInstruction_assign_subscript:
            instructionStr = "assign_subscript";
            break;
        case MaPLInstruction_assign_property:
            instructionStr = "assign_property";
            break;
        case MaPLInstruction_conditional:
            instructionStr = "conditional";
            break;
        case MaPLInstruction_cursor_move_forward:
            instructionStr = "cursor_move_forward";
            break;
        case MaPLInstruction_cursor_move_back:
            instructionStr = "cursor_move_back";
            break;
        case MaPLInstruction_program_exit:
            instructionStr = "program_exit";
            break;
        case MaPLInstruction_metadata:
            instructionStr = "metadata";
            break;
        case MaPLInstruction_debug_line:
            instructionStr = "debug_line";
            break;
        case MaPLInstruction_debug_update_variable:
            instructionStr = "debug_update_variable";
            break;
        case MaPLInstruction_debug_delete_variable:
            instructionStr = "debug_delete_variable";
            break;
        case MaPLInstruction_no_op:
            instructionStr = "no_op";
            break;
        case MaPLInstruction_error:
            instructionStr = "error";
            break;
        default:
            fputs("Error: Unrecognized bytecode instruction.\n", stderr);
            exit(1);
    }
    printLineNumber(context);
    printf("INSTRUCTION=%s\n", instructionStr);
    context->cursorPosition++;
    return instruction;
}

void printMemoryAddress(MaPLDecompilerContext *context) {
    MaPLMemoryAddress address = *((MaPLMemoryAddress *)(context->scriptBuffer+context->cursorPosition));
    printLineNumber(context, sizeof(MaPLMemoryAddress));
    printf("MEMORY ADDRESS=%u\n", address);
    context->cursorPosition += sizeof(MaPLMemoryAddress);
}

void printCursorMove(MaPLDecompilerContext *context) {
    MaPLBytecodeLength move = *((MaPLBytecodeLength *)(context->scriptBuffer+context->cursorPosition));
    printLineNumber(context, sizeof(MaPLBytecodeLength));
    printf("CURSOR MOVE=%u\n", move);
    context->cursorPosition += sizeof(MaPLBytecodeLength);
}

void printSymbol(MaPLDecompilerContext *context) {
    MaPLSymbol symbol = *((MaPLSymbol *)(context->scriptBuffer+context->cursorPosition));
    printLineNumber(context, sizeof(MaPLSymbol));
    printf("SYMBOL=%u\n", symbol);
    context->cursorPosition += sizeof(MaPLSymbol);
}

MaPLParameterCount printParameterCount(MaPLDecompilerContext *context) {
    MaPLParameterCount parameterCount = *((MaPLParameterCount *)(context->scriptBuffer+context->cursorPosition));
    printLineNumber(context, sizeof(MaPLParameterCount));
    printf("PARAMETER COUNT=%u\n", parameterCount);
    context->cursorPosition += sizeof(MaPLParameterCount);
    return parameterCount;
}

void printString(MaPLDecompilerContext *context) {
    char *literal = (char *)(context->scriptBuffer+context->cursorPosition);
    MaPLBytecodeLength stringLength = strlen(literal)+1;
    printLineNumber(context, stringLength);
    printf("LITERAL STRING=\"%s\"\n", literal);
    context->cursorPosition += stringLength;
}

void evaluateFunctionInvocation(MaPLDecompilerContext *context) {
    // This function assumes that we've already advanced past the initial "function_invocation" byte.
    if (context->scriptBuffer[context->cursorPosition] == MaPLInstruction_no_op) {
        // This function is not invoked on another pointer, it's a global call.
        printIndent(context);
        printf("(GLOBAL FUNCTION CALL)\n");
        printInstruction(context);
    } else {
        printIndent(context);
        printf("(INVOKED ON POINTER)\n");
        evaluateStatement(context);
    }
    
    printSymbol(context);
 
    // Create the list of parameters.
    MaPLParameterCount paramCount = printParameterCount(context);
    for (MaPLParameterCount i = 0; i < paramCount; i++) {
        printIndent(context);
        printf("(PARAMETER #%u)\n", i);
        evaluateStatement(context);
    }
}

void evaluateSubscriptInvocation(MaPLDecompilerContext *context) {
    // This function assumes that we've already advanced past the initial "subscript_invocation" byte.
    printIndent(context);
    printf("(INVOKED ON POINTER)\n");
    evaluateStatement(context);
    
    printIndent(context);
    printf("(SUBSCRIPT INDEX)\n");
    evaluateStatement(context);
}

void evaluateStatement(MaPLDecompilerContext *context) {
    MaPLInstruction instruction =  printInstruction(context);
    context->indent++;
    switch(instruction) {
        case MaPLInstruction_char_literal: {
            u_int8_t literal = *((u_int8_t *)(context->scriptBuffer+context->cursorPosition));
            printLineNumber(context, sizeof(u_int8_t));
            printf("LITERAL CHAR=%u\n", literal);
            context->cursorPosition += sizeof(u_int8_t);
        }
            break;
        case MaPLInstruction_int32_literal: {
            int32_t literal = *((int32_t *)(context->scriptBuffer+context->cursorPosition));
            printLineNumber(context, sizeof(u_int32_t));
            printf("LITERAL INT32=%d\n", literal);
            context->cursorPosition += sizeof(int32_t);
        }
            break;
        case MaPLInstruction_int64_literal: {
            int64_t literal = *((int64_t *)(context->scriptBuffer+context->cursorPosition));
            printLineNumber(context, sizeof(int64_t));
            printf("LITERAL INT64=%lld\n", literal);
            context->cursorPosition += sizeof(int64_t);
        }
            break;
        case MaPLInstruction_uint32_literal: {
            u_int32_t literal = *((u_int32_t *)(context->scriptBuffer+context->cursorPosition));
            printLineNumber(context, sizeof(u_int32_t));
            printf("LITERAL UINT32=%u\n", literal);
            context->cursorPosition += sizeof(u_int32_t);
        }
            break;
        case MaPLInstruction_uint64_literal: {
            u_int64_t literal = *((u_int64_t *)(context->scriptBuffer+context->cursorPosition));
            printLineNumber(context, sizeof(u_int64_t));
            printf("LITERAL UINT64=%llu\n", literal);
            context->cursorPosition += sizeof(u_int64_t);
        }
            break;
        case MaPLInstruction_float32_literal: {
            float literal = *((float *)(context->scriptBuffer+context->cursorPosition));
            printLineNumber(context, sizeof(float));
            printf("LITERAL FLOAT32=%g\n", literal);
            context->cursorPosition += sizeof(float);
        }
            break;
        case MaPLInstruction_float64_literal: {
            double literal = *((double *)(context->scriptBuffer+context->cursorPosition));
            printLineNumber(context, sizeof(double));
            printf("LITERAL FLOAT64=%g\n", literal);
            context->cursorPosition += sizeof(double);
        }
            break;
        case MaPLInstruction_string_literal: // Intentional fallthrough.
        case MaPLInstruction_metadata: // Intentional fallthrough.
        case MaPLInstruction_debug_delete_variable:
            printString(context);
            break;
        case MaPLInstruction_debug_line: {
            MaPLLineNumber lineNumber = *((MaPLLineNumber *)(context->scriptBuffer+context->cursorPosition));
            printLineNumber(context, sizeof(MaPLLineNumber));
            printf("LINE NUMBER=%u\n", lineNumber);
            context->cursorPosition += sizeof(MaPLLineNumber);
        }
            break;
        case MaPLInstruction_literal_true:
        case MaPLInstruction_literal_false:
        case MaPLInstruction_literal_null:
        case MaPLInstruction_program_exit:
            // No-op.
            break;
        case MaPLInstruction_char_variable: // Intentional fallthrough.
        case MaPLInstruction_int32_variable: // Intentional fallthrough.
        case MaPLInstruction_int64_variable: // Intentional fallthrough.
        case MaPLInstruction_uint32_variable: // Intentional fallthrough.
        case MaPLInstruction_uint64_variable: // Intentional fallthrough.
        case MaPLInstruction_float32_variable: // Intentional fallthrough.
        case MaPLInstruction_float64_variable: // Intentional fallthrough.
        case MaPLInstruction_boolean_variable: // Intentional fallthrough.
        case MaPLInstruction_pointer_variable: // Intentional fallthrough.
        case MaPLInstruction_string_variable:
            printMemoryAddress(context);
            break;
        case MaPLInstruction_char_add: // Intentional fallthrough.
        case MaPLInstruction_char_subtract: // Intentional fallthrough.
        case MaPLInstruction_char_divide: // Intentional fallthrough.
        case MaPLInstruction_char_multiply: // Intentional fallthrough.
        case MaPLInstruction_char_modulo: // Intentional fallthrough.
        case MaPLInstruction_char_bitwise_and: // Intentional fallthrough.
        case MaPLInstruction_char_bitwise_or: // Intentional fallthrough.
        case MaPLInstruction_char_bitwise_xor: // Intentional fallthrough.
        case MaPLInstruction_char_bitwise_shift_left: // Intentional fallthrough.
        case MaPLInstruction_char_bitwise_shift_right: // Intentional fallthrough.
        case MaPLInstruction_int32_add: // Intentional fallthrough.
        case MaPLInstruction_int32_subtract: // Intentional fallthrough.
        case MaPLInstruction_int32_divide: // Intentional fallthrough.
        case MaPLInstruction_int32_multiply: // Intentional fallthrough.
        case MaPLInstruction_int32_modulo: // Intentional fallthrough.
        case MaPLInstruction_int32_bitwise_and: // Intentional fallthrough.
        case MaPLInstruction_int32_bitwise_or: // Intentional fallthrough.
        case MaPLInstruction_int32_bitwise_xor: // Intentional fallthrough.
        case MaPLInstruction_int32_bitwise_shift_left: // Intentional fallthrough.
        case MaPLInstruction_int32_bitwise_shift_right: // Intentional fallthrough.
        case MaPLInstruction_int64_add: // Intentional fallthrough.
        case MaPLInstruction_int64_subtract: // Intentional fallthrough.
        case MaPLInstruction_int64_divide: // Intentional fallthrough.
        case MaPLInstruction_int64_multiply: // Intentional fallthrough.
        case MaPLInstruction_int64_modulo: // Intentional fallthrough.
        case MaPLInstruction_int64_bitwise_and: // Intentional fallthrough.
        case MaPLInstruction_int64_bitwise_or: // Intentional fallthrough.
        case MaPLInstruction_int64_bitwise_xor: // Intentional fallthrough.
        case MaPLInstruction_int64_bitwise_shift_left: // Intentional fallthrough.
        case MaPLInstruction_int64_bitwise_shift_right: // Intentional fallthrough.
        case MaPLInstruction_uint32_add: // Intentional fallthrough.
        case MaPLInstruction_uint32_subtract: // Intentional fallthrough.
        case MaPLInstruction_uint32_divide: // Intentional fallthrough.
        case MaPLInstruction_uint32_multiply: // Intentional fallthrough.
        case MaPLInstruction_uint32_modulo: // Intentional fallthrough.
        case MaPLInstruction_uint32_bitwise_and: // Intentional fallthrough.
        case MaPLInstruction_uint32_bitwise_or: // Intentional fallthrough.
        case MaPLInstruction_uint32_bitwise_xor: // Intentional fallthrough.
        case MaPLInstruction_uint32_bitwise_shift_left: // Intentional fallthrough.
        case MaPLInstruction_uint32_bitwise_shift_right: // Intentional fallthrough.
        case MaPLInstruction_uint64_add: // Intentional fallthrough.
        case MaPLInstruction_uint64_subtract: // Intentional fallthrough.
        case MaPLInstruction_uint64_divide: // Intentional fallthrough.
        case MaPLInstruction_uint64_multiply: // Intentional fallthrough.
        case MaPLInstruction_uint64_modulo: // Intentional fallthrough.
        case MaPLInstruction_uint64_bitwise_and: // Intentional fallthrough.
        case MaPLInstruction_uint64_bitwise_or: // Intentional fallthrough.
        case MaPLInstruction_uint64_bitwise_xor: // Intentional fallthrough.
        case MaPLInstruction_uint64_bitwise_shift_left: // Intentional fallthrough.
        case MaPLInstruction_uint64_bitwise_shift_right: // Intentional fallthrough.
        case MaPLInstruction_float32_add: // Intentional fallthrough.
        case MaPLInstruction_float32_subtract: // Intentional fallthrough.
        case MaPLInstruction_float32_divide: // Intentional fallthrough.
        case MaPLInstruction_float32_multiply: // Intentional fallthrough.
        case MaPLInstruction_float32_modulo: // Intentional fallthrough.
        case MaPLInstruction_float64_add: // Intentional fallthrough.
        case MaPLInstruction_float64_subtract: // Intentional fallthrough.
        case MaPLInstruction_float64_divide: // Intentional fallthrough.
        case MaPLInstruction_float64_multiply: // Intentional fallthrough.
        case MaPLInstruction_float64_modulo: // Intentional fallthrough.
        case MaPLInstruction_logical_equality_char: // Intentional fallthrough.
        case MaPLInstruction_logical_equality_int32: // Intentional fallthrough.
        case MaPLInstruction_logical_equality_int64: // Intentional fallthrough.
        case MaPLInstruction_logical_equality_uint32: // Intentional fallthrough.
        case MaPLInstruction_logical_equality_uint64: // Intentional fallthrough.
        case MaPLInstruction_logical_equality_float32: // Intentional fallthrough.
        case MaPLInstruction_logical_equality_float64: // Intentional fallthrough.
        case MaPLInstruction_logical_equality_boolean: // Intentional fallthrough.
        case MaPLInstruction_logical_equality_string: // Intentional fallthrough.
        case MaPLInstruction_logical_equality_pointer: // Intentional fallthrough.
        case MaPLInstruction_logical_inequality_char: // Intentional fallthrough.
        case MaPLInstruction_logical_inequality_int32: // Intentional fallthrough.
        case MaPLInstruction_logical_inequality_int64: // Intentional fallthrough.
        case MaPLInstruction_logical_inequality_uint32: // Intentional fallthrough.
        case MaPLInstruction_logical_inequality_uint64: // Intentional fallthrough.
        case MaPLInstruction_logical_inequality_float32: // Intentional fallthrough.
        case MaPLInstruction_logical_inequality_float64: // Intentional fallthrough.
        case MaPLInstruction_logical_inequality_boolean: // Intentional fallthrough.
        case MaPLInstruction_logical_inequality_string: // Intentional fallthrough.
        case MaPLInstruction_logical_inequality_pointer: // Intentional fallthrough.
        case MaPLInstruction_logical_less_than_char: // Intentional fallthrough.
        case MaPLInstruction_logical_less_than_int32: // Intentional fallthrough.
        case MaPLInstruction_logical_less_than_int64: // Intentional fallthrough.
        case MaPLInstruction_logical_less_than_uint32: // Intentional fallthrough.
        case MaPLInstruction_logical_less_than_uint64: // Intentional fallthrough.
        case MaPLInstruction_logical_less_than_float32: // Intentional fallthrough.
        case MaPLInstruction_logical_less_than_float64: // Intentional fallthrough.
        case MaPLInstruction_logical_less_than_equal_char: // Intentional fallthrough.
        case MaPLInstruction_logical_less_than_equal_int32: // Intentional fallthrough.
        case MaPLInstruction_logical_less_than_equal_int64: // Intentional fallthrough.
        case MaPLInstruction_logical_less_than_equal_uint32: // Intentional fallthrough.
        case MaPLInstruction_logical_less_than_equal_uint64: // Intentional fallthrough.
        case MaPLInstruction_logical_less_than_equal_float32: // Intentional fallthrough.
        case MaPLInstruction_logical_less_than_equal_float64: // Intentional fallthrough.
        case MaPLInstruction_logical_greater_than_char: // Intentional fallthrough.
        case MaPLInstruction_logical_greater_than_int32: // Intentional fallthrough.
        case MaPLInstruction_logical_greater_than_int64: // Intentional fallthrough.
        case MaPLInstruction_logical_greater_than_uint32: // Intentional fallthrough.
        case MaPLInstruction_logical_greater_than_uint64: // Intentional fallthrough.
        case MaPLInstruction_logical_greater_than_float32: // Intentional fallthrough.
        case MaPLInstruction_logical_greater_than_float64: // Intentional fallthrough.
        case MaPLInstruction_logical_greater_than_equal_char: // Intentional fallthrough.
        case MaPLInstruction_logical_greater_than_equal_int32: // Intentional fallthrough.
        case MaPLInstruction_logical_greater_than_equal_int64: // Intentional fallthrough.
        case MaPLInstruction_logical_greater_than_equal_uint32: // Intentional fallthrough.
        case MaPLInstruction_logical_greater_than_equal_uint64: // Intentional fallthrough.
        case MaPLInstruction_logical_greater_than_equal_float32: // Intentional fallthrough.
        case MaPLInstruction_logical_greater_than_equal_float64: // Intentional fallthrough.
        case MaPLInstruction_logical_and: // Intentional fallthrough.
        case MaPLInstruction_logical_or: // Intentional fallthrough.
        case MaPLInstruction_pointer_null_coalescing: // Intentional fallthrough.
        case MaPLInstruction_string_concat:
            // Evaluate binary operands.
            evaluateStatement(context);
            evaluateStatement(context);
            break;
        case MaPLInstruction_char_bitwise_negation: // Intentional fallthrough.
        case MaPLInstruction_int32_numeric_negation: // Intentional fallthrough.
        case MaPLInstruction_int32_bitwise_negation: // Intentional fallthrough.
        case MaPLInstruction_int64_numeric_negation: // Intentional fallthrough.
        case MaPLInstruction_int64_bitwise_negation: // Intentional fallthrough.
        case MaPLInstruction_uint32_bitwise_negation: // Intentional fallthrough.
        case MaPLInstruction_uint64_bitwise_negation: // Intentional fallthrough.
        case MaPLInstruction_float32_numeric_negation: // Intentional fallthrough.
        case MaPLInstruction_float64_numeric_negation: // Intentional fallthrough.
        case MaPLInstruction_logical_negation:
            // Evaluate unary operand.
            return evaluateStatement(context);
        case MaPLInstruction_char_function_invocation: // Intentional fallthrough.
        case MaPLInstruction_int32_function_invocation: // Intentional fallthrough.
        case MaPLInstruction_int64_function_invocation: // Intentional fallthrough.
        case MaPLInstruction_uint32_function_invocation: // Intentional fallthrough.
        case MaPLInstruction_uint64_function_invocation: // Intentional fallthrough.
        case MaPLInstruction_float32_function_invocation: // Intentional fallthrough.
        case MaPLInstruction_float64_function_invocation: // Intentional fallthrough.
        case MaPLInstruction_boolean_function_invocation: // Intentional fallthrough.
        case MaPLInstruction_pointer_function_invocation: // Intentional fallthrough.
        case MaPLInstruction_string_function_invocation: // Intentional fallthrough.
        case MaPLInstruction_unused_return_function_invocation:
            evaluateFunctionInvocation(context);
            break;
        case MaPLInstruction_char_subscript_invocation: // Intentional fallthrough.
        case MaPLInstruction_int32_subscript_invocation: // Intentional fallthrough.
        case MaPLInstruction_int64_subscript_invocation: // Intentional fallthrough.
        case MaPLInstruction_uint32_subscript_invocation: // Intentional fallthrough.
        case MaPLInstruction_uint64_subscript_invocation: // Intentional fallthrough.
        case MaPLInstruction_float32_subscript_invocation: // Intentional fallthrough.
        case MaPLInstruction_float64_subscript_invocation: // Intentional fallthrough.
        case MaPLInstruction_boolean_subscript_invocation: // Intentional fallthrough.
        case MaPLInstruction_pointer_subscript_invocation: // Intentional fallthrough.
        case MaPLInstruction_string_subscript_invocation:
            evaluateSubscriptInvocation(context);
            break;
        case MaPLInstruction_char_ternary_conditional: // Intentional fallthrough.
        case MaPLInstruction_int32_ternary_conditional: // Intentional fallthrough.
        case MaPLInstruction_int64_ternary_conditional: // Intentional fallthrough.
        case MaPLInstruction_uint32_ternary_conditional: // Intentional fallthrough.
        case MaPLInstruction_uint64_ternary_conditional: // Intentional fallthrough.
        case MaPLInstruction_float32_ternary_conditional: // Intentional fallthrough.
        case MaPLInstruction_float64_ternary_conditional: // Intentional fallthrough.
        case MaPLInstruction_boolean_ternary_conditional: // Intentional fallthrough.
        case MaPLInstruction_pointer_ternary_conditional: // Intentional fallthrough.
        case MaPLInstruction_string_ternary_conditional:
            // Evaluate conditional.
            evaluateStatement(context);
            // Evaluate branches.
            evaluateStatement(context);
            evaluateStatement(context);
            break;
        case MaPLInstruction_char_typecast: // Intentional fallthrough.
        case MaPLInstruction_int32_typecast: // Intentional fallthrough.
        case MaPLInstruction_int64_typecast: // Intentional fallthrough.
        case MaPLInstruction_uint32_typecast: // Intentional fallthrough.
        case MaPLInstruction_uint64_typecast: // Intentional fallthrough.
        case MaPLInstruction_float32_typecast: // Intentional fallthrough.
        case MaPLInstruction_float64_typecast: // Intentional fallthrough.
        case MaPLInstruction_boolean_typecast: // Intentional fallthrough.
        case MaPLInstruction_string_typecast:
            // Evaluate casted expression.
            evaluateStatement(context);
            break;
        case MaPLInstruction_char_assign: // Intentional fallthrough.
        case MaPLInstruction_int32_assign: // Intentional fallthrough.
        case MaPLInstruction_int64_assign: // Intentional fallthrough.
        case MaPLInstruction_uint32_assign: // Intentional fallthrough.
        case MaPLInstruction_uint64_assign: // Intentional fallthrough.
        case MaPLInstruction_float32_assign: // Intentional fallthrough.
        case MaPLInstruction_float64_assign: // Intentional fallthrough.
        case MaPLInstruction_boolean_assign: // Intentional fallthrough.
        case MaPLInstruction_string_assign: // Intentional fallthrough.
        case MaPLInstruction_pointer_assign:
            printMemoryAddress(context);
            evaluateStatement(context);
            break;
        case MaPLInstruction_assign_subscript:
            printIndent(context);
            printf("(INVOKED ON POINTER)\n");
            evaluateStatement(context);
            
            printIndent(context);
            printf("(SUBSCRIPT INDEX)\n");
            evaluateStatement(context);
            
            printIndent(context);
            printf("(OPERATOR ASSIGN)\n");
            printInstruction(context);
            
            printIndent(context);
            printf("(ASSIGNED EXPRESSION)\n");
            evaluateStatement(context);
            break;
        case MaPLInstruction_assign_property: {
            if (context->scriptBuffer[context->cursorPosition] == MaPLInstruction_no_op) {
                // This property is not invoked on another pointer, it's a global call.
                printIndent(context);
                printf("(GLOBAL PROPERTY ASSIGN)\n");
                printInstruction(context);
            } else {
                printIndent(context);
                printf("(INVOKED ON POINTER)\n");
                evaluateStatement(context);
            }
            
            printSymbol(context);
            
            printIndent(context);
            printf("(OPERATOR ASSIGN)\n");
            printInstruction(context);
            
            printIndent(context);
            printf("(ASSIGNED EXPRESSION)\n");
            evaluateStatement(context);
        }
            break;
        case MaPLInstruction_conditional:
            evaluateStatement(context);
            printCursorMove(context);
            break;
        case MaPLInstruction_cursor_move_forward: // Intentional fallthrough.
        case MaPLInstruction_cursor_move_back:
            printCursorMove(context);
            break;
        case MaPLInstruction_debug_update_variable:
            printString(context);
            evaluateStatement(context);
            break;
        default:
            fputs("Error: Malformed bytecode.\n", stderr);
            exit(1);
    }
    context->indent--;
}

void printDecompilationOfBytecode(const u_int8_t *bytes, MaPLBytecodeLength length) {
    MaPLDecompilerContext context;
    context.scriptBuffer = bytes;
    printf("(PRIMITIVE STACK BYTE SIZE)\n");
    printMemoryAddress(&context);
    printf("(ALLOCATED STACK INDEX SIZE)\n");
    printMemoryAddress(&context);
    while (context.cursorPosition < length) {
        evaluateStatement(&context);
    }
}

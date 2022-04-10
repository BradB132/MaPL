//
//  MaPLBytecodeConstants.h
//
//  Created by Brad Bambara on 2/5/22.
//

#ifndef MaPLBytecodeConstants_h
#define MaPLBytecodeConstants_h

#include "MaPLTypedefs.h"

// Byte sizes for MaPL bytecode.
typedef u_int16_t MaPLMemoryAddress;
typedef u_int16_t MaPLCursorMove;

// Bytecodes are sorted by return type.
enum MaPLInstruction : u_int8_t {
    MaPLInstruction_placeholder = 0,
    
    // INT32
    MaPLInstruction_int32_literal = 1,
    MaPLInstruction_int32_variable = 2,
    MaPLInstruction_int32_add = 3,
    MaPLInstruction_int32_subtract = 4,
    MaPLInstruction_int32_divide = 5,
    MaPLInstruction_int32_multiply = 6,
    MaPLInstruction_int32_modulo = 7,
    MaPLInstruction_int32_numeric_negation = 8,
    MaPLInstruction_int32_bitwise_and = 9,
    MaPLInstruction_int32_bitwise_or = 10,
    MaPLInstruction_int32_bitwise_xor = 11,
    MaPLInstruction_int32_bitwise_negation = 12,
    MaPLInstruction_int32_bitwise_shift_left = 13,
    MaPLInstruction_int32_bitwise_shift_right = 14,
    MaPLInstruction_int32_function_invocation = 15,
    MaPLInstruction_int32_subscript_invocation = 16,
    MaPLInstruction_int32_ternary_conditional = 17,
    MaPLInstruction_int32_typecast = 18,
    
    // FLOAT32
    MaPLInstruction_float32_literal = 19,
    MaPLInstruction_float32_variable = 20,
    MaPLInstruction_float32_add = 21,
    MaPLInstruction_float32_subtract = 22,
    MaPLInstruction_float32_divide = 23,
    MaPLInstruction_float32_multiply = 24,
    MaPLInstruction_float32_modulo = 25,
    MaPLInstruction_float32_numeric_negation = 26,
    MaPLInstruction_float32_function_invocation = 27,
    MaPLInstruction_float32_subscript_invocation = 28,
    MaPLInstruction_float32_ternary_conditional = 29,
    MaPLInstruction_float32_typecast = 30,
    
    // STRING
    MaPLInstruction_string_literal = 31,
    MaPLInstruction_string_variable = 32,
    MaPLInstruction_string_concat = 33,
    MaPLInstruction_string_function_invocation = 34,
    MaPLInstruction_string_subscript_invocation = 35,
    MaPLInstruction_string_ternary_conditional = 36,
    MaPLInstruction_string_typecast = 37,
    
    // POINTER
    MaPLInstruction_literal_null = 38,
    MaPLInstruction_pointer_variable = 39,
    MaPLInstruction_pointer_null_coalescing = 40,
    MaPLInstruction_pointer_function_invocation = 41,
    MaPLInstruction_pointer_subscript_invocation = 42,
    MaPLInstruction_pointer_ternary_conditional = 43,
    
    // BOOLEAN
    MaPLInstruction_literal_true = 44,
    MaPLInstruction_literal_false = 45,
    MaPLInstruction_boolean_variable = 46,
    MaPLInstruction_boolean_function_invocation = 47,
    MaPLInstruction_boolean_subscript_invocation = 48,
    MaPLInstruction_boolean_ternary_conditional = 49,
    MaPLInstruction_boolean_typecast = 50,
    MaPLInstruction_logical_equality_char = 51,
    MaPLInstruction_logical_equality_int32 = 52,
    MaPLInstruction_logical_equality_int64 = 53,
    MaPLInstruction_logical_equality_uint32 = 54,
    MaPLInstruction_logical_equality_uint64 = 55,
    MaPLInstruction_logical_equality_float32 = 56,
    MaPLInstruction_logical_equality_float64 = 57,
    MaPLInstruction_logical_equality_boolean = 58,
    MaPLInstruction_logical_equality_string = 59,
    MaPLInstruction_logical_equality_pointer = 60,
    MaPLInstruction_logical_inequality_char = 61,
    MaPLInstruction_logical_inequality_int32 = 62,
    MaPLInstruction_logical_inequality_int64 = 63,
    MaPLInstruction_logical_inequality_uint32 = 64,
    MaPLInstruction_logical_inequality_uint64 = 65,
    MaPLInstruction_logical_inequality_float32 = 66,
    MaPLInstruction_logical_inequality_float64 = 67,
    MaPLInstruction_logical_inequality_boolean = 68,
    MaPLInstruction_logical_inequality_string = 69,
    MaPLInstruction_logical_inequality_pointer = 70,
    MaPLInstruction_logical_less_than_char = 71,
    MaPLInstruction_logical_less_than_int32 = 72,
    MaPLInstruction_logical_less_than_int64 = 73,
    MaPLInstruction_logical_less_than_uint32 = 74,
    MaPLInstruction_logical_less_than_uint64 = 75,
    MaPLInstruction_logical_less_than_float32 = 76,
    MaPLInstruction_logical_less_than_float64 = 77,
    MaPLInstruction_logical_less_than_equal_char = 78,
    MaPLInstruction_logical_less_than_equal_int32 = 79,
    MaPLInstruction_logical_less_than_equal_int64 = 80,
    MaPLInstruction_logical_less_than_equal_uint32 = 81,
    MaPLInstruction_logical_less_than_equal_uint64 = 82,
    MaPLInstruction_logical_less_than_equal_float32 = 83,
    MaPLInstruction_logical_less_than_equal_float64 = 84,
    MaPLInstruction_logical_greater_than_char = 85,
    MaPLInstruction_logical_greater_than_int32 = 86,
    MaPLInstruction_logical_greater_than_int64 = 87,
    MaPLInstruction_logical_greater_than_uint32 = 88,
    MaPLInstruction_logical_greater_than_uint64 = 89,
    MaPLInstruction_logical_greater_than_float32 = 90,
    MaPLInstruction_logical_greater_than_float64 = 91,
    MaPLInstruction_logical_greater_than_equal_char = 92,
    MaPLInstruction_logical_greater_than_equal_int32 = 93,
    MaPLInstruction_logical_greater_than_equal_int64 = 94,
    MaPLInstruction_logical_greater_than_equal_uint32 = 95,
    MaPLInstruction_logical_greater_than_equal_uint64 = 96,
    MaPLInstruction_logical_greater_than_equal_float32 = 97,
    MaPLInstruction_logical_greater_than_equal_float64 = 98,
    MaPLInstruction_logical_and = 99,
    MaPLInstruction_logical_or = 100,
    MaPLInstruction_logical_negation = 101,
    
    // INT64
    MaPLInstruction_int64_literal = 102,
    MaPLInstruction_int64_variable = 103,
    MaPLInstruction_int64_add = 104,
    MaPLInstruction_int64_subtract = 105,
    MaPLInstruction_int64_divide = 106,
    MaPLInstruction_int64_multiply = 107,
    MaPLInstruction_int64_modulo = 108,
    MaPLInstruction_int64_numeric_negation = 109,
    MaPLInstruction_int64_bitwise_and = 110,
    MaPLInstruction_int64_bitwise_or = 111,
    MaPLInstruction_int64_bitwise_xor = 112,
    MaPLInstruction_int64_bitwise_negation = 113,
    MaPLInstruction_int64_bitwise_shift_left = 114,
    MaPLInstruction_int64_bitwise_shift_right = 115,
    MaPLInstruction_int64_function_invocation = 116,
    MaPLInstruction_int64_subscript_invocation = 117,
    MaPLInstruction_int64_ternary_conditional = 118,
    MaPLInstruction_int64_typecast = 119,
    
    // FLOAT64
    MaPLInstruction_float64_literal = 120,
    MaPLInstruction_float64_variable = 121,
    MaPLInstruction_float64_add = 122,
    MaPLInstruction_float64_subtract = 123,
    MaPLInstruction_float64_divide = 124,
    MaPLInstruction_float64_multiply = 125,
    MaPLInstruction_float64_modulo = 126,
    MaPLInstruction_float64_numeric_negation = 127,
    MaPLInstruction_float64_function_invocation = 128,
    MaPLInstruction_float64_subscript_invocation = 129,
    MaPLInstruction_float64_ternary_conditional = 130,
    MaPLInstruction_float64_typecast = 131,
    
    // UINT32
    MaPLInstruction_uint32_literal = 132,
    MaPLInstruction_uint32_variable = 133,
    MaPLInstruction_uint32_add = 134,
    MaPLInstruction_uint32_subtract = 135,
    MaPLInstruction_uint32_divide = 136,
    MaPLInstruction_uint32_multiply = 137,
    MaPLInstruction_uint32_modulo = 138,
    MaPLInstruction_uint32_bitwise_and = 139,
    MaPLInstruction_uint32_bitwise_or = 140,
    MaPLInstruction_uint32_bitwise_xor = 141,
    MaPLInstruction_uint32_bitwise_negation = 142,
    MaPLInstruction_uint32_bitwise_shift_left = 143,
    MaPLInstruction_uint32_bitwise_shift_right = 144,
    MaPLInstruction_uint32_function_invocation = 145,
    MaPLInstruction_uint32_subscript_invocation = 146,
    MaPLInstruction_uint32_ternary_conditional = 147,
    MaPLInstruction_uint32_typecast = 148,
    
    // UINT64
    MaPLInstruction_uint64_literal = 149,
    MaPLInstruction_uint64_variable = 150,
    MaPLInstruction_uint64_add = 151,
    MaPLInstruction_uint64_subtract = 152,
    MaPLInstruction_uint64_divide = 153,
    MaPLInstruction_uint64_multiply = 154,
    MaPLInstruction_uint64_modulo = 155,
    MaPLInstruction_uint64_bitwise_and = 156,
    MaPLInstruction_uint64_bitwise_or = 157,
    MaPLInstruction_uint64_bitwise_xor = 158,
    MaPLInstruction_uint64_bitwise_negation = 159,
    MaPLInstruction_uint64_bitwise_shift_left = 160,
    MaPLInstruction_uint64_bitwise_shift_right = 161,
    MaPLInstruction_uint64_function_invocation = 162,
    MaPLInstruction_uint64_subscript_invocation = 163,
    MaPLInstruction_uint64_ternary_conditional = 164,
    MaPLInstruction_uint64_typecast = 165,
    
    // CHAR
    MaPLInstruction_char_literal = 166,
    MaPLInstruction_char_variable = 167,
    MaPLInstruction_char_add = 168,
    MaPLInstruction_char_subtract = 169,
    MaPLInstruction_char_divide = 170,
    MaPLInstruction_char_multiply = 171,
    MaPLInstruction_char_modulo = 172,
    MaPLInstruction_char_bitwise_and = 173,
    MaPLInstruction_char_bitwise_or = 174,
    MaPLInstruction_char_bitwise_xor = 175,
    MaPLInstruction_char_bitwise_negation = 176,
    MaPLInstruction_char_bitwise_shift_left = 177,
    MaPLInstruction_char_bitwise_shift_right = 178,
    MaPLInstruction_char_function_invocation = 179,
    MaPLInstruction_char_subscript_invocation = 180,
    MaPLInstruction_char_ternary_conditional = 181,
    MaPLInstruction_char_typecast = 182,

    // VOID
    MaPLInstruction_unused_return_function_invocation = 183,
    MaPLInstruction_char_assign = 184,
    MaPLInstruction_int32_assign = 185,
    MaPLInstruction_int64_assign = 186,
    MaPLInstruction_uint32_assign = 187,
    MaPLInstruction_uint64_assign = 188,
    MaPLInstruction_float32_assign = 189,
    MaPLInstruction_float64_assign = 190,
    MaPLInstruction_boolean_assign = 191,
    MaPLInstruction_string_assign = 192,
    MaPLInstruction_pointer_assign = 193,
    MaPLInstruction_assign_subscript = 194,
    MaPLInstruction_assign_property = 195,

    // CONTROL FLOW
    MaPLInstruction_conditional = 196,
    MaPLInstruction_cursor_move_forward = 197,
    MaPLInstruction_cursor_move_back = 198,
    MaPLInstruction_program_exit = 199,

    // METADATA
    MaPLInstruction_metadata = 200,

    // DEBUGGING
    MaPLInstruction_debug_line = 201,
    MaPLInstruction_debug_update_variable = 202,
    MaPLInstruction_debug_delete_variable = 203,
    
    // NO-OP
    MaPLInstruction_no_op = 204,
    
    // ERROR CHECKING
    MaPLInstruction_error = 205,
};

#endif /* MaPLBytecodeConstants_h */

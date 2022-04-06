//
//  MaPLBytecodeConstants.h
//
//  Created by Brad Bambara on 2/5/22.
//

#ifndef MaPLBytecodeConstants_h
#define MaPLBytecodeConstants_h

// Byte sizes for MaPL bytecode.
typedef u_int8_t MaPLParameterCount;
typedef u_int16_t MaPLMemoryAddress;
typedef u_int16_t MaPLCursorMove;
typedef u_int16_t MaPLSymbol;
typedef u_int16_t MaPLLineNumber;

// Bytecodes are sorted by return type.
enum MaPLInstruction : u_int8_t {
    MaPLInstruction_placeholder = 0,
    
    // CHAR
    MaPLInstruction_char_literal = 1,
    MaPLInstruction_char_variable = 2,
    MaPLInstruction_char_add = 3,
    MaPLInstruction_char_subtract = 4,
    MaPLInstruction_char_divide = 5,
    MaPLInstruction_char_multiply = 6,
    MaPLInstruction_char_modulo = 7,
    MaPLInstruction_char_bitwise_and = 8,
    MaPLInstruction_char_bitwise_or = 9,
    MaPLInstruction_char_bitwise_xor = 10,
    MaPLInstruction_char_bitwise_negation = 11,
    MaPLInstruction_char_bitwise_shift_left = 12,
    MaPLInstruction_char_bitwise_shift_right = 13,
    MaPLInstruction_char_function_invocation = 14,
    MaPLInstruction_char_subscript_invocation = 15,
    MaPLInstruction_char_ternary_conditional = 16,
    MaPLInstruction_char_typecast = 17,
    
    // INT32
    MaPLInstruction_int32_literal = 18,
    MaPLInstruction_int32_variable = 19,
    MaPLInstruction_int32_add = 20,
    MaPLInstruction_int32_subtract = 21,
    MaPLInstruction_int32_divide = 22,
    MaPLInstruction_int32_multiply = 23,
    MaPLInstruction_int32_modulo = 24,
    MaPLInstruction_int32_numeric_negation = 25,
    MaPLInstruction_int32_bitwise_and = 26,
    MaPLInstruction_int32_bitwise_or = 27,
    MaPLInstruction_int32_bitwise_xor = 28,
    MaPLInstruction_int32_bitwise_negation = 29,
    MaPLInstruction_int32_bitwise_shift_left = 30,
    MaPLInstruction_int32_bitwise_shift_right = 31,
    MaPLInstruction_int32_function_invocation = 32,
    MaPLInstruction_int32_subscript_invocation = 33,
    MaPLInstruction_int32_ternary_conditional = 34,
    MaPLInstruction_int32_typecast = 35,
    
    // INT64
    MaPLInstruction_int64_literal = 36,
    MaPLInstruction_int64_variable = 37,
    MaPLInstruction_int64_add = 38,
    MaPLInstruction_int64_subtract = 39,
    MaPLInstruction_int64_divide = 40,
    MaPLInstruction_int64_multiply = 41,
    MaPLInstruction_int64_modulo = 42,
    MaPLInstruction_int64_numeric_negation = 43,
    MaPLInstruction_int64_bitwise_and = 44,
    MaPLInstruction_int64_bitwise_or = 45,
    MaPLInstruction_int64_bitwise_xor = 46,
    MaPLInstruction_int64_bitwise_negation = 47,
    MaPLInstruction_int64_bitwise_shift_left = 48,
    MaPLInstruction_int64_bitwise_shift_right = 49,
    MaPLInstruction_int64_function_invocation = 50,
    MaPLInstruction_int64_subscript_invocation = 51,
    MaPLInstruction_int64_ternary_conditional = 52,
    MaPLInstruction_int64_typecast = 53,
    
    // UINT32
    MaPLInstruction_uint32_literal = 54,
    MaPLInstruction_uint32_variable = 55,
    MaPLInstruction_uint32_add = 56,
    MaPLInstruction_uint32_subtract = 57,
    MaPLInstruction_uint32_divide = 58,
    MaPLInstruction_uint32_multiply = 59,
    MaPLInstruction_uint32_modulo = 60,
    MaPLInstruction_uint32_bitwise_and = 61,
    MaPLInstruction_uint32_bitwise_or = 62,
    MaPLInstruction_uint32_bitwise_xor = 63,
    MaPLInstruction_uint32_bitwise_negation = 64,
    MaPLInstruction_uint32_bitwise_shift_left = 65,
    MaPLInstruction_uint32_bitwise_shift_right = 66,
    MaPLInstruction_uint32_function_invocation = 67,
    MaPLInstruction_uint32_subscript_invocation = 68,
    MaPLInstruction_uint32_ternary_conditional = 69,
    MaPLInstruction_uint32_typecast = 70,
    
    // UINT64
    MaPLInstruction_uint64_literal = 71,
    MaPLInstruction_uint64_variable = 72,
    MaPLInstruction_uint64_add = 73,
    MaPLInstruction_uint64_subtract = 74,
    MaPLInstruction_uint64_divide = 75,
    MaPLInstruction_uint64_multiply = 76,
    MaPLInstruction_uint64_modulo = 77,
    MaPLInstruction_uint64_bitwise_and = 78,
    MaPLInstruction_uint64_bitwise_or = 79,
    MaPLInstruction_uint64_bitwise_xor = 80,
    MaPLInstruction_uint64_bitwise_negation = 81,
    MaPLInstruction_uint64_bitwise_shift_left = 82,
    MaPLInstruction_uint64_bitwise_shift_right = 83,
    MaPLInstruction_uint64_function_invocation = 84,
    MaPLInstruction_uint64_subscript_invocation = 85,
    MaPLInstruction_uint64_ternary_conditional = 86,
    MaPLInstruction_uint64_typecast = 87,
    
    // FLOAT32
    MaPLInstruction_float32_literal = 88,
    MaPLInstruction_float32_variable = 89,
    MaPLInstruction_float32_add = 90,
    MaPLInstruction_float32_subtract = 91,
    MaPLInstruction_float32_divide = 92,
    MaPLInstruction_float32_multiply = 93,
    MaPLInstruction_float32_modulo = 94,
    MaPLInstruction_float32_numeric_negation = 95,
    MaPLInstruction_float32_function_invocation = 96,
    MaPLInstruction_float32_subscript_invocation = 97,
    MaPLInstruction_float32_ternary_conditional = 98,
    MaPLInstruction_float32_typecast = 99,
    
    // FLOAT64
    MaPLInstruction_float64_literal = 100,
    MaPLInstruction_float64_variable = 101,
    MaPLInstruction_float64_add = 102,
    MaPLInstruction_float64_subtract = 103,
    MaPLInstruction_float64_divide = 104,
    MaPLInstruction_float64_multiply = 105,
    MaPLInstruction_float64_modulo = 106,
    MaPLInstruction_float64_numeric_negation = 107,
    MaPLInstruction_float64_function_invocation = 108,
    MaPLInstruction_float64_subscript_invocation = 109,
    MaPLInstruction_float64_ternary_conditional = 110,
    MaPLInstruction_float64_typecast = 111,
    
    // STRING
    MaPLInstruction_string_literal = 112,
    MaPLInstruction_string_variable = 113,
    MaPLInstruction_string_concat = 114,
    MaPLInstruction_string_function_invocation = 115,
    MaPLInstruction_string_subscript_invocation = 116,
    MaPLInstruction_string_ternary_conditional = 117,
    MaPLInstruction_string_typecast = 118,
    MaPLInstruction_string_typecast_from_pointer = 119,
    
    // POINTER
    MaPLInstruction_literal_null = 120,
    MaPLInstruction_pointer_variable = 121,
    MaPLInstruction_pointer_null_coalescing = 122,
    MaPLInstruction_pointer_function_invocation = 123,
    MaPLInstruction_pointer_subscript_invocation = 124,
    MaPLInstruction_pointer_ternary_conditional = 125,
    
    // BOOLEAN
    MaPLInstruction_literal_true = 126,
    MaPLInstruction_literal_false = 127,
    MaPLInstruction_boolean_variable = 128,
    MaPLInstruction_boolean_function_invocation = 129,
    MaPLInstruction_boolean_subscript_invocation = 130,
    MaPLInstruction_boolean_ternary_conditional = 131,
    MaPLInstruction_boolean_typecast = 132,
    MaPLInstruction_logical_equality_char = 133,
    MaPLInstruction_logical_equality_int32 = 134,
    MaPLInstruction_logical_equality_int64 = 135,
    MaPLInstruction_logical_equality_uint32 = 136,
    MaPLInstruction_logical_equality_uint64 = 137,
    MaPLInstruction_logical_equality_float32 = 138,
    MaPLInstruction_logical_equality_float64 = 139,
    MaPLInstruction_logical_equality_boolean = 140,
    MaPLInstruction_logical_equality_string = 141,
    MaPLInstruction_logical_equality_pointer = 142,
    MaPLInstruction_logical_inequality_char = 143,
    MaPLInstruction_logical_inequality_int32 = 144,
    MaPLInstruction_logical_inequality_int64 = 145,
    MaPLInstruction_logical_inequality_uint32 = 146,
    MaPLInstruction_logical_inequality_uint64 = 147,
    MaPLInstruction_logical_inequality_float32 = 148,
    MaPLInstruction_logical_inequality_float64 = 149,
    MaPLInstruction_logical_inequality_boolean = 150,
    MaPLInstruction_logical_inequality_string = 151,
    MaPLInstruction_logical_inequality_pointer = 152,
    MaPLInstruction_logical_less_than_char = 153,
    MaPLInstruction_logical_less_than_int32 = 154,
    MaPLInstruction_logical_less_than_int64 = 155,
    MaPLInstruction_logical_less_than_uint32 = 156,
    MaPLInstruction_logical_less_than_uint64 = 157,
    MaPLInstruction_logical_less_than_float32 = 158,
    MaPLInstruction_logical_less_than_float64 = 159,
    MaPLInstruction_logical_less_than_equal_char = 160,
    MaPLInstruction_logical_less_than_equal_int32 = 161,
    MaPLInstruction_logical_less_than_equal_int64 = 162,
    MaPLInstruction_logical_less_than_equal_uint32 = 163,
    MaPLInstruction_logical_less_than_equal_uint64 = 164,
    MaPLInstruction_logical_less_than_equal_float32 = 165,
    MaPLInstruction_logical_less_than_equal_float64 = 166,
    MaPLInstruction_logical_greater_than_char = 167,
    MaPLInstruction_logical_greater_than_int32 = 168,
    MaPLInstruction_logical_greater_than_int64 = 169,
    MaPLInstruction_logical_greater_than_uint32 = 170,
    MaPLInstruction_logical_greater_than_uint64 = 171,
    MaPLInstruction_logical_greater_than_float32 = 172,
    MaPLInstruction_logical_greater_than_float64 = 173,
    MaPLInstruction_logical_greater_than_equal_char = 174,
    MaPLInstruction_logical_greater_than_equal_int32 = 175,
    MaPLInstruction_logical_greater_than_equal_int64 = 176,
    MaPLInstruction_logical_greater_than_equal_uint32 = 177,
    MaPLInstruction_logical_greater_than_equal_uint64 = 178,
    MaPLInstruction_logical_greater_than_equal_float32 = 179,
    MaPLInstruction_logical_greater_than_equal_float64 = 180,
    MaPLInstruction_logical_and = 181,
    MaPLInstruction_logical_or = 182,
    MaPLInstruction_logical_negation = 183,

    // VOID
    MaPLInstruction_void_function_invocation = 184,
    MaPLInstruction_void_subscript_invocation = 185,
    MaPLInstruction_char_assign = 186,
    MaPLInstruction_int32_assign = 187,
    MaPLInstruction_int64_assign = 188,
    MaPLInstruction_uint32_assign = 189,
    MaPLInstruction_uint64_assign = 190,
    MaPLInstruction_float32_assign = 191,
    MaPLInstruction_float64_assign = 192,
    MaPLInstruction_boolean_assign = 193,
    MaPLInstruction_string_assign = 194,
    MaPLInstruction_pointer_assign = 195,
    MaPLInstruction_char_assign_subscript = 196,
    MaPLInstruction_int32_assign_subscript = 197,
    MaPLInstruction_int64_assign_subscript = 198,
    MaPLInstruction_uint32_assign_subscript = 199,
    MaPLInstruction_uint64_assign_subscript = 200,
    MaPLInstruction_float32_assign_subscript = 201,
    MaPLInstruction_float64_assign_subscript = 202,
    MaPLInstruction_boolean_assign_subscript = 203,
    MaPLInstruction_string_assign_subscript = 204,
    MaPLInstruction_pointer_assign_subscript = 205,
    MaPLInstruction_char_assign_property = 206,
    MaPLInstruction_int32_assign_property = 207,
    MaPLInstruction_int64_assign_property = 208,
    MaPLInstruction_uint32_assign_property = 209,
    MaPLInstruction_uint64_assign_property = 210,
    MaPLInstruction_float32_assign_property = 211,
    MaPLInstruction_float64_assign_property = 212,
    MaPLInstruction_boolean_assign_property = 213,
    MaPLInstruction_string_assign_property = 214,
    MaPLInstruction_pointer_assign_property = 215,

    // CONTROL FLOW
    MaPLInstruction_conditional = 216,
    MaPLInstruction_cursor_move_forward = 217,
    MaPLInstruction_cursor_move_back = 218,
    MaPLInstruction_program_exit = 219,

    // METADATA
    MaPLInstruction_metadata = 220,

    // DEBUGGING
    MaPLInstruction_debug_line = 221,
    MaPLInstruction_debug_update_variable = 222,
    MaPLInstruction_debug_delete_variable = 223,
    
    // NO-OP
    MaPLInstruction_no_op = 224,
    
    // CAST
    MaPLInstruction_typecast_from_char = 225,
    MaPLInstruction_typecast_from_int32 = 226,
    MaPLInstruction_typecast_from_int64 = 227,
    MaPLInstruction_typecast_from_uint32 = 228,
    MaPLInstruction_typecast_from_uint64 = 229,
    MaPLInstruction_typecast_from_float32 = 230,
    MaPLInstruction_typecast_from_float64 = 231,
    MaPLInstruction_typecast_from_boolean = 232,
    MaPLInstruction_typecast_from_string = 233,
    
    // ERROR CHECKING
    MaPLInstruction_error = 234,
};

#endif /* MaPLBytecodeConstants_h */

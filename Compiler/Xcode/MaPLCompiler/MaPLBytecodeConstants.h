//
//  MaPLBytecodeConstants.h
//  MaPLCompiler
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
struct MaPLString {
    char* stringValue;
    bool isAllocated;
};

// Bytecodes are sorted by return type.
enum MaPLInstruction : u_int8_t {
    MaPLInstruction_placeholder_or_error = 0,
    
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
    MaPLInstruction_char_typecast = 16,
    
    // INT32
    MaPLInstruction_int32_literal = 17,
    MaPLInstruction_int32_variable = 18,
    MaPLInstruction_int32_add = 19,
    MaPLInstruction_int32_subtract = 20,
    MaPLInstruction_int32_divide = 21,
    MaPLInstruction_int32_multiply = 22,
    MaPLInstruction_int32_modulo = 23,
    MaPLInstruction_int32_numeric_negation = 24,
    MaPLInstruction_int32_bitwise_and = 25,
    MaPLInstruction_int32_bitwise_or = 26,
    MaPLInstruction_int32_bitwise_xor = 27,
    MaPLInstruction_int32_bitwise_negation = 28,
    MaPLInstruction_int32_bitwise_shift_left = 29,
    MaPLInstruction_int32_bitwise_shift_right = 30,
    MaPLInstruction_int32_function_invocation = 31,
    MaPLInstruction_int32_subscript_invocation = 32,
    MaPLInstruction_int32_typecast = 33,
    
    // INT64
    MaPLInstruction_int64_literal = 34,
    MaPLInstruction_int64_variable = 35,
    MaPLInstruction_int64_add = 36,
    MaPLInstruction_int64_subtract = 37,
    MaPLInstruction_int64_divide = 38,
    MaPLInstruction_int64_multiply = 39,
    MaPLInstruction_int64_modulo = 40,
    MaPLInstruction_int64_numeric_negation = 41,
    MaPLInstruction_int64_bitwise_and = 42,
    MaPLInstruction_int64_bitwise_or = 43,
    MaPLInstruction_int64_bitwise_xor = 44,
    MaPLInstruction_int64_bitwise_negation = 45,
    MaPLInstruction_int64_bitwise_shift_left = 46,
    MaPLInstruction_int64_bitwise_shift_right = 47,
    MaPLInstruction_int64_function_invocation = 48,
    MaPLInstruction_int64_subscript_invocation = 49,
    MaPLInstruction_int64_typecast = 50,
    
    // UINT32
    MaPLInstruction_uint32_literal = 51,
    MaPLInstruction_uint32_variable = 52,
    MaPLInstruction_uint32_add = 53,
    MaPLInstruction_uint32_subtract = 54,
    MaPLInstruction_uint32_divide = 55,
    MaPLInstruction_uint32_multiply = 56,
    MaPLInstruction_uint32_modulo = 57,
    MaPLInstruction_uint32_bitwise_and = 58,
    MaPLInstruction_uint32_bitwise_or = 59,
    MaPLInstruction_uint32_bitwise_xor = 60,
    MaPLInstruction_uint32_bitwise_negation = 61,
    MaPLInstruction_uint32_bitwise_shift_left = 62,
    MaPLInstruction_uint32_bitwise_shift_right = 63,
    MaPLInstruction_uint32_function_invocation = 64,
    MaPLInstruction_uint32_subscript_invocation = 65,
    MaPLInstruction_uint32_typecast = 66,
    
    // UINT64
    MaPLInstruction_uint64_literal = 67,
    MaPLInstruction_uint64_variable = 68,
    MaPLInstruction_uint64_add = 69,
    MaPLInstruction_uint64_subtract = 70,
    MaPLInstruction_uint64_divide = 71,
    MaPLInstruction_uint64_multiply = 72,
    MaPLInstruction_uint64_modulo = 73,
    MaPLInstruction_uint64_bitwise_and = 74,
    MaPLInstruction_uint64_bitwise_or = 75,
    MaPLInstruction_uint64_bitwise_xor = 76,
    MaPLInstruction_uint64_bitwise_negation = 77,
    MaPLInstruction_uint64_bitwise_shift_left = 78,
    MaPLInstruction_uint64_bitwise_shift_right = 79,
    MaPLInstruction_uint64_function_invocation = 80,
    MaPLInstruction_uint64_subscript_invocation = 81,
    MaPLInstruction_uint64_typecast = 82,
    
    // FLOAT32
    MaPLInstruction_float32_literal = 83,
    MaPLInstruction_float32_variable = 84,
    MaPLInstruction_float32_add = 85,
    MaPLInstruction_float32_subtract = 86,
    MaPLInstruction_float32_divide = 87,
    MaPLInstruction_float32_multiply = 88,
    MaPLInstruction_float32_modulo = 89,
    MaPLInstruction_float32_numeric_negation = 90,
    MaPLInstruction_float32_function_invocation = 91,
    MaPLInstruction_float32_subscript_invocation = 92,
    MaPLInstruction_float32_typecast = 93,
    
    // FLOAT64
    MaPLInstruction_float64_literal = 94,
    MaPLInstruction_float64_variable = 95,
    MaPLInstruction_float64_add = 96,
    MaPLInstruction_float64_subtract = 97,
    MaPLInstruction_float64_divide = 98,
    MaPLInstruction_float64_multiply = 99,
    MaPLInstruction_float64_modulo = 100,
    MaPLInstruction_float64_numeric_negation = 101,
    MaPLInstruction_float64_function_invocation = 102,
    MaPLInstruction_float64_subscript_invocation = 103,
    MaPLInstruction_float64_typecast = 104,
    
    // STRING
    MaPLInstruction_string_literal = 105,
    MaPLInstruction_string_variable = 106,
    MaPLInstruction_string_concat = 107,
    MaPLInstruction_string_function_invocation = 108,
    MaPLInstruction_string_subscript_invocation = 109,
    MaPLInstruction_string_typecast = 110,
    MaPLInstruction_string_typecast_from_pointer = 111,
    
    // POINTER
    MaPLInstruction_literal_null = 112,
    MaPLInstruction_pointer_variable = 113,
    MaPLInstruction_pointer_function_invocation = 114,
    MaPLInstruction_pointer_subscript_invocation = 115,
    
    // BOOLEAN
    MaPLInstruction_literal_true = 116,
    MaPLInstruction_literal_false = 117,
    MaPLInstruction_boolean_variable = 118,
    MaPLInstruction_boolean_function_invocation = 119,
    MaPLInstruction_boolean_subscript_invocation = 120,
    MaPLInstruction_boolean_typecast = 121,
    MaPLInstruction_logical_equality_char = 122,
    MaPLInstruction_logical_equality_int32 = 123,
    MaPLInstruction_logical_equality_int64 = 124,
    MaPLInstruction_logical_equality_uint32 = 125,
    MaPLInstruction_logical_equality_uint64 = 126,
    MaPLInstruction_logical_equality_float32 = 127,
    MaPLInstruction_logical_equality_float64 = 128,
    MaPLInstruction_logical_equality_boolean = 129,
    MaPLInstruction_logical_equality_string = 130,
    MaPLInstruction_logical_equality_pointer = 131,
    MaPLInstruction_logical_inequality_char = 132,
    MaPLInstruction_logical_inequality_int32 = 133,
    MaPLInstruction_logical_inequality_int64 = 134,
    MaPLInstruction_logical_inequality_uint32 = 135,
    MaPLInstruction_logical_inequality_uint64 = 136,
    MaPLInstruction_logical_inequality_float32 = 137,
    MaPLInstruction_logical_inequality_float64 = 138,
    MaPLInstruction_logical_inequality_boolean = 139,
    MaPLInstruction_logical_inequality_string = 140,
    MaPLInstruction_logical_inequality_pointer = 141,
    MaPLInstruction_logical_less_than_char = 142,
    MaPLInstruction_logical_less_than_int32 = 143,
    MaPLInstruction_logical_less_than_int64 = 144,
    MaPLInstruction_logical_less_than_uint32 = 145,
    MaPLInstruction_logical_less_than_uint64 = 146,
    MaPLInstruction_logical_less_than_float32 = 147,
    MaPLInstruction_logical_less_than_float64 = 148,
    MaPLInstruction_logical_less_than_equal_char = 149,
    MaPLInstruction_logical_less_than_equal_int32 = 150,
    MaPLInstruction_logical_less_than_equal_int64 = 151,
    MaPLInstruction_logical_less_than_equal_uint32 = 152,
    MaPLInstruction_logical_less_than_equal_uint64 = 153,
    MaPLInstruction_logical_less_than_equal_float32 = 154,
    MaPLInstruction_logical_less_than_equal_float64 = 155,
    MaPLInstruction_logical_greater_than_char = 156,
    MaPLInstruction_logical_greater_than_int32 = 157,
    MaPLInstruction_logical_greater_than_int64 = 158,
    MaPLInstruction_logical_greater_than_uint32 = 159,
    MaPLInstruction_logical_greater_than_uint64 = 160,
    MaPLInstruction_logical_greater_than_float32 = 161,
    MaPLInstruction_logical_greater_than_float64 = 162,
    MaPLInstruction_logical_greater_than_equal_char = 163,
    MaPLInstruction_logical_greater_than_equal_int32 = 164,
    MaPLInstruction_logical_greater_than_equal_int64 = 165,
    MaPLInstruction_logical_greater_than_equal_uint32 = 166,
    MaPLInstruction_logical_greater_than_equal_uint64 = 167,
    MaPLInstruction_logical_greater_than_equal_float32 = 168,
    MaPLInstruction_logical_greater_than_equal_float64 = 169,
    MaPLInstruction_logical_and = 170,
    MaPLInstruction_logical_or = 171,
    MaPLInstruction_logical_negation = 172,

    // VOID
    MaPLInstruction_void_function_invocation = 173,
    MaPLInstruction_void_subscript_invocation = 174,
    MaPLInstruction_char_assign = 175,
    MaPLInstruction_int32_assign = 176,
    MaPLInstruction_int64_assign = 177,
    MaPLInstruction_uint32_assign = 178,
    MaPLInstruction_uint64_assign = 179,
    MaPLInstruction_float32_assign = 180,
    MaPLInstruction_float64_assign = 181,
    MaPLInstruction_boolean_assign = 182,
    MaPLInstruction_string_assign = 183,
    MaPLInstruction_pointer_assign = 184,
    MaPLInstruction_char_assign_subscript = 185,
    MaPLInstruction_int32_assign_subscript = 186,
    MaPLInstruction_int64_assign_subscript = 187,
    MaPLInstruction_uint32_assign_subscript = 188,
    MaPLInstruction_uint64_assign_subscript = 189,
    MaPLInstruction_float32_assign_subscript = 190,
    MaPLInstruction_float64_assign_subscript = 191,
    MaPLInstruction_boolean_assign_subscript = 192,
    MaPLInstruction_string_assign_subscript = 193,
    MaPLInstruction_pointer_assign_subscript = 194,
    MaPLInstruction_char_assign_property = 195,
    MaPLInstruction_int32_assign_property = 196,
    MaPLInstruction_int64_assign_property = 197,
    MaPLInstruction_uint32_assign_property = 198,
    MaPLInstruction_uint64_assign_property = 199,
    MaPLInstruction_float32_assign_property = 200,
    MaPLInstruction_float64_assign_property = 201,
    MaPLInstruction_boolean_assign_property = 202,
    MaPLInstruction_string_assign_property = 203,
    MaPLInstruction_pointer_assign_property = 204,

    // CONTROL FLOW
    MaPLInstruction_conditional = 205,
    MaPLInstruction_ternary_conditional = 206,
    MaPLInstruction_null_coalescing = 207,
    MaPLInstruction_cursor_move_forward = 208,
    MaPLInstruction_cursor_move_back = 209,
    MaPLInstruction_no_op = 210,
    MaPLInstruction_program_exit = 211,

    // CAST
    MaPLInstruction_typecast_from_char = 212,
    MaPLInstruction_typecast_from_int32 = 213,
    MaPLInstruction_typecast_from_int64 = 214,
    MaPLInstruction_typecast_from_uint32 = 215,
    MaPLInstruction_typecast_from_uint64 = 216,
    MaPLInstruction_typecast_from_float32 = 217,
    MaPLInstruction_typecast_from_float64 = 218,
    MaPLInstruction_typecast_from_boolean = 219,
    MaPLInstruction_typecast_from_string = 220,

    // METADATA
    MaPLInstruction_metadata = 221,

    // DEBUGGING
    MaPLInstruction_debug_line = 222,
    MaPLInstruction_debug_update_variable = 223,
    MaPLInstruction_debug_delete_variable = 224,
};

#endif /* MaPLBytecodeConstants_h */

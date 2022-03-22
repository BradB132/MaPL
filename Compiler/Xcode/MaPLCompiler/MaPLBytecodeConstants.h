//
//  MaPLBytecodeConstants.h
//  MaPLCompiler
//
//  Created by Brad Bambara on 2/5/22.
//

#ifndef MaPLBytecodeConstants_h
#define MaPLBytecodeConstants_h

// Byte sizes for MaPL bytecode.
typedef u_int8_t MaPL_ParemeterCount;
typedef u_int16_t MaPL_MemoryAddress;
typedef u_int16_t MaPL_CursorMove;
typedef u_int16_t MaPL_Symbol;
struct MaPL_String {
    char* stringValue;
    bool isAllocated;
};

enum MaPLInstruction : u_int8_t {
    MaPLInstruction_placeholder_or_error = 0,
    
    //PRIMITIVE LITERALS
    MaPLInstruction_literal_int8 = 1,
    MaPLInstruction_literal_int16 = 2,
    MaPLInstruction_literal_int32 = 3,
    MaPLInstruction_literal_int64 = 4,
    MaPLInstruction_literal_uint8 = 5,
    MaPLInstruction_literal_uint16 = 6,
    MaPLInstruction_literal_uint32 = 7,
    MaPLInstruction_literal_uint64 = 8,
    MaPLInstruction_literal_float32 = 9,
    MaPLInstruction_literal_float64 = 10,
    MaPLInstruction_literal_boolean_true = 11,
    MaPLInstruction_literal_boolean_false = 12,
    MaPLInstruction_literal_string = 13,
    MaPLInstruction_literal_null = 14,

    //VARIABLE RETRIEVAL
    MaPLInstruction_variable_int8 = 15,
    MaPLInstruction_variable_int16 = 16,
    MaPLInstruction_variable_int32 = 17,
    MaPLInstruction_variable_int64 = 18,
    MaPLInstruction_variable_uint8 = 19,
    MaPLInstruction_variable_uint16 = 20,
    MaPLInstruction_variable_uint32 = 21,
    MaPLInstruction_variable_uint64 = 22,
    MaPLInstruction_variable_float32 = 23,
    MaPLInstruction_variable_float64 = 24,
    MaPLInstruction_variable_boolean = 25,
    MaPLInstruction_variable_string = 26,
    MaPLInstruction_variable_pointer = 27,

    //NUMERIC LOGIC
    MaPLInstruction_int8_logical_equality = 28,
    MaPLInstruction_int16_logical_equality = 29,
    MaPLInstruction_int32_logical_equality = 30,
    MaPLInstruction_int64_logical_equality = 31,
    MaPLInstruction_uint8_logical_equality = 32,
    MaPLInstruction_uint16_logical_equality = 33,
    MaPLInstruction_uint32_logical_equality = 34,
    MaPLInstruction_uint64_logical_equality = 35,
    MaPLInstruction_float32_logical_equality = 36,
    MaPLInstruction_float64_logical_equality = 37,
    MaPLInstruction_int8_logical_inequality = 38,
    MaPLInstruction_int16_logical_inequality = 39,
    MaPLInstruction_int32_logical_inequality = 40,
    MaPLInstruction_int64_logical_inequality = 41,
    MaPLInstruction_uint8_logical_inequality = 42,
    MaPLInstruction_uint16_logical_inequality = 43,
    MaPLInstruction_uint32_logical_inequality = 44,
    MaPLInstruction_uint64_logical_inequality = 45,
    MaPLInstruction_float32_logical_inequality = 46,
    MaPLInstruction_float64_logical_inequality = 47,
    MaPLInstruction_int8_logical_less_than = 48,
    MaPLInstruction_int16_logical_less_than = 49,
    MaPLInstruction_int32_logical_less_than = 50,
    MaPLInstruction_int64_logical_less_than = 51,
    MaPLInstruction_uint8_logical_less_than = 52,
    MaPLInstruction_uint16_logical_less_than = 53,
    MaPLInstruction_uint32_logical_less_than = 54,
    MaPLInstruction_uint64_logical_less_than = 55,
    MaPLInstruction_float32_logical_less_than = 56,
    MaPLInstruction_float64_logical_less_than = 57,
    MaPLInstruction_int8_logical_less_than_equal = 58,
    MaPLInstruction_int16_logical_less_than_equal = 59,
    MaPLInstruction_int32_logical_less_than_equal = 60,
    MaPLInstruction_int64_logical_less_than_equal = 61,
    MaPLInstruction_uint8_logical_less_than_equal = 62,
    MaPLInstruction_uint16_logical_less_than_equal = 63,
    MaPLInstruction_uint32_logical_less_than_equal = 64,
    MaPLInstruction_uint64_logical_less_than_equal = 65,
    MaPLInstruction_float32_logical_less_than_equal = 66,
    MaPLInstruction_float64_logical_less_than_equal = 67,
    MaPLInstruction_int8_logical_greater_than = 68,
    MaPLInstruction_int16_logical_greater_than = 69,
    MaPLInstruction_int32_logical_greater_than = 70,
    MaPLInstruction_int64_logical_greater_than = 71,
    MaPLInstruction_uint8_logical_greater_than = 72,
    MaPLInstruction_uint16_logical_greater_than = 73,
    MaPLInstruction_uint32_logical_greater_than = 74,
    MaPLInstruction_uint64_logical_greater_than = 75,
    MaPLInstruction_float32_logical_greater_than = 76,
    MaPLInstruction_float64_logical_greater_than = 77,
    MaPLInstruction_int8_logical_greater_than_equal = 78,
    MaPLInstruction_int16_logical_greater_than_equal = 79,
    MaPLInstruction_int32_logical_greater_than_equal = 80,
    MaPLInstruction_int64_logical_greater_than_equal = 81,
    MaPLInstruction_uint8_logical_greater_than_equal = 82,
    MaPLInstruction_uint16_logical_greater_than_equal = 83,
    MaPLInstruction_uint32_logical_greater_than_equal = 84,
    MaPLInstruction_uint64_logical_greater_than_equal = 85,
    MaPLInstruction_float32_logical_greater_than_equal = 86,
    MaPLInstruction_float64_logical_greater_than_equal = 87,

    //NUMERIC OPERATORS
    MaPLInstruction_numeric_add = 88,
    MaPLInstruction_numeric_subtract = 89,
    MaPLInstruction_numeric_divide = 90,
    MaPLInstruction_numeric_multiply = 91,
    MaPLInstruction_numeric_modulo = 92,
    MaPLInstruction_numeric_negation = 93,

    //BITWISE OPERATORS
    MaPLInstruction_bitwise_and = 94,
    MaPLInstruction_bitwise_or = 95,
    MaPLInstruction_bitwise_xor = 96,
    MaPLInstruction_bitwise_negation = 97,
    MaPLInstruction_bitwise_shift_left = 98,
    MaPLInstruction_bitwise_shift_right = 99,

    //BOOLEAN OPERATORS
    MaPLInstruction_boolean_logical_equality = 100,
    MaPLInstruction_boolean_logical_inequality = 101,

    //LOGICAL OPERATORS
    MaPLInstruction_logical_and = 102,
    MaPLInstruction_logical_or = 103,
    MaPLInstruction_logical_negation = 104,

    //STRING OPERATORS
    MaPLInstruction_string_logical_equality = 105,
    MaPLInstruction_string_logical_inequality = 106,
    MaPLInstruction_string_concat = 107,

    //POINTER OPERATORS
    MaPLInstruction_pointer_logical_equality = 108,
    MaPLInstruction_pointer_logical_inequality = 109,

    //ASSIGN
    MaPLInstruction_int8_assign = 110,
    MaPLInstruction_int16_assign = 111,
    MaPLInstruction_int32_assign = 112,
    MaPLInstruction_int64_assign = 113,
    MaPLInstruction_uint8_assign = 114,
    MaPLInstruction_uint16_assign = 115,
    MaPLInstruction_uint32_assign = 116,
    MaPLInstruction_uint64_assign = 117,
    MaPLInstruction_float32_assign = 118,
    MaPLInstruction_float64_assign = 119,
    MaPLInstruction_boolean_assign = 120,
    MaPLInstruction_string_assign = 121,
    MaPLInstruction_pointer_assign = 122,
    MaPLInstruction_int8_assign_subscript = 123,
    MaPLInstruction_int16_assign_subscript = 124,
    MaPLInstruction_int32_assign_subscript = 125,
    MaPLInstruction_int64_assign_subscript = 126,
    MaPLInstruction_uint8_assign_subscript = 127,
    MaPLInstruction_uint16_assign_subscript = 128,
    MaPLInstruction_uint32_assign_subscript = 129,
    MaPLInstruction_uint64_assign_subscript = 130,
    MaPLInstruction_float32_assign_subscript = 131,
    MaPLInstruction_float64_assign_subscript = 132,
    MaPLInstruction_boolean_assign_subscript = 133,
    MaPLInstruction_string_assign_subscript = 134,
    MaPLInstruction_pointer_assign_subscript = 135,
    MaPLInstruction_int8_assign_property = 136,
    MaPLInstruction_int16_assign_property = 137,
    MaPLInstruction_int32_assign_property = 138,
    MaPLInstruction_int64_assign_property = 139,
    MaPLInstruction_uint8_assign_property = 140,
    MaPLInstruction_uint16_assign_property = 141,
    MaPLInstruction_uint32_assign_property = 142,
    MaPLInstruction_uint64_assign_property = 143,
    MaPLInstruction_float32_assign_property = 144,
    MaPLInstruction_float64_assign_property = 145,
    MaPLInstruction_boolean_assign_property = 146,
    MaPLInstruction_string_assign_property = 147,
    MaPLInstruction_pointer_assign_property = 148,

    //FUNCTION CALLS
    MaPLInstruction_function_invocation = 149,
    MaPLInstruction_subscript_invocation = 150,
    MaPLInstruction_int8_parameter = 151,
    MaPLInstruction_int16_parameter = 152,
    MaPLInstruction_int32_parameter = 153,
    MaPLInstruction_int64_parameter = 154,
    MaPLInstruction_uint8_parameter = 155,
    MaPLInstruction_uint16_parameter = 156,
    MaPLInstruction_uint32_parameter = 157,
    MaPLInstruction_uint64_parameter = 158,
    MaPLInstruction_float32_parameter = 159,
    MaPLInstruction_float64_parameter = 160,
    MaPLInstruction_string_parameter = 161,
    MaPLInstruction_boolean_parameter = 162,
    MaPLInstruction_pointer_parameter = 163,

    //CONTROL FLOW
    MaPLInstruction_conditional = 164,
    MaPLInstruction_ternary_conditional = 165,
    MaPLInstruction_null_coalescing = 166,
    MaPLInstruction_cursor_move_forward = 167,
    MaPLInstruction_cursor_move_back = 168,
    MaPLInstruction_no_op = 169,
    MaPLInstruction_program_exit = 170,

    //CAST (All casts are a combination of a 'to' and 'from' value)
    MaPLInstruction_typecast_to_int8 = 171,
    MaPLInstruction_typecast_to_int16 = 172,
    MaPLInstruction_typecast_to_int32 = 173,
    MaPLInstruction_typecast_to_int64 = 174,
    MaPLInstruction_typecast_to_uint8 = 175,
    MaPLInstruction_typecast_to_uint16 = 176,
    MaPLInstruction_typecast_to_uint32 = 177,
    MaPLInstruction_typecast_to_uint64 = 178,
    MaPLInstruction_typecast_to_float32 = 179,
    MaPLInstruction_typecast_to_float64 = 180,
    MaPLInstruction_typecast_to_boolean = 181,
    MaPLInstruction_typecast_to_string = 182,
    MaPLInstruction_typecast_from_int8 = 183,
    MaPLInstruction_typecast_from_int16 = 184,
    MaPLInstruction_typecast_from_int32 = 185,
    MaPLInstruction_typecast_from_int64 = 186,
    MaPLInstruction_typecast_from_uint8 = 187,
    MaPLInstruction_typecast_from_uint16 = 188,
    MaPLInstruction_typecast_from_uint32 = 189,
    MaPLInstruction_typecast_from_uint64 = 190,
    MaPLInstruction_typecast_from_float32 = 191,
    MaPLInstruction_typecast_from_float64 = 192,
    MaPLInstruction_typecast_from_boolean = 193,
    MaPLInstruction_typecast_from_string = 194,
    MaPLInstruction_typecast_from_pointer_to_string = 195,

    //METADATA
    MaPLInstruction_metadata = 196,

    //DEBUGGING
    // TODO: Add debug instructions based on command line flag.
    MaPLInstruction_debug_line = 197,
    MaPLInstruction_debug_update_variable = 198,
    MaPLInstruction_debug_delete_variable = 199,
};

#endif /* MaPLBytecodeConstants_h */

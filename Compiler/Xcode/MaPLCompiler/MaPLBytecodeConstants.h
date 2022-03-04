//
//  MaPLBytecodeConstants.h
//  MaPLCompiler
//
//  Created by Brad Bambara on 2/5/22.
//

#ifndef MaPLBytecodeConstants_h
#define MaPLBytecodeConstants_h

// Byte sizes for MaPL bytecode.
typedef u_int8_t MaPL_Instruction;
typedef u_int16_t MaPL_Index;

//PRIMITIVE LITERALS
#define MAPL_BYTE_LITERAL_INT8 1
#define MAPL_BYTE_LITERAL_INT16 2
#define MAPL_BYTE_LITERAL_INT32 3
#define MAPL_BYTE_LITERAL_INT64 4
#define MAPL_BYTE_LITERAL_UINT8 5
#define MAPL_BYTE_LITERAL_UINT16 6
#define MAPL_BYTE_LITERAL_UINT32 7
#define MAPL_BYTE_LITERAL_UINT64 8
#define MAPL_BYTE_LITERAL_FLOAT32 9
#define MAPL_BYTE_LITERAL_FLOAT64 10
#define MAPL_BYTE_LITERAL_BOOLEAN_TRUE 11
#define MAPL_BYTE_LITERAL_BOOLEAN_FALSE 12
#define MAPL_BYTE_LITERAL_STRING 13
#define MAPL_BYTE_LITERAL_NULL 14

//VARIABLE RETRIEVAL
#define MAPL_BYTE_VARIABLE_INT8 15
#define MAPL_BYTE_VARIABLE_INT16 16
#define MAPL_BYTE_VARIABLE_INT32 17
#define MAPL_BYTE_VARIABLE_INT64 18
#define MAPL_BYTE_VARIABLE_UINT8 19
#define MAPL_BYTE_VARIABLE_UINT16 20
#define MAPL_BYTE_VARIABLE_UINT32 21
#define MAPL_BYTE_VARIABLE_UINT64 22
#define MAPL_BYTE_VARIABLE_FLOAT32 23
#define MAPL_BYTE_VARIABLE_FLOAT64 24
#define MAPL_BYTE_VARIABLE_BOOLEAN 25
#define MAPL_BYTE_VARIABLE_STRING 26
#define MAPL_BYTE_VARIABLE_OBJECT 27

//NUMERIC LOGIC
#define MAPL_BYTE_INT8_LOGICAL_EQUALITY 28
#define MAPL_BYTE_INT16_LOGICAL_EQUALITY 29
#define MAPL_BYTE_INT32_LOGICAL_EQUALITY 30
#define MAPL_BYTE_INT64_LOGICAL_EQUALITY 31
#define MAPL_BYTE_UINT8_LOGICAL_EQUALITY 32
#define MAPL_BYTE_UINT16_LOGICAL_EQUALITY 33
#define MAPL_BYTE_UINT32_LOGICAL_EQUALITY 34
#define MAPL_BYTE_UINT64_LOGICAL_EQUALITY 35
#define MAPL_BYTE_FLOAT32_LOGICAL_EQUALITY 36
#define MAPL_BYTE_FLOAT64_LOGICAL_EQUALITY 37
#define MAPL_BYTE_INT8_LOGICAL_INEQUALITY 38
#define MAPL_BYTE_INT16_LOGICAL_INEQUALITY 39
#define MAPL_BYTE_INT32_LOGICAL_INEQUALITY 40
#define MAPL_BYTE_INT64_LOGICAL_INEQUALITY 41
#define MAPL_BYTE_UINT8_LOGICAL_INEQUALITY 42
#define MAPL_BYTE_UINT16_LOGICAL_INEQUALITY 43
#define MAPL_BYTE_UINT32_LOGICAL_INEQUALITY 44
#define MAPL_BYTE_UINT64_LOGICAL_INEQUALITY 45
#define MAPL_BYTE_FLOAT32_LOGICAL_INEQUALITY 46
#define MAPL_BYTE_FLOAT64_LOGICAL_INEQUALITY 47
#define MAPL_BYTE_INT8_LOGICAL_LESS_THAN 48
#define MAPL_BYTE_INT16_LOGICAL_LESS_THAN 49
#define MAPL_BYTE_INT32_LOGICAL_LESS_THAN 50
#define MAPL_BYTE_INT64_LOGICAL_LESS_THAN 51
#define MAPL_BYTE_UINT8_LOGICAL_LESS_THAN 52
#define MAPL_BYTE_UINT16_LOGICAL_LESS_THAN 53
#define MAPL_BYTE_UINT32_LOGICAL_LESS_THAN 54
#define MAPL_BYTE_UINT64_LOGICAL_LESS_THAN 55
#define MAPL_BYTE_FLOAT32_LOGICAL_LESS_THAN 56
#define MAPL_BYTE_FLOAT64_LOGICAL_LESS_THAN 57
#define MAPL_BYTE_INT8_LOGICAL_LESS_THAN_EQUAL 58
#define MAPL_BYTE_INT16_LOGICAL_LESS_THAN_EQUAL 59
#define MAPL_BYTE_INT32_LOGICAL_LESS_THAN_EQUAL 60
#define MAPL_BYTE_INT64_LOGICAL_LESS_THAN_EQUAL 61
#define MAPL_BYTE_UINT8_LOGICAL_LESS_THAN_EQUAL 62
#define MAPL_BYTE_UINT16_LOGICAL_LESS_THAN_EQUAL 63
#define MAPL_BYTE_UINT32_LOGICAL_LESS_THAN_EQUAL 64
#define MAPL_BYTE_UINT64_LOGICAL_LESS_THAN_EQUAL 65
#define MAPL_BYTE_FLOAT32_LOGICAL_LESS_THAN_EQUAL 66
#define MAPL_BYTE_FLOAT64_LOGICAL_LESS_THAN_EQUAL 67
#define MAPL_BYTE_INT8_LOGICAL_GREATER_THAN 68
#define MAPL_BYTE_INT16_LOGICAL_GREATER_THAN 69
#define MAPL_BYTE_INT32_LOGICAL_GREATER_THAN 70
#define MAPL_BYTE_INT64_LOGICAL_GREATER_THAN 71
#define MAPL_BYTE_UINT8_LOGICAL_GREATER_THAN 72
#define MAPL_BYTE_UINT16_LOGICAL_GREATER_THAN 73
#define MAPL_BYTE_UINT32_LOGICAL_GREATER_THAN 74
#define MAPL_BYTE_UINT64_LOGICAL_GREATER_THAN 75
#define MAPL_BYTE_FLOAT32_LOGICAL_GREATER_THAN 76
#define MAPL_BYTE_FLOAT64_LOGICAL_GREATER_THAN 77
#define MAPL_BYTE_INT8_LOGICAL_GREATER_THAN_EQUAL 78
#define MAPL_BYTE_INT16_LOGICAL_GREATER_THAN_EQUAL 79
#define MAPL_BYTE_INT32_LOGICAL_GREATER_THAN_EQUAL 80
#define MAPL_BYTE_INT64_LOGICAL_GREATER_THAN_EQUAL 81
#define MAPL_BYTE_UINT8_LOGICAL_GREATER_THAN_EQUAL 82
#define MAPL_BYTE_UINT16_LOGICAL_GREATER_THAN_EQUAL 83
#define MAPL_BYTE_UINT32_LOGICAL_GREATER_THAN_EQUAL 84
#define MAPL_BYTE_UINT64_LOGICAL_GREATER_THAN_EQUAL 85
#define MAPL_BYTE_FLOAT32_LOGICAL_GREATER_THAN_EQUAL 86
#define MAPL_BYTE_FLOAT64_LOGICAL_GREATER_THAN_EQUAL 87

//NUMERIC OPERATORS
#define MAPL_BYTE_NUMERIC_ADD 88
#define MAPL_BYTE_NUMERIC_SUBTRACT 89
#define MAPL_BYTE_NUMERIC_DIVIDE 90
#define MAPL_BYTE_NUMERIC_MULTIPLY 91
#define MAPL_BYTE_NUMERIC_MODULO 92
#define MAPL_BYTE_NUMERIC_INCREMENT 93
#define MAPL_BYTE_NUMERIC_DECREMENT 94
#define MAPL_BYTE_NUMERIC_ABS_VALUE 95
#define MAPL_BYTE_NUMERIC_NEGATION 96

//BITWISE OPERATORS
#define MAPL_BYTE_BITWISE_AND 97
#define MAPL_BYTE_BITWISE_OR 98
#define MAPL_BYTE_BITWISE_XOR 99
#define MAPL_BYTE_BITWISE_NEGATION 100
#define MAPL_BYTE_BITWISE_SHIFT_LEFT 101
#define MAPL_BYTE_BITWISE_SHIFT_RIGHT 102

//BOOLEAN OPERATORS
#define MAPL_BYTE_BOOLEAN_LOGICAL_EQUALITY 103
#define MAPL_BYTE_BOOLEAN_LOGICAL_INEQUALITY 104

//LOGICAL OPERATORS
#define MAPL_BYTE_LOGICAL_AND 105
#define MAPL_BYTE_LOGICAL_OR 106
#define MAPL_BYTE_LOGICAL_NEGATION 107

//STRING OPERATORS
#define MAPL_BYTE_STRING_LOGICAL_EQUALITY 108
#define MAPL_BYTE_STRING_LOGICAL_INEQUALITY 109
#define MAPL_BYTE_STRING_CONCAT 110

//OBJECT OPERATORS
#define MAPL_BYTE_OBJECT_LOGICAL_EQUALITY 111
#define MAPL_BYTE_OBJECT_LOGICAL_INEQUALITY 112

//ASSIGN
#define MAPL_BYTE_INT8_ASSIGN 113
#define MAPL_BYTE_INT16_ASSIGN 114
#define MAPL_BYTE_INT32_ASSIGN 115
#define MAPL_BYTE_INT64_ASSIGN 116
#define MAPL_BYTE_UINT8_ASSIGN 117
#define MAPL_BYTE_UINT16_ASSIGN 118
#define MAPL_BYTE_UINT32_ASSIGN 119
#define MAPL_BYTE_UINT64_ASSIGN 120
#define MAPL_BYTE_FLOAT32_ASSIGN 121
#define MAPL_BYTE_FLOAT64_ASSIGN 122
#define MAPL_BYTE_INT8_ASSIGN_WITH_OPERATOR 123
#define MAPL_BYTE_INT16_ASSIGN_WITH_OPERATOR 124
#define MAPL_BYTE_INT32_ASSIGN_WITH_OPERATOR 125
#define MAPL_BYTE_INT64_ASSIGN_WITH_OPERATOR 126
#define MAPL_BYTE_UINT8_ASSIGN_WITH_OPERATOR 127
#define MAPL_BYTE_UINT16_ASSIGN_WITH_OPERATOR 128
#define MAPL_BYTE_UINT32_ASSIGN_WITH_OPERATOR 129
#define MAPL_BYTE_UINT64_ASSIGN_WITH_OPERATOR 130
#define MAPL_BYTE_FLOAT32_ASSIGN_WITH_OPERATOR 131
#define MAPL_BYTE_FLOAT64_ASSIGN_WITH_OPERATOR 132
#define MAPL_BYTE_BOOLEAN_ASSIGN 133
#define MAPL_BYTE_STRING_ASSIGN 134
#define MAPL_BYTE_STRING_CONCAT_ASSIGN 135
#define MAPL_BYTE_OBJECT_ASSIGN 136

//FUNCTION CALLS
#define MAPL_BYTE_FUNCTION_CALL 137
#define MAPL_BYTE_SUBSCRIPT_CALL 138
#define MAPL_BYTE_INT8_PARAMETER 139
#define MAPL_BYTE_INT16_PARAMETER 140
#define MAPL_BYTE_INT32_PARAMETER 141
#define MAPL_BYTE_INT64_PARAMETER 142
#define MAPL_BYTE_UINT8_PARAMETER 143
#define MAPL_BYTE_UINT16_PARAMETER 144
#define MAPL_BYTE_UINT32_PARAMETER 145
#define MAPL_BYTE_UINT64_PARAMETER 146
#define MAPL_BYTE_FLOAT32_PARAMETER 147
#define MAPL_BYTE_FLOAT64_PARAMETER 148
#define MAPL_BYTE_STRING_PARAMETER 149
#define MAPL_BYTE_BOOLEAN_PARAMETER 150
#define MAPL_BYTE_OBJECT_PARAMETER 151

//CONTROL FLOW
#define MAPL_BYTE_CONDITIONAL 152
#define MAPL_BYTE_SWITCH_CASE_INT16 153
#define MAPL_BYTE_SWITCH_CASE_INT32 154
#define MAPL_BYTE_SWITCH_CASE_INT64 155
#define MAPL_BYTE_SWITCH_CASE_UINT8 156
#define MAPL_BYTE_SWITCH_CASE_UINT16 157
#define MAPL_BYTE_SWITCH_CASE_UINT32 158
#define MAPL_BYTE_SWITCH_CASE_UINT64 159
#define MAPL_BYTE_SWITCH_CASE_FLOAT32 160
#define MAPL_BYTE_SWITCH_CASE_FLOAT64 161
#define MAPL_BYTE_SWITCH_CASE_STRING 162
#define MAPL_BYTE_SWITCH_CASE_BOOLEAN 163
#define MAPL_BYTE_CURSOR_MOVE_FORWARD 164
#define MAPL_BYTE_CURSOR_MOVE_BACK 165
#define MAPL_BYTE_PROGRAM_EXIT 166

//CAST (All casts are a combination of a 'to' and 'from' value)
#define MAPL_BYTE_TYPECAST_TO_INT8 167
#define MAPL_BYTE_TYPECAST_TO_INT16 168
#define MAPL_BYTE_TYPECAST_TO_INT32 169
#define MAPL_BYTE_TYPECAST_TO_INT64 170
#define MAPL_BYTE_TYPECAST_TO_UINT8 171
#define MAPL_BYTE_TYPECAST_TO_UINT16 172
#define MAPL_BYTE_TYPECAST_TO_UINT32 173
#define MAPL_BYTE_TYPECAST_TO_UINT64 174
#define MAPL_BYTE_TYPECAST_TO_FLOAT32 175
#define MAPL_BYTE_TYPECAST_TO_FLOAT64 176
#define MAPL_BYTE_TYPECAST_TO_BOOLEAN 177
#define MAPL_BYTE_TYPECAST_TO_STRING 178
#define MAPL_BYTE_TYPECAST_TO_OBJECT 179
#define MAPL_BYTE_TYPECAST_FROM_INT8 180
#define MAPL_BYTE_TYPECAST_FROM_INT16 181
#define MAPL_BYTE_TYPECAST_FROM_INT32 182
#define MAPL_BYTE_TYPECAST_FROM_INT64 183
#define MAPL_BYTE_TYPECAST_FROM_UINT8 184
#define MAPL_BYTE_TYPECAST_FROM_UINT16 185
#define MAPL_BYTE_TYPECAST_FROM_UINT32 186
#define MAPL_BYTE_TYPECAST_FROM_UINT64 187
#define MAPL_BYTE_TYPECAST_FROM_FLOAT32 188
#define MAPL_BYTE_TYPECAST_FROM_FLOAT64 189
#define MAPL_BYTE_TYPECAST_FROM_BOOLEAN 190
#define MAPL_BYTE_TYPECAST_FROM_STRING 191
#define MAPL_BYTE_TYPECAST_FROM_OBJECT 192

//METADATA
#define MAPL_BYTE_METADATA 193

//DEBUGGING
#define MAPL_BYTE_DEBUG_LINE 194
#define MAPL_BYTE_DEBUG_UPDATE_VARIABLE 195
#define MAPL_BYTE_DEBUG_DELETE_VARIABLE 196

#endif /* MaPLBytecodeConstants_h */

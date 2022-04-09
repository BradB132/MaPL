//
//  MaPLRuntime.c
//  MaPLRuntime
//
//  Created by Brad Bambara on 4/5/22.
//

#include "MaPLRuntime.h"
#include "MaPLBytecodeConstants.h"
#include <math.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

typedef enum {
    MaPLExecutionState_continue,
    MaPLExecutionState_exit,
    MaPLExecutionState_error,
} MaPLExecutionState;

typedef struct {
    const u_int8_t* scriptBuffer;
    MaPLCursorMove bufferLength;
    MaPLCursorMove cursorPosition;
    u_int8_t *primitiveTable;
    char **stringTable;
    const MaPLCallbacks *callbacks;
    bool isUnusedCodepath;
    MaPLExecutionState executionState;
} MaPLExecutionContext;

u_int8_t evaluateChar(MaPLExecutionContext *context);
int32_t evaluateInt32(MaPLExecutionContext *context);
int64_t evaluateInt64(MaPLExecutionContext *context);
u_int32_t evaluateUint32(MaPLExecutionContext *context);
u_int64_t evaluateUint64(MaPLExecutionContext *context);
float evaluateFloat32(MaPLExecutionContext *context);
double evaluateFloat64(MaPLExecutionContext *context);
bool evaluateBool(MaPLExecutionContext *context);
void *evaluatePointer(MaPLExecutionContext *context);
char *evaluateString(MaPLExecutionContext *context);

// Memory management scheme for MaPL strings:
// The higher-order bits in 64-bit pointers are unused, so the MaPL runtime
// "tags" these pointers to store metadata about the usage of the string.
// Two flags are stored:
// 1- Allocated - Indicates when the string has been malloc'd and needs to be freed when no longer used.
// 2- Stored - Indicates if the string is stored in the string table.
//
// Example #1:
// string s = ""; -> Both the Lvalue "s" and the literal Rvalue are neither allocated nor stored.
//   If there was an allocated string previously stored in "s", it will be freed.
//
// Example #2:
// string s1 = mallocString(); -> Rvalue is allocated but not stored. Lvalue is the same memory address tagged as allocated and stored.
// string s2 = s1; -> Rvalue is stored but not allocated. Lvalue is allocated and stored.
//   Tagged pointers are not capable of a more complex memory management scheme like reference counting.
//   In the edge case of assigning the same malloc'd string to multiple variables, the string must be copied.
//
// Example #3:
// string s = mallocString(); -> Same as example #2.
// s = s;
//   In the edge case of assigning a stored string to itself, this should be a no-op.
char *tagStringAsAllocated(char *string) {
    return (char *)((uintptr_t)string | 0x8000000000000000);
}
char *tagStringAsNotAllocated(char *string) {
    return (char *)((uintptr_t)string & 0x7FFFFFFFFFFFFFFF);
}
bool isStringAllocated(const char *taggedString) {
    return ((uintptr_t)taggedString & 0x8000000000000000) != 0;
}
char *tagStringAsStored(char *string) {
    return (char *)((uintptr_t)string | 0x4000000000000000);
}
bool isStringStored(const char *taggedString) {
    return ((uintptr_t)taggedString & 0x4000000000000000) != 0;
}
char *untagString(char *string) {
    return (char *)((uintptr_t)string & 0x3FFFFFFFFFFFFFFF);
}
void freeStringIfNeeded(char *string) {
    if (isStringAllocated(string)) {
        free(untagString(string));
    }
}

MaPLParameter MaPLUninitialized(void) {
    return (MaPLParameter){ MaPLDataType_uninitialized };
}
MaPLParameter MaPLVoid(void) {
    return (MaPLParameter){ MaPLDataType_void };
}
MaPLParameter MaPLChar(u_int8_t charValue) {
    MaPLParameter parameter = { MaPLDataType_char };
    parameter.charValue = charValue;
    return parameter;
}
MaPLParameter MaPLInt32(int32_t int32Value) {
    MaPLParameter parameter = { MaPLDataType_int32 };
    parameter.int32Value = int32Value;
    return parameter;
}
MaPLParameter MaPLInt64(int64_t int64Value) {
    MaPLParameter parameter = { MaPLDataType_int64 };
    parameter.int64Value = int64Value;
    return parameter;
}
MaPLParameter MaPLUint32(u_int32_t uint32Value) {
    MaPLParameter parameter = { MaPLDataType_uint32 };
    parameter.uint32Value = uint32Value;
    return parameter;
}
MaPLParameter MaPLUint64(u_int64_t uint64Value) {
    MaPLParameter parameter = { MaPLDataType_uint64 };
    parameter.uint64Value = uint64Value;
    return parameter;
}
MaPLParameter MaPLFloat32(float float32Value) {
    MaPLParameter parameter = { MaPLDataType_float32 };
    parameter.float32Value = float32Value;
    return parameter;
}
MaPLParameter MaPLFloat64(double float64Value) {
    MaPLParameter parameter = { MaPLDataType_float64 };
    parameter.float64Value = float64Value;
    return parameter;
}
MaPLParameter MaPLBool(bool booleanValue) {
    MaPLParameter parameter = { MaPLDataType_boolean };
    parameter.booleanValue = booleanValue;
    return parameter;
}
MaPLParameter MaPLPointer(void *pointerValue) {
    MaPLParameter parameter = { MaPLDataType_pointer };
    parameter.pointerValue = pointerValue;
    return parameter;
}
MaPLParameter MaPLStringByReference(char *stringValue) {
    MaPLParameter parameter = { MaPLDataType_string };
    parameter.stringValue = stringValue;
    return parameter;
}
MaPLParameter MaPLStringByValue(char *stringValue) {
    MaPLParameter parameter = { MaPLDataType_string };
    parameter.stringValue = malloc(strlen(stringValue)+1);
    strcpy((char *)parameter.stringValue, stringValue);
    parameter.stringValue = tagStringAsAllocated((char *)parameter.stringValue);
    return parameter;
}

MaPLDataType typeForInstruction(enum MaPLInstruction instruction) {
    if (instruction <= MaPLInstruction_int32_typecast) { return MaPLDataType_int32; }
    if (instruction <= MaPLInstruction_float32_typecast) { return MaPLDataType_float32; }
    if (instruction <= MaPLInstruction_string_typecast) { return MaPLDataType_string; }
    if (instruction <= MaPLInstruction_pointer_ternary_conditional) { return MaPLDataType_pointer; }
    if (instruction <= MaPLInstruction_logical_negation) { return MaPLDataType_boolean; }
    if (instruction <= MaPLInstruction_int64_typecast) { return MaPLDataType_int64; }
    if (instruction <= MaPLInstruction_float64_typecast) { return MaPLDataType_float64; }
    if (instruction <= MaPLInstruction_uint32_typecast) { return MaPLDataType_uint32; }
    if (instruction <= MaPLInstruction_uint64_typecast) { return MaPLDataType_uint64; }
    if (instruction <= MaPLInstruction_char_typecast) { return MaPLDataType_char; }
    return MaPLDataType_void;
}

enum MaPLInstruction readInstruction(MaPLExecutionContext *context) {
    enum MaPLInstruction instruction = context->scriptBuffer[context->cursorPosition];
    context->cursorPosition++;
    return instruction;
}

MaPLMemoryAddress readMemoryAddress(MaPLExecutionContext *context) {
    MaPLMemoryAddress address = *((MaPLMemoryAddress *)(context->scriptBuffer+context->cursorPosition));
    context->cursorPosition += sizeof(MaPLMemoryAddress);
    return address;
}

MaPLSymbol readSymbol(MaPLExecutionContext *context) {
    MaPLSymbol symbol = *((MaPLSymbol *)(context->scriptBuffer+context->cursorPosition));
    context->cursorPosition += sizeof(MaPLSymbol);
    return symbol;
}

const char *readString(MaPLExecutionContext *context) {
    const char *string = (const char *)(context->scriptBuffer+context->cursorPosition);
    context->cursorPosition += strlen(string)+1;
    return string;
}

MaPLParameter evaluateParameter(MaPLExecutionContext *context) {
    switch (typeForInstruction(context->scriptBuffer[context->cursorPosition])) {
        case MaPLDataType_char:
            return MaPLChar(evaluateChar(context));
        case MaPLDataType_int32:
            return MaPLInt32(evaluateInt32(context));
        case MaPLDataType_int64:
            return MaPLInt64(evaluateInt64(context));
        case MaPLDataType_uint32:
            return MaPLUint32(evaluateUint32(context));
        case MaPLDataType_uint64:
            return MaPLUint64(evaluateUint64(context));
        case MaPLDataType_float32:
            return MaPLFloat32(evaluateFloat32(context));
        case MaPLDataType_float64:
            return MaPLFloat64(evaluateFloat64(context));
        case MaPLDataType_string:
            return MaPLStringByReference(evaluateString(context));
        case MaPLDataType_boolean:
            return MaPLBool(evaluateBool(context));
        case MaPLDataType_pointer:
            return MaPLPointer(evaluatePointer(context));
        default:
            context->executionState = MaPLExecutionState_error;
            return MaPLUninitialized();
    }
}

MaPLParameter evaluateFunctionInvocation(MaPLExecutionContext *context) {
    // This function assumes that we've already advanced past the initial "function_invocation" byte.
    const void *invokedOnPointer = NULL;
    if (context->scriptBuffer[context->cursorPosition] == MaPLInstruction_no_op) {
        // This function is not invoked on another pointer, it's a global call.
        context->cursorPosition++;
    } else {
        invokedOnPointer = evaluatePointer(context);
    }
    
    MaPLSymbol symbol = readSymbol(context);
 
    // Create the list of parameters.
    MaPLParameterCount paramCount = *((MaPLParameterCount *)(context->scriptBuffer+context->cursorPosition));
    context->cursorPosition += sizeof(MaPLParameterCount);
    MaPLParameter functionParams[paramCount];
    char *taggedStringParams[paramCount];
    memset(taggedStringParams, 0, sizeof(taggedStringParams));
    for (MaPLParameterCount i = 0; i < paramCount; i++) {
        functionParams[i] = evaluateParameter(context);
        if (functionParams[i].dataType == MaPLDataType_string) {
            // Untag the string and store the tagged pointer for later release.
            taggedStringParams[i] = (char *)functionParams[i].stringValue;
            functionParams[i].stringValue = untagString((char *)functionParams[i].stringValue);
        }
    }
    
    // Invoke the function.
    MaPLParameter returnValue = MaPLUninitialized();
    if (context->executionState == MaPLExecutionState_continue &&
        !context->isUnusedCodepath &&
        context->callbacks->invokeFunction) {
        returnValue = context->callbacks->invokeFunction(invokedOnPointer,
                                                         symbol,
                                                         functionParams,
                                                         paramCount);
    }
    
    // Clean up string params.
    for (MaPLParameterCount i = 0; i < paramCount; i++) {
        freeStringIfNeeded(taggedStringParams[i]);
    }
    
    return returnValue;
}

MaPLParameter evaluateSubscriptInvocation(MaPLExecutionContext *context) {
    // This function assumes that we've already advanced past the initial "subscript_invocation" byte.
    const void *invokedOnPointer = evaluatePointer(context);
    if (!invokedOnPointer) {
        context->executionState = MaPLExecutionState_error;
    }
    
    MaPLParameter subscriptIndex = evaluateParameter(context);
    char *taggedIndex = NULL;
    if (subscriptIndex.dataType == MaPLDataType_string) {
        // Untag the string and store the tagged pointer for later release.
        taggedIndex = (char *)subscriptIndex.stringValue;
        subscriptIndex.stringValue = untagString((char *)subscriptIndex.stringValue);
    }
    
    // Invoke the subscript.
    MaPLParameter returnValue = MaPLUninitialized();
    if (context->executionState == MaPLExecutionState_continue &&
        !context->isUnusedCodepath &&
        context->callbacks->invokeSubscript) {
        returnValue = context->callbacks->invokeSubscript(invokedOnPointer, subscriptIndex);
    }
    
    // Clean up string index.
    freeStringIfNeeded(taggedIndex);
    
    return returnValue;
}

u_int8_t evaluateChar(MaPLExecutionContext *context) {
    switch(readInstruction(context)) {
        case MaPLInstruction_char_literal: {
            u_int8_t literal = *((u_int8_t *)(context->scriptBuffer+context->cursorPosition));
            context->cursorPosition += sizeof(u_int8_t);
            return literal;
        }
        case MaPLInstruction_char_variable:
            return *((u_int8_t *)(context->primitiveTable+readMemoryAddress(context)));
        case MaPLInstruction_char_add:
            return evaluateChar(context) + evaluateChar(context);
        case MaPLInstruction_char_subtract:
            return evaluateChar(context) - evaluateChar(context);
        case MaPLInstruction_char_divide:
            return evaluateChar(context) / evaluateChar(context);
        case MaPLInstruction_char_multiply:
            return evaluateChar(context) * evaluateChar(context);
        case MaPLInstruction_char_modulo:
            return evaluateChar(context) % evaluateChar(context);
        case MaPLInstruction_char_bitwise_and:
            return evaluateChar(context) & evaluateChar(context);
        case MaPLInstruction_char_bitwise_or:
            return evaluateChar(context) | evaluateChar(context);
        case MaPLInstruction_char_bitwise_xor:
            return evaluateChar(context) ^ evaluateChar(context);
        case MaPLInstruction_char_bitwise_negation:
            return ~evaluateChar(context);
        case MaPLInstruction_char_bitwise_shift_left:
            return evaluateChar(context) << evaluateChar(context);
        case MaPLInstruction_char_bitwise_shift_right:
            return evaluateChar(context) >> evaluateChar(context);
        case MaPLInstruction_char_function_invocation: {
            MaPLParameter returnedValue = evaluateFunctionInvocation(context);
            if (returnedValue.dataType != MaPLDataType_char) {
                if (!context->isUnusedCodepath) {
                    context->executionState = MaPLExecutionState_error;
                    if (returnedValue.dataType == MaPLDataType_string) {
                        freeStringIfNeeded((char *)returnedValue.stringValue);
                    }
                }
                return 0;
            }
            return returnedValue.charValue;
        }
        case MaPLInstruction_char_subscript_invocation: {
            MaPLParameter returnedValue = evaluateSubscriptInvocation(context);
            if (returnedValue.dataType != MaPLDataType_char) {
                if (!context->isUnusedCodepath) {
                    context->executionState = MaPLExecutionState_error;
                    if (returnedValue.dataType == MaPLDataType_string) {
                        freeStringIfNeeded((char *)returnedValue.stringValue);
                    }
                }
                return 0;
            }
            return returnedValue.charValue;
        }
        case MaPLInstruction_char_ternary_conditional: {
            bool previousUnusedCodepath = context->isUnusedCodepath;
            u_int8_t result;
            if (evaluateBool(context)) {
                result = evaluateChar(context);
                context->isUnusedCodepath = true;
                evaluateChar(context);
                context->isUnusedCodepath = previousUnusedCodepath;
            } else {
                context->isUnusedCodepath = true;
                evaluateChar(context);
                context->isUnusedCodepath = previousUnusedCodepath;
                result = evaluateChar(context);
            }
            return result;
        }
        case MaPLInstruction_char_typecast:
            switch (typeForInstruction(context->scriptBuffer[context->cursorPosition])) {
                case MaPLDataType_int32:
                    return (u_int8_t)evaluateInt32(context);
                case MaPLDataType_int64:
                    return (u_int8_t)evaluateInt64(context);
                case MaPLDataType_uint32:
                    return (u_int8_t)evaluateUint32(context);
                case MaPLDataType_uint64:
                    return (u_int8_t)evaluateUint64(context);
                case MaPLDataType_float32:
                    return (u_int8_t)evaluateFloat32(context);
                case MaPLDataType_float64:
                    return (u_int8_t)evaluateFloat64(context);
                case MaPLDataType_string: {
                    char *taggedString = evaluateString(context);
                    u_int8_t returnChar = (u_int8_t)atoi(untagString(taggedString));
                    freeStringIfNeeded(taggedString);
                    return returnChar;
                }
                case MaPLDataType_boolean:
                    return evaluateBool(context) ? 1 : 0;
                default:
                    context->executionState = MaPLExecutionState_error;
                    break;
            }
            break;
        default:
            context->executionState = MaPLExecutionState_error;
            break;
    }
    return 0;
}

int32_t evaluateInt32(MaPLExecutionContext *context) {
    switch(readInstruction(context)) {
        case MaPLInstruction_int32_literal: {
            int32_t literal = *((int32_t *)(context->scriptBuffer+context->cursorPosition));
            context->cursorPosition += sizeof(int32_t);
            return literal;
        }
        case MaPLInstruction_int32_variable:
            return *((int32_t *)(context->primitiveTable+readMemoryAddress(context)));
        case MaPLInstruction_int32_add:
            return evaluateInt32(context) + evaluateInt32(context);
        case MaPLInstruction_int32_subtract:
            return evaluateInt32(context) - evaluateInt32(context);
        case MaPLInstruction_int32_divide:
            return evaluateInt32(context) / evaluateInt32(context);
        case MaPLInstruction_int32_multiply:
            return evaluateInt32(context) * evaluateInt32(context);
        case MaPLInstruction_int32_modulo:
            return evaluateInt32(context) % evaluateInt32(context);
        case MaPLInstruction_int32_numeric_negation:
            return -evaluateInt32(context);
        case MaPLInstruction_int32_bitwise_and:
            return evaluateInt32(context) & evaluateInt32(context);
        case MaPLInstruction_int32_bitwise_or:
            return evaluateInt32(context) | evaluateInt32(context);
        case MaPLInstruction_int32_bitwise_xor:
            return evaluateInt32(context) ^ evaluateInt32(context);
        case MaPLInstruction_int32_bitwise_negation:
            return ~evaluateInt32(context);
        case MaPLInstruction_int32_bitwise_shift_left:
            return evaluateInt32(context) << evaluateInt32(context);
        case MaPLInstruction_int32_bitwise_shift_right:
            return evaluateInt32(context) >> evaluateInt32(context);
        case MaPLInstruction_int32_function_invocation: {
            MaPLParameter returnedValue = evaluateFunctionInvocation(context);
            if (returnedValue.dataType != MaPLDataType_int32) {
                if (!context->isUnusedCodepath) {
                    context->executionState = MaPLExecutionState_error;
                    if (returnedValue.dataType == MaPLDataType_string) {
                        freeStringIfNeeded((char *)returnedValue.stringValue);
                    }
                }
                return 0;
            }
            return returnedValue.int32Value;
        }
        case MaPLInstruction_int32_subscript_invocation: {
            MaPLParameter returnedValue = evaluateSubscriptInvocation(context);
            if (returnedValue.dataType != MaPLDataType_int32) {
                if (!context->isUnusedCodepath) {
                    context->executionState = MaPLExecutionState_error;
                    if (returnedValue.dataType == MaPLDataType_string) {
                        freeStringIfNeeded((char *)returnedValue.stringValue);
                    }
                }
                return 0;
            }
            return returnedValue.int32Value;
        }
        case MaPLInstruction_int32_ternary_conditional: {
            bool previousUnusedCodepath = context->isUnusedCodepath;
            int32_t result;
            if (evaluateBool(context)) {
                result = evaluateInt32(context);
                context->isUnusedCodepath = true;
                evaluateInt32(context);
                context->isUnusedCodepath = previousUnusedCodepath;
            } else {
                context->isUnusedCodepath = true;
                evaluateInt32(context);
                context->isUnusedCodepath = previousUnusedCodepath;
                result = evaluateInt32(context);
            }
            return result;
        }
        case MaPLInstruction_int32_typecast:
            switch (typeForInstruction(context->scriptBuffer[context->cursorPosition])) {
                case MaPLDataType_char:
                    return (int32_t)evaluateChar(context);
                case MaPLDataType_int64:
                    return (int32_t)evaluateInt64(context);
                case MaPLDataType_uint32:
                    return (int32_t)evaluateUint32(context);
                case MaPLDataType_uint64:
                    return (int32_t)evaluateUint64(context);
                case MaPLDataType_float32:
                    return (int32_t)evaluateFloat32(context);
                case MaPLDataType_float64:
                    return (int32_t)evaluateFloat64(context);
                case MaPLDataType_string: {
                    char *taggedString = evaluateString(context);
                    int32_t returnInt = (int32_t)atoi(untagString(taggedString));
                    freeStringIfNeeded(taggedString);
                    return returnInt;
                }
                case MaPLDataType_boolean:
                    return evaluateBool(context) ? 1 : 0;
                default:
                    context->executionState = MaPLExecutionState_error;
                    break;
            }
            break;
        default:
            context->executionState = MaPLExecutionState_error;
            break;
    }
    return 0;
}

int64_t evaluateInt64(MaPLExecutionContext *context) {
    switch(readInstruction(context)) {
        case MaPLInstruction_int64_literal: {
            int64_t literal = *((int64_t *)(context->scriptBuffer+context->cursorPosition));
            context->cursorPosition += sizeof(int64_t);
            return literal;
        }
        case MaPLInstruction_int64_variable:
            return *((int64_t *)(context->primitiveTable+readMemoryAddress(context)));
        case MaPLInstruction_int64_add:
            return evaluateInt64(context) + evaluateInt64(context);
        case MaPLInstruction_int64_subtract:
            return evaluateInt64(context) - evaluateInt64(context);
        case MaPLInstruction_int64_divide:
            return evaluateInt64(context) / evaluateInt64(context);
        case MaPLInstruction_int64_multiply:
            return evaluateInt64(context) * evaluateInt64(context);
        case MaPLInstruction_int64_modulo:
            return evaluateInt64(context) % evaluateInt64(context);
        case MaPLInstruction_int64_numeric_negation:
            return -evaluateInt64(context);
        case MaPLInstruction_int64_bitwise_and:
            return evaluateInt64(context) & evaluateInt64(context);
        case MaPLInstruction_int64_bitwise_or:
            return evaluateInt64(context) | evaluateInt64(context);
        case MaPLInstruction_int64_bitwise_xor:
            return evaluateInt64(context) ^ evaluateInt64(context);
        case MaPLInstruction_int64_bitwise_negation:
            return ~evaluateInt64(context);
        case MaPLInstruction_int64_bitwise_shift_left:
            return evaluateInt64(context) << evaluateInt64(context);
        case MaPLInstruction_int64_bitwise_shift_right:
            return evaluateInt64(context) >> evaluateInt64(context);
        case MaPLInstruction_int64_function_invocation: {
            MaPLParameter returnedValue = evaluateFunctionInvocation(context);
            if (returnedValue.dataType != MaPLDataType_int64) {
                if (!context->isUnusedCodepath) {
                    context->executionState = MaPLExecutionState_error;
                    if (returnedValue.dataType == MaPLDataType_string) {
                        freeStringIfNeeded((char *)returnedValue.stringValue);
                    }
                }
                return 0;
            }
            return returnedValue.int64Value;
        }
        case MaPLInstruction_int64_subscript_invocation: {
            MaPLParameter returnedValue = evaluateSubscriptInvocation(context);
            if (returnedValue.dataType != MaPLDataType_int64) {
                if (!context->isUnusedCodepath) {
                    context->executionState = MaPLExecutionState_error;
                    if (returnedValue.dataType == MaPLDataType_string) {
                        freeStringIfNeeded((char *)returnedValue.stringValue);
                    }
                }
                return 0;
            }
            return returnedValue.int64Value;
        }
        case MaPLInstruction_int64_ternary_conditional: {
            bool previousUnusedCodepath = context->isUnusedCodepath;
            int64_t result;
            if (evaluateBool(context)) {
                result = evaluateInt64(context);
                context->isUnusedCodepath = true;
                evaluateInt64(context);
                context->isUnusedCodepath = previousUnusedCodepath;
            } else {
                context->isUnusedCodepath = true;
                evaluateInt64(context);
                context->isUnusedCodepath = previousUnusedCodepath;
                result = evaluateInt64(context);
            }
            return result;
        }
        case MaPLInstruction_int64_typecast:
            switch (typeForInstruction(context->scriptBuffer[context->cursorPosition])) {
                case MaPLDataType_char:
                    return (int64_t)evaluateChar(context);
                case MaPLDataType_int32:
                    return (int64_t)evaluateInt32(context);
                case MaPLDataType_uint32:
                    return (int64_t)evaluateUint32(context);
                case MaPLDataType_uint64:
                    return (int64_t)evaluateUint64(context);
                case MaPLDataType_float32:
                    return (int64_t)evaluateFloat32(context);
                case MaPLDataType_float64:
                    return (int64_t)evaluateFloat64(context);
                case MaPLDataType_string: {
                    char *taggedString = evaluateString(context);
                    int64_t returnInt = (int64_t)atol(untagString(taggedString));
                    freeStringIfNeeded(taggedString);
                    return returnInt;
                }
                case MaPLDataType_boolean:
                    return evaluateBool(context) ? 1 : 0;
                default:
                    context->executionState = MaPLExecutionState_error;
                    break;
            }
            break;
        default:
            context->executionState = MaPLExecutionState_error;
            break;
    }
    return 0;
}

u_int32_t evaluateUint32(MaPLExecutionContext *context) {
    switch(readInstruction(context)) {
        case MaPLInstruction_uint32_literal: {
            u_int32_t literal = *((u_int32_t *)(context->scriptBuffer+context->cursorPosition));
            context->cursorPosition += sizeof(u_int32_t);
            return literal;
        }
        case MaPLInstruction_uint32_variable:
            return *((u_int32_t *)(context->primitiveTable+readMemoryAddress(context)));
        case MaPLInstruction_uint32_add:
            return evaluateUint32(context) + evaluateUint32(context);
        case MaPLInstruction_uint32_subtract:
            return evaluateUint32(context) - evaluateUint32(context);
        case MaPLInstruction_uint32_divide:
            return evaluateUint32(context) / evaluateUint32(context);
        case MaPLInstruction_uint32_multiply:
            return evaluateUint32(context) * evaluateUint32(context);
        case MaPLInstruction_uint32_modulo:
            return evaluateUint32(context) % evaluateUint32(context);
        case MaPLInstruction_uint32_bitwise_and:
            return evaluateUint32(context) & evaluateUint32(context);
        case MaPLInstruction_uint32_bitwise_or:
            return evaluateUint32(context) | evaluateUint32(context);
        case MaPLInstruction_uint32_bitwise_xor:
            return evaluateUint32(context) ^ evaluateUint32(context);
        case MaPLInstruction_uint32_bitwise_negation:
            return ~evaluateUint32(context);
        case MaPLInstruction_uint32_bitwise_shift_left:
            return evaluateUint32(context) << evaluateUint32(context);
        case MaPLInstruction_uint32_bitwise_shift_right:
            return evaluateUint32(context) >> evaluateUint32(context);
        case MaPLInstruction_uint32_function_invocation: {
            MaPLParameter returnedValue = evaluateFunctionInvocation(context);
            if (returnedValue.dataType != MaPLDataType_uint32) {
                if (!context->isUnusedCodepath) {
                    context->executionState = MaPLExecutionState_error;
                    if (returnedValue.dataType == MaPLDataType_string) {
                        freeStringIfNeeded((char *)returnedValue.stringValue);
                    }
                }
                return 0;
            }
            return returnedValue.uint32Value;
        }
        case MaPLInstruction_uint32_subscript_invocation: {
            MaPLParameter returnedValue = evaluateSubscriptInvocation(context);
            if (returnedValue.dataType != MaPLDataType_uint32) {
                if (!context->isUnusedCodepath) {
                    context->executionState = MaPLExecutionState_error;
                    if (returnedValue.dataType == MaPLDataType_string) {
                        freeStringIfNeeded((char *)returnedValue.stringValue);
                    }
                }
                return 0;
            }
            return returnedValue.uint32Value;
        }
        case MaPLInstruction_uint32_ternary_conditional: {
            bool previousUnusedCodepath = context->isUnusedCodepath;
            u_int32_t result;
            if (evaluateBool(context)) {
                result = evaluateUint32(context);
                context->isUnusedCodepath = true;
                evaluateUint32(context);
                context->isUnusedCodepath = previousUnusedCodepath;
            } else {
                context->isUnusedCodepath = true;
                evaluateUint32(context);
                context->isUnusedCodepath = previousUnusedCodepath;
                result = evaluateUint32(context);
            }
            return result;
        }
        case MaPLInstruction_uint32_typecast:
            switch (typeForInstruction(context->scriptBuffer[context->cursorPosition])) {
                case MaPLDataType_char:
                    return (u_int32_t)evaluateChar(context);
                case MaPLDataType_int32:
                    return (u_int32_t)evaluateInt32(context);
                case MaPLDataType_int64:
                    return (u_int32_t)evaluateInt64(context);
                case MaPLDataType_uint64:
                    return (u_int32_t)evaluateUint64(context);
                case MaPLDataType_float32:
                    return (u_int32_t)evaluateFloat32(context);
                case MaPLDataType_float64:
                    return (u_int32_t)evaluateFloat64(context);
                case MaPLDataType_string: {
                    char *taggedString = evaluateString(context);
                    u_int32_t returnInt = (u_int32_t)atol(untagString(taggedString));
                    freeStringIfNeeded(taggedString);
                    return returnInt;
                }
                case MaPLDataType_boolean:
                    return evaluateBool(context) ? 1 : 0;
                default:
                    context->executionState = MaPLExecutionState_error;
                    break;
            }
            break;
        default:
            context->executionState = MaPLExecutionState_error;
            break;
    }
    return 0;
}

u_int64_t evaluateUint64(MaPLExecutionContext *context) {
    switch(readInstruction(context)) {
        case MaPLInstruction_uint64_literal: {
            u_int64_t literal = *((u_int64_t *)(context->scriptBuffer+context->cursorPosition));
            context->cursorPosition += sizeof(u_int64_t);
            return literal;
        }
        case MaPLInstruction_uint64_variable:
            return *((u_int64_t *)(context->primitiveTable+readMemoryAddress(context)));
        case MaPLInstruction_uint64_add:
            return evaluateUint64(context) + evaluateUint64(context);
        case MaPLInstruction_uint64_subtract:
            return evaluateUint64(context) - evaluateUint64(context);
        case MaPLInstruction_uint64_divide:
            return evaluateUint64(context) / evaluateUint64(context);
        case MaPLInstruction_uint64_multiply:
            return evaluateUint64(context) * evaluateUint64(context);
        case MaPLInstruction_uint64_modulo:
            return evaluateUint64(context) % evaluateUint64(context);
        case MaPLInstruction_uint64_bitwise_and:
            return evaluateUint64(context) & evaluateUint64(context);
        case MaPLInstruction_uint64_bitwise_or:
            return evaluateUint64(context) | evaluateUint64(context);
        case MaPLInstruction_uint64_bitwise_xor:
            return evaluateUint64(context) ^ evaluateUint64(context);
        case MaPLInstruction_uint64_bitwise_negation:
            return ~evaluateUint64(context);
        case MaPLInstruction_uint64_bitwise_shift_left:
            return evaluateUint64(context) << evaluateUint64(context);
        case MaPLInstruction_uint64_bitwise_shift_right:
            return evaluateUint64(context) >> evaluateUint64(context);
        case MaPLInstruction_uint64_function_invocation: {
            MaPLParameter returnedValue = evaluateFunctionInvocation(context);
            if (returnedValue.dataType != MaPLDataType_uint64) {
                if (!context->isUnusedCodepath) {
                    context->executionState = MaPLExecutionState_error;
                    if (returnedValue.dataType == MaPLDataType_string) {
                        freeStringIfNeeded((char *)returnedValue.stringValue);
                    }
                }
                return 0;
            }
            return returnedValue.uint64Value;
        }
        case MaPLInstruction_uint64_subscript_invocation: {
            MaPLParameter returnedValue = evaluateSubscriptInvocation(context);
            if (returnedValue.dataType != MaPLDataType_uint64) {
                if (!context->isUnusedCodepath) {
                    context->executionState = MaPLExecutionState_error;
                    if (returnedValue.dataType == MaPLDataType_string) {
                        freeStringIfNeeded((char *)returnedValue.stringValue);
                    }
                }
                return 0;
            }
            return returnedValue.uint64Value;
        }
        case MaPLInstruction_uint64_ternary_conditional: {
            bool previousUnusedCodepath = context->isUnusedCodepath;
            u_int64_t result;
            if (evaluateBool(context)) {
                result = evaluateUint64(context);
                context->isUnusedCodepath = true;
                evaluateUint64(context);
                context->isUnusedCodepath = previousUnusedCodepath;
            } else {
                context->isUnusedCodepath = true;
                evaluateUint64(context);
                context->isUnusedCodepath = previousUnusedCodepath;
                result = evaluateUint64(context);
            }
            return result;
        }
        case MaPLInstruction_uint64_typecast:
            switch (typeForInstruction(context->scriptBuffer[context->cursorPosition])) {
                case MaPLDataType_char:
                    return (u_int64_t)evaluateChar(context);
                case MaPLDataType_int32:
                    return (u_int64_t)evaluateInt32(context);
                case MaPLDataType_int64:
                    return (u_int64_t)evaluateInt64(context);
                case MaPLDataType_uint32:
                    return (u_int64_t)evaluateUint32(context);
                case MaPLDataType_float32:
                    return (u_int64_t)evaluateFloat32(context);
                case MaPLDataType_float64:
                    return (u_int64_t)evaluateFloat64(context);
                case MaPLDataType_string: {
                    char *taggedString = evaluateString(context);
                    u_int64_t returnInt = (u_int64_t)atol(untagString(taggedString));
                    freeStringIfNeeded(taggedString);
                    return returnInt;
                }
                case MaPLDataType_boolean:
                    return evaluateBool(context) ? 1 : 0;
                default:
                    context->executionState = MaPLExecutionState_error;
                    break;
            }
            break;
        default:
            context->executionState = MaPLExecutionState_error;
            break;
    }
    return 0;
}

float evaluateFloat32(MaPLExecutionContext *context) {
    switch(readInstruction(context)) {
        case MaPLInstruction_float32_literal: {
            float literal = *((float *)(context->scriptBuffer+context->cursorPosition));
            context->cursorPosition += sizeof(float);
            return literal;
        }
        case MaPLInstruction_float32_variable:
            return *((float *)(context->primitiveTable+readMemoryAddress(context)));
        case MaPLInstruction_float32_add:
            return evaluateFloat32(context) + evaluateFloat32(context);
        case MaPLInstruction_float32_subtract:
            return evaluateFloat32(context) - evaluateFloat32(context);
        case MaPLInstruction_float32_divide:
            return evaluateFloat32(context) / evaluateFloat32(context);
        case MaPLInstruction_float32_multiply:
            return evaluateFloat32(context) * evaluateFloat32(context);
        case MaPLInstruction_float32_modulo: {
            float f1 = evaluateFloat32(context);
            float f2 = evaluateFloat32(context);
            return fmodf(f1, f2);
        }
        case MaPLInstruction_float32_numeric_negation:
            return -evaluateFloat32(context);
        case MaPLInstruction_float32_function_invocation: {
            MaPLParameter returnedValue = evaluateFunctionInvocation(context);
            if (returnedValue.dataType != MaPLDataType_float32) {
                if (!context->isUnusedCodepath) {
                    context->executionState = MaPLExecutionState_error;
                    if (returnedValue.dataType == MaPLDataType_string) {
                        freeStringIfNeeded((char *)returnedValue.stringValue);
                    }
                }
                return 0.0f;
            }
            return returnedValue.float32Value;
        }
        case MaPLInstruction_float32_subscript_invocation: {
            MaPLParameter returnedValue = evaluateSubscriptInvocation(context);
            if (returnedValue.dataType != MaPLDataType_float32) {
                if (!context->isUnusedCodepath) {
                    context->executionState = MaPLExecutionState_error;
                    if (returnedValue.dataType == MaPLDataType_string) {
                        freeStringIfNeeded((char *)returnedValue.stringValue);
                    }
                }
                return 0.0f;
            }
            return returnedValue.float32Value;
        }
        case MaPLInstruction_float32_ternary_conditional: {
            bool previousUnusedCodepath = context->isUnusedCodepath;
            float result;
            if (evaluateBool(context)) {
                result = evaluateFloat32(context);
                context->isUnusedCodepath = true;
                evaluateFloat32(context);
                context->isUnusedCodepath = previousUnusedCodepath;
            } else {
                context->isUnusedCodepath = true;
                evaluateFloat32(context);
                context->isUnusedCodepath = previousUnusedCodepath;
                result = evaluateFloat32(context);
            }
            return result;
        }
        case MaPLInstruction_float32_typecast:
            switch (typeForInstruction(context->scriptBuffer[context->cursorPosition])) {
                case MaPLDataType_char:
                    return (float)evaluateChar(context);
                case MaPLDataType_int32:
                    return (float)evaluateInt32(context);
                case MaPLDataType_int64:
                    return (float)evaluateInt64(context);
                case MaPLDataType_uint32:
                    return (float)evaluateUint32(context);
                case MaPLDataType_uint64:
                    return (float)evaluateUint64(context);
                case MaPLDataType_float64:
                    return (float)evaluateFloat64(context);
                case MaPLDataType_string: {
                    char *taggedString = evaluateString(context);
                    float returnFloat = (float)atof(untagString(taggedString));
                    freeStringIfNeeded(taggedString);
                    return returnFloat;
                }
                case MaPLDataType_boolean:
                    return evaluateBool(context) ? 1.0f : 0.0f;
                default:
                    context->executionState = MaPLExecutionState_error;
                    break;
            }
            break;
        default:
            context->executionState = MaPLExecutionState_error;
            break;
    }
    return 0.0f;
}

double evaluateFloat64(MaPLExecutionContext *context) {
    switch(readInstruction(context)) {
        case MaPLInstruction_float64_literal: {
            double literal = *((double *)(context->scriptBuffer+context->cursorPosition));
            context->cursorPosition += sizeof(double);
            return literal;
        }
        case MaPLInstruction_float64_variable:
            return *((double *)(context->primitiveTable+readMemoryAddress(context)));
        case MaPLInstruction_float64_add:
            return evaluateFloat64(context) + evaluateFloat64(context);
        case MaPLInstruction_float64_subtract:
            return evaluateFloat64(context) - evaluateFloat64(context);
        case MaPLInstruction_float64_divide:
            return evaluateFloat64(context) / evaluateFloat64(context);
        case MaPLInstruction_float64_multiply:
            return evaluateFloat64(context) * evaluateFloat64(context);
        case MaPLInstruction_float64_modulo: {
            double d1 = evaluateFloat64(context);
            double d2 = evaluateFloat64(context);
            return fmod(d1, d2);
        }
        case MaPLInstruction_float64_numeric_negation:
            return -evaluateFloat64(context);
        case MaPLInstruction_float64_function_invocation: {
            MaPLParameter returnedValue = evaluateFunctionInvocation(context);
            if (returnedValue.dataType != MaPLDataType_float64) {
                if (!context->isUnusedCodepath) {
                    context->executionState = MaPLExecutionState_error;
                    if (returnedValue.dataType == MaPLDataType_string) {
                        freeStringIfNeeded((char *)returnedValue.stringValue);
                    }
                }
                return 0.0;
            }
            return returnedValue.float64Value;
        }
        case MaPLInstruction_float64_subscript_invocation: {
            MaPLParameter returnedValue = evaluateSubscriptInvocation(context);
            if (returnedValue.dataType != MaPLDataType_float64) {
                if (!context->isUnusedCodepath) {
                    context->executionState = MaPLExecutionState_error;
                    if (returnedValue.dataType == MaPLDataType_string) {
                        freeStringIfNeeded((char *)returnedValue.stringValue);
                    }
                }
                return 0.0;
            }
            return returnedValue.float64Value;
        }
        case MaPLInstruction_float64_ternary_conditional: {
            bool previousUnusedCodepath = context->isUnusedCodepath;
            double result;
            if (evaluateBool(context)) {
                result = evaluateFloat64(context);
                context->isUnusedCodepath = true;
                evaluateFloat64(context);
                context->isUnusedCodepath = previousUnusedCodepath;
            } else {
                context->isUnusedCodepath = true;
                evaluateFloat64(context);
                context->isUnusedCodepath = previousUnusedCodepath;
                result = evaluateFloat64(context);
            }
            return result;
        }
        case MaPLInstruction_float64_typecast:
            switch (typeForInstruction(context->scriptBuffer[context->cursorPosition])) {
                case MaPLDataType_char:
                    return (double)evaluateChar(context);
                case MaPLDataType_int32:
                    return (double)evaluateInt32(context);
                case MaPLDataType_int64:
                    return (double)evaluateInt64(context);
                case MaPLDataType_uint32:
                    return (double)evaluateUint32(context);
                case MaPLDataType_uint64:
                    return (double)evaluateUint64(context);
                case MaPLDataType_float32:
                    return (double)evaluateFloat32(context);
                case MaPLDataType_string: {
                    char *taggedString = evaluateString(context);
                    double returnDouble = atof(untagString(taggedString));
                    freeStringIfNeeded(taggedString);
                    return returnDouble;
                }
                case MaPLDataType_boolean:
                    return evaluateBool(context) ? 1.0 : 0.0;
                default:
                    context->executionState = MaPLExecutionState_error;
                    break;
            }
            break;
        default:
            context->executionState = MaPLExecutionState_error;
            break;
    }
    return 0.0;
}

bool evaluateBool(MaPLExecutionContext *context) {
    switch(readInstruction(context)) {
        case MaPLInstruction_literal_true:
            return true;
        case MaPLInstruction_literal_false:
            return false;
        case MaPLInstruction_boolean_variable: {
            u_int8_t variableValue = *((u_int8_t *)(context->primitiveTable+readMemoryAddress(context)));
            return variableValue != 0;
        }
        case MaPLInstruction_boolean_function_invocation: {
            MaPLParameter returnedValue = evaluateFunctionInvocation(context);
            if (returnedValue.dataType != MaPLDataType_boolean) {
                if (!context->isUnusedCodepath) {
                    context->executionState = MaPLExecutionState_error;
                    if (returnedValue.dataType == MaPLDataType_string) {
                        freeStringIfNeeded((char *)returnedValue.stringValue);
                    }
                }
                return false;
            }
            return returnedValue.booleanValue;
        }
        case MaPLInstruction_boolean_subscript_invocation: {
            MaPLParameter returnedValue = evaluateSubscriptInvocation(context);
            if (returnedValue.dataType != MaPLDataType_boolean) {
                if (!context->isUnusedCodepath) {
                    context->executionState = MaPLExecutionState_error;
                    if (returnedValue.dataType == MaPLDataType_string) {
                        freeStringIfNeeded((char *)returnedValue.stringValue);
                    }
                }
                return false;
            }
            return returnedValue.booleanValue;
        }
        case MaPLInstruction_boolean_ternary_conditional: {
            bool previousUnusedCodepath = context->isUnusedCodepath;
            bool result;
            if (evaluateBool(context)) {
                result = evaluateBool(context);
                context->isUnusedCodepath = true;
                evaluateBool(context);
                context->isUnusedCodepath = previousUnusedCodepath;
            } else {
                context->isUnusedCodepath = true;
                evaluateBool(context);
                context->isUnusedCodepath = previousUnusedCodepath;
                result = evaluateBool(context);
            }
            return result;
        }
        case MaPLInstruction_boolean_typecast:
            switch (typeForInstruction(context->scriptBuffer[context->cursorPosition])) {
                case MaPLDataType_char:
                    return evaluateChar(context) != 0;
                case MaPLDataType_int32:
                    return evaluateInt32(context) != 0;
                case MaPLDataType_int64:
                    return evaluateInt64(context) != 0;
                case MaPLDataType_uint32:
                    return evaluateUint32(context) != 0;
                case MaPLDataType_uint64:
                    return evaluateUint64(context) != 0;
                case MaPLDataType_float32:
                    return evaluateFloat32(context) != 0.0f;
                case MaPLDataType_float64:
                    return evaluateFloat32(context) != 0.0;
                case MaPLDataType_string: {
                    char *taggedString = evaluateString(context);
                    bool returnBool = !strcmp(untagString(taggedString), "true");
                    freeStringIfNeeded(taggedString);
                    return returnBool;
                }
                default:
                    context->executionState = MaPLExecutionState_error;
                    break;
            }
            break;
        case MaPLInstruction_logical_equality_char:
            return evaluateChar(context) == evaluateChar(context);
        case MaPLInstruction_logical_equality_int32:
            return evaluateInt32(context) == evaluateInt32(context);
        case MaPLInstruction_logical_equality_int64:
            return evaluateInt64(context) == evaluateInt64(context);
        case MaPLInstruction_logical_equality_uint32:
            return evaluateUint32(context) == evaluateUint32(context);
        case MaPLInstruction_logical_equality_uint64:
            return evaluateUint64(context) == evaluateUint64(context);
        case MaPLInstruction_logical_equality_float32:
            return evaluateFloat32(context) == evaluateFloat32(context);
        case MaPLInstruction_logical_equality_float64:
            return evaluateFloat64(context) == evaluateFloat64(context);
        case MaPLInstruction_logical_equality_boolean:
            return evaluateBool(context) == evaluateBool(context);
        case MaPLInstruction_logical_equality_string: {
            char *taggedString1 = evaluateString(context);
            char *taggedString2 = evaluateString(context);
            bool returnValue = !strcmp(untagString(taggedString1), untagString(taggedString2));
            freeStringIfNeeded(taggedString1);
            freeStringIfNeeded(taggedString2);
            return returnValue;
        }
        case MaPLInstruction_logical_equality_pointer:
            return evaluatePointer(context) == evaluatePointer(context);
        case MaPLInstruction_logical_inequality_char:
            return evaluateChar(context) != evaluateChar(context);
        case MaPLInstruction_logical_inequality_int32:
            return evaluateInt32(context) != evaluateInt32(context);
        case MaPLInstruction_logical_inequality_int64:
            return evaluateInt64(context) != evaluateInt64(context);
        case MaPLInstruction_logical_inequality_uint32:
            return evaluateUint32(context) != evaluateUint32(context);
        case MaPLInstruction_logical_inequality_uint64:
            return evaluateUint64(context) != evaluateUint64(context);
        case MaPLInstruction_logical_inequality_float32:
            return evaluateFloat32(context) != evaluateFloat32(context);
        case MaPLInstruction_logical_inequality_float64:
            return evaluateFloat64(context) != evaluateFloat64(context);
        case MaPLInstruction_logical_inequality_boolean:
            return evaluateBool(context) != evaluateBool(context);
        case MaPLInstruction_logical_inequality_string: {
            char *taggedString1 = evaluateString(context);
            char *taggedString2 = evaluateString(context);
            bool returnValue = strcmp(untagString(taggedString1), untagString(taggedString2)) != 0;
            freeStringIfNeeded(taggedString1);
            freeStringIfNeeded(taggedString2);
            return returnValue;
        }
        case MaPLInstruction_logical_inequality_pointer:
            return evaluatePointer(context) != evaluatePointer(context);
        case MaPLInstruction_logical_less_than_char:
            return evaluateChar(context) < evaluateChar(context);
        case MaPLInstruction_logical_less_than_int32:
            return evaluateInt32(context) < evaluateInt32(context);
        case MaPLInstruction_logical_less_than_int64:
            return evaluateInt64(context) < evaluateInt64(context);
        case MaPLInstruction_logical_less_than_uint32:
            return evaluateUint32(context) < evaluateUint32(context);
        case MaPLInstruction_logical_less_than_uint64:
            return evaluateUint64(context) < evaluateUint64(context);
        case MaPLInstruction_logical_less_than_float32:
            return evaluateFloat32(context) < evaluateFloat32(context);
        case MaPLInstruction_logical_less_than_float64:
            return evaluateFloat64(context) < evaluateFloat64(context);
        case MaPLInstruction_logical_less_than_equal_char:
            return evaluateChar(context) <= evaluateChar(context);
        case MaPLInstruction_logical_less_than_equal_int32:
            return evaluateInt32(context) <= evaluateInt32(context);
        case MaPLInstruction_logical_less_than_equal_int64:
            return evaluateInt64(context) <= evaluateInt64(context);
        case MaPLInstruction_logical_less_than_equal_uint32:
            return evaluateUint32(context) <= evaluateUint32(context);
        case MaPLInstruction_logical_less_than_equal_uint64:
            return evaluateUint64(context) <= evaluateUint64(context);
        case MaPLInstruction_logical_less_than_equal_float32:
            return evaluateFloat32(context) <= evaluateFloat32(context);
        case MaPLInstruction_logical_less_than_equal_float64:
            return evaluateFloat64(context) <= evaluateFloat64(context);
        case MaPLInstruction_logical_greater_than_char:
            return evaluateChar(context) > evaluateChar(context);
        case MaPLInstruction_logical_greater_than_int32:
            return evaluateInt32(context) > evaluateInt32(context);
        case MaPLInstruction_logical_greater_than_int64:
            return evaluateInt64(context) > evaluateInt64(context);
        case MaPLInstruction_logical_greater_than_uint32:
            return evaluateUint32(context) > evaluateUint32(context);
        case MaPLInstruction_logical_greater_than_uint64:
            return evaluateUint64(context) > evaluateUint64(context);
        case MaPLInstruction_logical_greater_than_float32:
            return evaluateFloat32(context) > evaluateFloat32(context);
        case MaPLInstruction_logical_greater_than_float64:
            return evaluateFloat64(context) > evaluateFloat64(context);
        case MaPLInstruction_logical_greater_than_equal_char:
            return evaluateChar(context) >= evaluateChar(context);
        case MaPLInstruction_logical_greater_than_equal_int32:
            return evaluateInt32(context) >= evaluateInt32(context);
        case MaPLInstruction_logical_greater_than_equal_int64:
            return evaluateInt64(context) >= evaluateInt64(context);
        case MaPLInstruction_logical_greater_than_equal_uint32:
            return evaluateUint32(context) >= evaluateUint32(context);
        case MaPLInstruction_logical_greater_than_equal_uint64:
            return evaluateUint64(context) >= evaluateUint64(context);
        case MaPLInstruction_logical_greater_than_equal_float32:
            return evaluateFloat32(context) >= evaluateFloat32(context);
        case MaPLInstruction_logical_greater_than_equal_float64:
            return evaluateFloat64(context) >= evaluateFloat64(context);
        case MaPLInstruction_logical_and: {
            // If the first bool is false, we don't need to evaluate the second one.
            bool firstBool = evaluateBool(context);
            if (firstBool) {
                return evaluateBool(context);
            }
            bool previousUnusedCodepath = context->isUnusedCodepath;
            context->isUnusedCodepath = true;
            evaluateBool(context);
            context->isUnusedCodepath = previousUnusedCodepath;
            return false;
        }
        case MaPLInstruction_logical_or: {
            // If the first bool is true, we don't need to evaluate the second one.
            bool firstBool = evaluateBool(context);
            if (!firstBool) {
                return evaluateBool(context);
            }
            bool previousUnusedCodepath = context->isUnusedCodepath;
            context->isUnusedCodepath = true;
            evaluateBool(context);
            context->isUnusedCodepath = previousUnusedCodepath;
            return true;
        }
        case MaPLInstruction_logical_negation:
            return !evaluateBool(context);
        default:
            context->executionState = MaPLExecutionState_error;
            break;
    }
    return false;
}

void *evaluatePointer(MaPLExecutionContext *context) {
    return NULL; // TODO: Implement this.
}

char *evaluateString(MaPLExecutionContext *context) {
    // TODO: Skip any string allocations if isUnusedCodepath.
    return NULL; // TODO: Implement this.
}

void evaluateStatement(MaPLExecutionContext *context) {
    switch(readInstruction(context)) {
        case MaPLInstruction_unused_return_function_invocation:
            // TODO: Implement this.
            break;
        case MaPLInstruction_char_assign:
            // TODO: Implement this.
            break;
        case MaPLInstruction_int32_assign:
            // TODO: Implement this.
            break;
        case MaPLInstruction_int64_assign:
            // TODO: Implement this.
            break;
        case MaPLInstruction_uint32_assign:
            // TODO: Implement this.
            break;
        case MaPLInstruction_uint64_assign:
            // TODO: Implement this.
            break;
        case MaPLInstruction_float32_assign:
            // TODO: Implement this.
            break;
        case MaPLInstruction_float64_assign:
            // TODO: Implement this.
            break;
        case MaPLInstruction_boolean_assign:
            // TODO: Implement this.
            break;
        case MaPLInstruction_string_assign:
            // TODO: Implement this.
            break;
        case MaPLInstruction_pointer_assign:
            // TODO: Implement this.
            break;
        case MaPLInstruction_char_assign_subscript:
            // TODO: Implement this.
            break;
        case MaPLInstruction_int32_assign_subscript:
            // TODO: Implement this.
            break;
        case MaPLInstruction_int64_assign_subscript:
            // TODO: Implement this.
            break;
        case MaPLInstruction_uint32_assign_subscript:
            // TODO: Implement this.
            break;
        case MaPLInstruction_uint64_assign_subscript:
            // TODO: Implement this.
            break;
        case MaPLInstruction_float32_assign_subscript:
            // TODO: Implement this.
            break;
        case MaPLInstruction_float64_assign_subscript:
            // TODO: Implement this.
            break;
        case MaPLInstruction_boolean_assign_subscript:
            // TODO: Implement this.
            break;
        case MaPLInstruction_string_assign_subscript:
            // TODO: Implement this.
            break;
        case MaPLInstruction_pointer_assign_subscript:
            // TODO: Implement this.
            break;
        case MaPLInstruction_char_assign_property:
            // TODO: Implement this.
            break;
        case MaPLInstruction_int32_assign_property:
            // TODO: Implement this.
            break;
        case MaPLInstruction_int64_assign_property:
            // TODO: Implement this.
            break;
        case MaPLInstruction_uint32_assign_property:
            // TODO: Implement this.
            break;
        case MaPLInstruction_uint64_assign_property:
            // TODO: Implement this.
            break;
        case MaPLInstruction_float32_assign_property:
            // TODO: Implement this.
            break;
        case MaPLInstruction_float64_assign_property:
            // TODO: Implement this.
            break;
        case MaPLInstruction_boolean_assign_property:
            // TODO: Implement this.
            break;
        case MaPLInstruction_string_assign_property:
            // TODO: Implement this.
            break;
        case MaPLInstruction_pointer_assign_property:
            // TODO: Implement this.
            break;
        case MaPLInstruction_conditional:
            // TODO: Implement this.
            break;
        case MaPLInstruction_cursor_move_forward:
            // TODO: Implement this.
            break;
        case MaPLInstruction_cursor_move_back:
            // TODO: Implement this.
            break;
        case MaPLInstruction_program_exit:
            context->executionState = MaPLExecutionState_exit;
            break;
        case MaPLInstruction_metadata: {
            const char *metadata = readString(context);
            if (context->callbacks->metadata) {
                context->callbacks->metadata(metadata);
            }
        }
            break;
        case MaPLInstruction_debug_line:
            // TODO: Implement this.
            break;
        case MaPLInstruction_debug_update_variable:
            // TODO: Implement this.
            break;
        case MaPLInstruction_debug_delete_variable:
            // TODO: Implement this.
            break;
        default:
            context->executionState = MaPLExecutionState_error;
            break;
    }
}

void executeMaPLScript(const void* scriptBuffer, u_int16_t bufferLength, const MaPLCallbacks *callbacks) {
    // TODO: Here and in compiler assert the byte sizes of all types ( https://stackoverflow.com/a/18511691 ).
    
    MaPLExecutionContext context;
    context.scriptBuffer = (u_int8_t *)scriptBuffer;
    context.bufferLength = bufferLength;
    context.callbacks = callbacks;
    context.isUnusedCodepath = false;
    context.executionState = MaPLExecutionState_continue;
    
    // The first bytes are always two instances of MaPLMemoryAddress that describe the table sizes.
    // The entire script execution happens synchronously inside this function, so these tables can be stack allocated.
    MaPLMemoryAddress primitiveTableSize = *((MaPLMemoryAddress *)context.scriptBuffer);
    MaPLMemoryAddress stringTableSize = *((MaPLMemoryAddress *)context.scriptBuffer+sizeof(MaPLMemoryAddress));
    u_int8_t primitiveTable[primitiveTableSize];
    char *stringTable[stringTableSize];
    memset(stringTable, 0, sizeof(stringTable));
    context.primitiveTable = primitiveTable;
    context.stringTable = stringTable;
    context.cursorPosition = sizeof(MaPLMemoryAddress)*2;
    
    while (context.executionState == MaPLExecutionState_continue && context.cursorPosition < context.bufferLength) {
        evaluateStatement(&context);
    }
    if (context.executionState == MaPLExecutionState_error && context.callbacks->error) {
        context.callbacks->error();
    }
    
    // Free any remaining allocated strings.
    for(MaPLMemoryAddress i = 0; i < stringTableSize; i++) {
        freeStringIfNeeded(stringTable[i]);
    }
}

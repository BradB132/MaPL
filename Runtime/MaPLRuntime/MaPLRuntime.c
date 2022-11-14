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
#include <assert.h>

typedef enum {
    MaPLExecutionState_continue,
    MaPLExecutionState_exit,
    MaPLExecutionState_error,
} MaPLExecutionState;

typedef struct {
    const u_int8_t* scriptBuffer;
    MaPLBytecodeLength cursorPosition;
    u_int8_t *primitiveTable;
    const char **stringTable;
    const MaPLCallbacks *callbacks;
    bool isDeadCodepath;
    MaPLExecutionState executionState;
    MaPLRuntimeError errorType;
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
const char *evaluateString(MaPLExecutionContext *context);

// Memory management scheme for MaPL strings:
// The higher-order bits in 64-bit pointers are unused, so the MaPL runtime
// "tags" these pointers to store metadata about the usage of the string.
// Two flags are stored:
// 1- Allocated - Indicates when the string has been malloc'd and needs to be freed when no longer used.
// 2- Stored - Indicates if the string is stored in the string table.
//
// Example #1:
// string s = "foo"; -> Neither the Lvalue "s" or the literal Rvalue are allocated or stored.
//   The string is not allocated because the bytes for the string are embedded in the bytecode.
//   Strings are only marked as Stored if they were allocated.
//   If there was an allocated string previously stored in "s", it will be freed.
//
// Example #2:
// string s1 = mallocString(); -> Rvalue is allocated but not stored. Lvalue is the same memory address tagged as allocated and stored.
// string s2 = s1; -> "s1" is stored but changed to "not allocated" when referenced as Rvalue. Lvalue is allocated and stored.
//   Tagged pointers are not capable of a more complex memory management scheme like reference counting.
//   In the edge case of assigning the same malloc'd string to multiple variables, the string must be copied.
//
// Example #3:
// string s = mallocString(); -> Same as example #2.
// s = s;
//   In the edge case of assigning a stored string to itself, this should be a no-op.
const char *tagStringAsAllocated(const char *string) {
    return (char *)((uintptr_t)string | 0x8000000000000000);
}
const char *tagStringAsNotAllocated(const char *string) {
    return (char *)((uintptr_t)string & 0x7FFFFFFFFFFFFFFF);
}
bool isStringAllocated(const char *taggedString) {
    return ((uintptr_t)taggedString & 0x8000000000000000) != 0;
}
const char *tagStringAsStored(const char *string) {
    return (char *)((uintptr_t)string | 0x4000000000000000);
}
bool isStringStored(const char *taggedString) {
    return ((uintptr_t)taggedString & 0x4000000000000000) != 0;
}
const char *untagString(const char *string) {
    return (char *)((uintptr_t)string & 0x3FFFFFFFFFFFFFFF);
}
void freeStringIfNeeded(const char *string) {
    if (isStringAllocated(string)) {
        free((char *)untagString(string));
    }
}
void freeMaPLParameterIfNeeded(MaPLParameter *parameter) {
    if (parameter->dataType == MaPLDataType_string) {
        freeStringIfNeeded((char *)parameter->stringValue);
    }
}

MaPLParameter MaPLUninitialized(void) {
    return (MaPLParameter){ MaPLDataType_uninitialized };
}
MaPLParameter MaPLVoid(void) {
    return (MaPLParameter){ MaPLDataType_void };
}
MaPLParameter MaPLChar(u_int8_t charValue) {
    return (MaPLParameter){ MaPLDataType_char, .charValue = charValue };
}
MaPLParameter MaPLInt32(int32_t int32Value) {
    return (MaPLParameter){ MaPLDataType_int32, .int32Value = int32Value };
}
MaPLParameter MaPLInt64(int64_t int64Value) {
    return (MaPLParameter){ MaPLDataType_int64, .int64Value = int64Value };
}
MaPLParameter MaPLUint32(u_int32_t uint32Value) {
    return (MaPLParameter){ MaPLDataType_uint32, .uint32Value = uint32Value };
}
MaPLParameter MaPLUint64(u_int64_t uint64Value) {
    return (MaPLParameter){ MaPLDataType_uint64, .uint64Value = uint64Value };
}
MaPLParameter MaPLFloat32(float float32Value) {
    return (MaPLParameter){ MaPLDataType_float32, .float32Value = float32Value };
}
MaPLParameter MaPLFloat64(double float64Value) {
    return (MaPLParameter){ MaPLDataType_float64, .float64Value = float64Value };
}
MaPLParameter MaPLBool(bool booleanValue) {
    return (MaPLParameter){ MaPLDataType_boolean, .booleanValue = booleanValue };
}
MaPLParameter MaPLPointer(void *pointerValue) {
    return (MaPLParameter){ MaPLDataType_pointer, .pointerValue = pointerValue };
}
MaPLParameter MaPLStringByReference(const char *stringValue) {
    return (MaPLParameter){ MaPLDataType_string, .stringValue = stringValue };
}
MaPLParameter MaPLStringByValue(const char *stringValue) {
    size_t strLen = strlen(stringValue)+1;
    MaPLParameter parameter = { MaPLDataType_string, .stringValue = malloc(strLen) };
    memcpy((char *)parameter.stringValue, stringValue, strLen);
    parameter.stringValue = tagStringAsAllocated((char *)parameter.stringValue);
    return parameter;
}

bool verifyReturnValue(MaPLExecutionContext *context, MaPLParameter *returnValue, MaPLDataType expectedType) {
    if (returnValue->dataType != expectedType) {
        if (!context->isDeadCodepath) {
            context->executionState = MaPLExecutionState_error;
            context->errorType = MaPLRuntimeError_returnValueTypeMismatch;
            freeMaPLParameterIfNeeded(returnValue);
        }
        return false;
    }
    return true;
}

MaPLDataType typeForInstruction(enum MaPLInstruction instruction) {
    // Profiling shows that this predication approach is ~2.5x faster than conditionals.
    return (instruction <= MaPLInstruction_int32_typecast) * MaPLDataType_int32 +
        (instruction > MaPLInstruction_int32_typecast && instruction <= MaPLInstruction_float32_typecast) * MaPLDataType_float32 +
        (instruction > MaPLInstruction_float32_typecast && instruction <= MaPLInstruction_string_typecast) * MaPLDataType_string +
        (instruction > MaPLInstruction_string_typecast && instruction <= MaPLInstruction_pointer_ternary_conditional) * MaPLDataType_pointer +
        (instruction > MaPLInstruction_pointer_ternary_conditional && instruction <= MaPLInstruction_logical_negation) * MaPLDataType_boolean +
        (instruction > MaPLInstruction_logical_negation && instruction <= MaPLInstruction_int64_typecast) * MaPLDataType_int64 +
        (instruction > MaPLInstruction_int64_typecast && instruction <= MaPLInstruction_float64_typecast) * MaPLDataType_float64 +
        (instruction > MaPLInstruction_float64_typecast && instruction <= MaPLInstruction_uint32_typecast) * MaPLDataType_uint32 +
        (instruction > MaPLInstruction_uint32_typecast && instruction <= MaPLInstruction_uint64_typecast) * MaPLDataType_uint64 +
        (instruction > MaPLInstruction_uint64_typecast && instruction <= MaPLInstruction_char_typecast) * MaPLDataType_char +
        (instruction > MaPLInstruction_char_typecast) * MaPLDataType_void;
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

MaPLBytecodeLength readCursorMove(MaPLExecutionContext *context) {
    MaPLBytecodeLength move = *((MaPLBytecodeLength *)(context->scriptBuffer+context->cursorPosition));
    context->cursorPosition += sizeof(MaPLBytecodeLength);
    return move;
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

const char *concatenateStrings(const char *taggedString1, const char *taggedString2) {
    const char *untaggedString1 = untagString(taggedString1);
    const char *untaggedString2 = untagString(taggedString2);
    size_t strlen1 = strlen(untaggedString1);
    size_t strlen2 = strlen(untaggedString2)+1;
    char *concatString = malloc(strlen1+strlen2);
    memcpy(concatString, untaggedString1, strlen1);
    memcpy(concatString+strlen1, untaggedString2, strlen2);
    return tagStringAsAllocated(concatString);
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
            context->errorType = MaPLRuntimeError_malformedBytecode;
            return MaPLUninitialized();
    }
}

MaPLParameter evaluateFunctionInvocation(MaPLExecutionContext *context) {
    // This function assumes that we've already advanced past the initial "function_invocation" byte.
    void *invokedOnPointer = NULL;
    if (context->scriptBuffer[context->cursorPosition] == MaPLInstruction_no_op) {
        // This function is not invoked on another pointer, it's a global call.
        context->cursorPosition++;
    } else {
        invokedOnPointer = evaluatePointer(context);
        if (!invokedOnPointer && !context->isDeadCodepath) {
            context->executionState = MaPLExecutionState_error;
            context->errorType = MaPLRuntimeError_invocationOnNullPointer;
        }
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
    if (!context->callbacks->invokeFunction) {
        context->executionState = MaPLExecutionState_error;
        context->errorType = MaPLRuntimeError_missingCallback;
    }
    MaPLParameter returnValue = MaPLUninitialized();
    if (context->executionState == MaPLExecutionState_continue && !context->isDeadCodepath) {
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
    void *invokedOnPointer = evaluatePointer(context);
    if (!invokedOnPointer && !context->isDeadCodepath) {
        context->executionState = MaPLExecutionState_error;
        context->errorType = MaPLRuntimeError_invocationOnNullPointer;
    }
    
    MaPLParameter subscriptIndex = evaluateParameter(context);
    char *taggedIndex = NULL;
    if (subscriptIndex.dataType == MaPLDataType_string) {
        // Untag the string and store the tagged pointer for later release.
        taggedIndex = (char *)subscriptIndex.stringValue;
        subscriptIndex.stringValue = untagString((char *)subscriptIndex.stringValue);
    }
    
    // Invoke the subscript.
    if (!context->callbacks->invokeSubscript) {
        context->executionState = MaPLExecutionState_error;
        context->errorType = MaPLRuntimeError_missingCallback;
    }
    MaPLParameter returnValue = MaPLUninitialized();
    if (context->executionState == MaPLExecutionState_continue && !context->isDeadCodepath) {
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
            return verifyReturnValue(context, &returnedValue, MaPLDataType_char) ? returnedValue.charValue : 0;
        }
        case MaPLInstruction_char_subscript_invocation: {
            MaPLParameter returnedValue = evaluateSubscriptInvocation(context);
            return verifyReturnValue(context, &returnedValue, MaPLDataType_char) ? returnedValue.charValue : 0;
        }
        case MaPLInstruction_char_ternary_conditional: {
            bool previousDeadCodepath = context->isDeadCodepath;
            u_int8_t result;
            if (evaluateBool(context)) {
                result = evaluateChar(context);
                context->isDeadCodepath = true;
                evaluateChar(context);
                context->isDeadCodepath = previousDeadCodepath;
            } else {
                context->isDeadCodepath = true;
                evaluateChar(context);
                context->isDeadCodepath = previousDeadCodepath;
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
                    const char *taggedString = evaluateString(context);
                    u_int8_t returnChar = (u_int8_t)atoi(untagString(taggedString));
                    freeStringIfNeeded(taggedString);
                    return returnChar;
                }
                case MaPLDataType_boolean:
                    return evaluateBool(context) ? 1 : 0;
                default:
                    context->executionState = MaPLExecutionState_error;
                    context->errorType = MaPLRuntimeError_malformedBytecode;
                    break;
            }
            break;
        default:
            context->executionState = MaPLExecutionState_error;
            context->errorType = MaPLRuntimeError_malformedBytecode;
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
            return verifyReturnValue(context, &returnedValue, MaPLDataType_int32) ? returnedValue.int32Value : 0;
        }
        case MaPLInstruction_int32_subscript_invocation: {
            MaPLParameter returnedValue = evaluateSubscriptInvocation(context);
            return verifyReturnValue(context, &returnedValue, MaPLDataType_int32) ? returnedValue.int32Value : 0;
        }
        case MaPLInstruction_int32_ternary_conditional: {
            bool previousDeadCodepath = context->isDeadCodepath;
            int32_t result;
            if (evaluateBool(context)) {
                result = evaluateInt32(context);
                context->isDeadCodepath = true;
                evaluateInt32(context);
                context->isDeadCodepath = previousDeadCodepath;
            } else {
                context->isDeadCodepath = true;
                evaluateInt32(context);
                context->isDeadCodepath = previousDeadCodepath;
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
                    const char *taggedString = evaluateString(context);
                    int32_t returnInt = (int32_t)atoi(untagString(taggedString));
                    freeStringIfNeeded(taggedString);
                    return returnInt;
                }
                case MaPLDataType_boolean:
                    return evaluateBool(context) ? 1 : 0;
                default:
                    context->executionState = MaPLExecutionState_error;
                    context->errorType = MaPLRuntimeError_malformedBytecode;
                    break;
            }
            break;
        default:
            context->executionState = MaPLExecutionState_error;
            context->errorType = MaPLRuntimeError_malformedBytecode;
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
            return verifyReturnValue(context, &returnedValue, MaPLDataType_int64) ? returnedValue.int64Value : 0;
        }
        case MaPLInstruction_int64_subscript_invocation: {
            MaPLParameter returnedValue = evaluateSubscriptInvocation(context);
            return verifyReturnValue(context, &returnedValue, MaPLDataType_int64) ? returnedValue.int64Value : 0;
        }
        case MaPLInstruction_int64_ternary_conditional: {
            bool previousDeadCodepath = context->isDeadCodepath;
            int64_t result;
            if (evaluateBool(context)) {
                result = evaluateInt64(context);
                context->isDeadCodepath = true;
                evaluateInt64(context);
                context->isDeadCodepath = previousDeadCodepath;
            } else {
                context->isDeadCodepath = true;
                evaluateInt64(context);
                context->isDeadCodepath = previousDeadCodepath;
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
                    const char *taggedString = evaluateString(context);
                    int64_t returnInt = (int64_t)atol(untagString(taggedString));
                    freeStringIfNeeded(taggedString);
                    return returnInt;
                }
                case MaPLDataType_boolean:
                    return evaluateBool(context) ? 1 : 0;
                default:
                    context->executionState = MaPLExecutionState_error;
                    context->errorType = MaPLRuntimeError_malformedBytecode;
                    break;
            }
            break;
        default:
            context->executionState = MaPLExecutionState_error;
            context->errorType = MaPLRuntimeError_malformedBytecode;
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
            return verifyReturnValue(context, &returnedValue, MaPLDataType_uint32) ? returnedValue.uint32Value : 0;
        }
        case MaPLInstruction_uint32_subscript_invocation: {
            MaPLParameter returnedValue = evaluateSubscriptInvocation(context);
            return verifyReturnValue(context, &returnedValue, MaPLDataType_uint32) ? returnedValue.uint32Value : 0;
        }
        case MaPLInstruction_uint32_ternary_conditional: {
            bool previousDeadCodepath = context->isDeadCodepath;
            u_int32_t result;
            if (evaluateBool(context)) {
                result = evaluateUint32(context);
                context->isDeadCodepath = true;
                evaluateUint32(context);
                context->isDeadCodepath = previousDeadCodepath;
            } else {
                context->isDeadCodepath = true;
                evaluateUint32(context);
                context->isDeadCodepath = previousDeadCodepath;
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
                    const char *taggedString = evaluateString(context);
                    u_int32_t returnInt = (u_int32_t)atol(untagString(taggedString));
                    freeStringIfNeeded(taggedString);
                    return returnInt;
                }
                case MaPLDataType_boolean:
                    return evaluateBool(context) ? 1 : 0;
                default:
                    context->executionState = MaPLExecutionState_error;
                    context->errorType = MaPLRuntimeError_malformedBytecode;
                    break;
            }
            break;
        default:
            context->executionState = MaPLExecutionState_error;
            context->errorType = MaPLRuntimeError_malformedBytecode;
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
            return verifyReturnValue(context, &returnedValue, MaPLDataType_uint64) ? returnedValue.uint64Value : 0;
        }
        case MaPLInstruction_uint64_subscript_invocation: {
            MaPLParameter returnedValue = evaluateSubscriptInvocation(context);
            return verifyReturnValue(context, &returnedValue, MaPLDataType_uint64) ? returnedValue.uint64Value : 0;
        }
        case MaPLInstruction_uint64_ternary_conditional: {
            bool previousDeadCodepath = context->isDeadCodepath;
            u_int64_t result;
            if (evaluateBool(context)) {
                result = evaluateUint64(context);
                context->isDeadCodepath = true;
                evaluateUint64(context);
                context->isDeadCodepath = previousDeadCodepath;
            } else {
                context->isDeadCodepath = true;
                evaluateUint64(context);
                context->isDeadCodepath = previousDeadCodepath;
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
                    const char *taggedString = evaluateString(context);
                    u_int64_t returnInt = (u_int64_t)atol(untagString(taggedString));
                    freeStringIfNeeded(taggedString);
                    return returnInt;
                }
                case MaPLDataType_boolean:
                    return evaluateBool(context) ? 1 : 0;
                default:
                    context->executionState = MaPLExecutionState_error;
                    context->errorType = MaPLRuntimeError_malformedBytecode;
                    break;
            }
            break;
        default:
            context->executionState = MaPLExecutionState_error;
            context->errorType = MaPLRuntimeError_malformedBytecode;
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
            return verifyReturnValue(context, &returnedValue, MaPLDataType_float32) ? returnedValue.float32Value : 0.0f;
        }
        case MaPLInstruction_float32_subscript_invocation: {
            MaPLParameter returnedValue = evaluateSubscriptInvocation(context);
            return verifyReturnValue(context, &returnedValue, MaPLDataType_float32) ? returnedValue.float32Value : 0.0f;
        }
        case MaPLInstruction_float32_ternary_conditional: {
            bool previousDeadCodepath = context->isDeadCodepath;
            float result;
            if (evaluateBool(context)) {
                result = evaluateFloat32(context);
                context->isDeadCodepath = true;
                evaluateFloat32(context);
                context->isDeadCodepath = previousDeadCodepath;
            } else {
                context->isDeadCodepath = true;
                evaluateFloat32(context);
                context->isDeadCodepath = previousDeadCodepath;
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
                    const char *taggedString = evaluateString(context);
                    float returnFloat = (float)atof(untagString(taggedString));
                    freeStringIfNeeded(taggedString);
                    return returnFloat;
                }
                case MaPLDataType_boolean:
                    return evaluateBool(context) ? 1.0f : 0.0f;
                default:
                    context->executionState = MaPLExecutionState_error;
                    context->errorType = MaPLRuntimeError_malformedBytecode;
                    break;
            }
            break;
        default:
            context->executionState = MaPLExecutionState_error;
            context->errorType = MaPLRuntimeError_malformedBytecode;
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
            return verifyReturnValue(context, &returnedValue, MaPLDataType_float64) ? returnedValue.float64Value : 0.0;
        }
        case MaPLInstruction_float64_subscript_invocation: {
            MaPLParameter returnedValue = evaluateSubscriptInvocation(context);
            return verifyReturnValue(context, &returnedValue, MaPLDataType_float64) ? returnedValue.float64Value : 0.0;
        }
        case MaPLInstruction_float64_ternary_conditional: {
            bool previousDeadCodepath = context->isDeadCodepath;
            double result;
            if (evaluateBool(context)) {
                result = evaluateFloat64(context);
                context->isDeadCodepath = true;
                evaluateFloat64(context);
                context->isDeadCodepath = previousDeadCodepath;
            } else {
                context->isDeadCodepath = true;
                evaluateFloat64(context);
                context->isDeadCodepath = previousDeadCodepath;
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
                    const char *taggedString = evaluateString(context);
                    double returnDouble = atof(untagString(taggedString));
                    freeStringIfNeeded(taggedString);
                    return returnDouble;
                }
                case MaPLDataType_boolean:
                    return evaluateBool(context) ? 1.0 : 0.0;
                default:
                    context->executionState = MaPLExecutionState_error;
                    context->errorType = MaPLRuntimeError_malformedBytecode;
                    break;
            }
            break;
        default:
            context->executionState = MaPLExecutionState_error;
            context->errorType = MaPLRuntimeError_malformedBytecode;
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
            return verifyReturnValue(context, &returnedValue, MaPLDataType_boolean) ? returnedValue.booleanValue : false;
        }
        case MaPLInstruction_boolean_subscript_invocation: {
            MaPLParameter returnedValue = evaluateSubscriptInvocation(context);
            return verifyReturnValue(context, &returnedValue, MaPLDataType_boolean) ? returnedValue.booleanValue : false;
        }
        case MaPLInstruction_boolean_ternary_conditional: {
            bool previousDeadCodepath = context->isDeadCodepath;
            bool result;
            if (evaluateBool(context)) {
                result = evaluateBool(context);
                context->isDeadCodepath = true;
                evaluateBool(context);
                context->isDeadCodepath = previousDeadCodepath;
            } else {
                context->isDeadCodepath = true;
                evaluateBool(context);
                context->isDeadCodepath = previousDeadCodepath;
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
                    return evaluateFloat64(context) != 0.0;
                case MaPLDataType_string: {
                    const char *taggedString = evaluateString(context);
                    bool returnBool = !strcmp(untagString(taggedString), "true");
                    freeStringIfNeeded(taggedString);
                    return returnBool;
                }
                default:
                    context->executionState = MaPLExecutionState_error;
                    context->errorType = MaPLRuntimeError_malformedBytecode;
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
            const char *taggedString1 = evaluateString(context);
            const char *taggedString2 = evaluateString(context);
            if (context->isDeadCodepath) {
                // If this is a dead codepath, the strings that are returned are not allocated.
                return false;
            }
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
            const char *taggedString1 = evaluateString(context);
            const char *taggedString2 = evaluateString(context);
            if (context->isDeadCodepath) {
                // If this is a dead codepath, the strings that are returned are not allocated.
                return false;
            }
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
            bool previousDeadCodepath = context->isDeadCodepath;
            context->isDeadCodepath = true;
            evaluateBool(context);
            context->isDeadCodepath = previousDeadCodepath;
            return false;
        }
        case MaPLInstruction_logical_or: {
            // If the first bool is true, we don't need to evaluate the second one.
            bool firstBool = evaluateBool(context);
            if (!firstBool) {
                return evaluateBool(context);
            }
            bool previousDeadCodepath = context->isDeadCodepath;
            context->isDeadCodepath = true;
            evaluateBool(context);
            context->isDeadCodepath = previousDeadCodepath;
            return true;
        }
        case MaPLInstruction_logical_negation:
            return !evaluateBool(context);
        default:
            context->executionState = MaPLExecutionState_error;
            context->errorType = MaPLRuntimeError_malformedBytecode;
            break;
    }
    return false;
}

void *evaluatePointer(MaPLExecutionContext *context) {
    switch(readInstruction(context)) {
        case MaPLInstruction_literal_null:
            return NULL;
        case MaPLInstruction_pointer_variable:
            return *((void **)(context->primitiveTable+readMemoryAddress(context)));
        case MaPLInstruction_pointer_null_coalescing: {
            // If the first pointer is non-NULL, we don't need to evaluate the second one.
            void *firstPointer = evaluatePointer(context);
            if (!firstPointer) {
                return evaluatePointer(context);
            }
            bool previousDeadCodepath = context->isDeadCodepath;
            context->isDeadCodepath = true;
            evaluatePointer(context);
            context->isDeadCodepath = previousDeadCodepath;
            return firstPointer;
        }
        case MaPLInstruction_pointer_function_invocation: {
            MaPLParameter returnedValue = evaluateFunctionInvocation(context);
            return verifyReturnValue(context, &returnedValue, MaPLDataType_pointer) ? returnedValue.pointerValue : NULL;
        }
        case MaPLInstruction_pointer_subscript_invocation: {
            MaPLParameter returnedValue = evaluateSubscriptInvocation(context);
            return verifyReturnValue(context, &returnedValue, MaPLDataType_pointer) ? returnedValue.pointerValue : NULL;
        }
        case MaPLInstruction_pointer_ternary_conditional: {
            bool previousDeadCodepath = context->isDeadCodepath;
            void *result;
            if (evaluateBool(context)) {
                result = evaluatePointer(context);
                context->isDeadCodepath = true;
                evaluatePointer(context);
                context->isDeadCodepath = previousDeadCodepath;
            } else {
                context->isDeadCodepath = true;
                evaluatePointer(context);
                context->isDeadCodepath = previousDeadCodepath;
                result = evaluatePointer(context);
            }
            return result;
        }
        default:
            context->executionState = MaPLExecutionState_error;
            context->errorType = MaPLRuntimeError_malformedBytecode;
            break;
    }
    return NULL;
}

const char *evaluateString(MaPLExecutionContext *context) {
    switch(readInstruction(context)) {
        case MaPLInstruction_string_literal: {
            return readString(context);
        }
        case MaPLInstruction_string_variable:
            return tagStringAsNotAllocated(context->stringTable[readMemoryAddress(context)]);
        case MaPLInstruction_string_concat: {
            const char *taggedString1 = evaluateString(context);
            const char *taggedString2 = evaluateString(context);
            if (context->isDeadCodepath) {
                // If this is a dead codepath, the strings that are returned are not allocated.
                break;
            }
            const char *concatenatedString = concatenateStrings(taggedString1, taggedString2);
            freeStringIfNeeded(taggedString1);
            freeStringIfNeeded(taggedString2);
            return concatenatedString;
        }
        case MaPLInstruction_string_function_invocation: {
            MaPLParameter returnedValue = evaluateFunctionInvocation(context);
            return verifyReturnValue(context, &returnedValue, MaPLDataType_string) ? (char *)returnedValue.stringValue : NULL;
        }
        case MaPLInstruction_string_subscript_invocation: {
            MaPLParameter returnedValue = evaluateSubscriptInvocation(context);
            return verifyReturnValue(context, &returnedValue, MaPLDataType_string) ? (char *)returnedValue.stringValue : NULL;
        }
        case MaPLInstruction_string_ternary_conditional: {
            bool previousDeadCodepath = context->isDeadCodepath;
            const char *result;
            if (evaluateBool(context)) {
                result = evaluateString(context);
                context->isDeadCodepath = true;
                evaluateString(context);
                context->isDeadCodepath = previousDeadCodepath;
            } else {
                context->isDeadCodepath = true;
                evaluateString(context);
                context->isDeadCodepath = previousDeadCodepath;
                result = evaluateString(context);
            }
            return result;
        }
        case MaPLInstruction_string_typecast: {
            if (context->isDeadCodepath) {
                switch (typeForInstruction(context->scriptBuffer[context->cursorPosition])) {
                    case MaPLDataType_char:
                        evaluateChar(context);
                        break;
                    case MaPLDataType_int32:
                        evaluateInt32(context);
                        break;
                    case MaPLDataType_int64:
                        evaluateInt64(context);
                        break;
                    case MaPLDataType_uint32:
                        evaluateUint32(context);
                        break;
                    case MaPLDataType_uint64:
                        evaluateUint64(context);
                        break;
                    case MaPLDataType_float32:
                        evaluateFloat32(context);
                        break;
                    case MaPLDataType_float64:
                        evaluateFloat64(context);
                        break;
                    case MaPLDataType_boolean:
                        evaluateBool(context);
                        break;
                    case MaPLDataType_pointer:
                        evaluatePointer(context);
                        break;
                    default:
                        break;
                }
                return NULL;
            }
            char *returnString = malloc(24);
            switch (typeForInstruction(context->scriptBuffer[context->cursorPosition])) {
                case MaPLDataType_char:
                    snprintf(returnString, 24, "%hhu", evaluateChar(context));
                    break;
                case MaPLDataType_int32:
                    snprintf(returnString, 24, "%d", evaluateInt32(context));
                    break;
                case MaPLDataType_int64:
                    snprintf(returnString, 24, "%lld", evaluateInt64(context));
                    break;
                case MaPLDataType_uint32:
                    snprintf(returnString, 24, "%u", evaluateUint32(context));
                    break;
                case MaPLDataType_uint64:
                    snprintf(returnString, 24, "%lld", evaluateUint64(context));
                    break;
                case MaPLDataType_float32:
                    snprintf(returnString, 24, "%g", evaluateFloat32(context));
                    break;
                case MaPLDataType_float64:
                    snprintf(returnString, 24, "%g", evaluateFloat64(context));
                    break;
                case MaPLDataType_boolean:
                    if (evaluateBool(context)) {
                        strcpy(returnString, "true");
                    } else {
                        strcpy(returnString, "false");
                    }
                    break;
                case MaPLDataType_pointer: {
                    snprintf(returnString, 24, "0x%#016lX", (uintptr_t)evaluatePointer(context));
                    break;
                }
                default:
                    returnString[0] = 0;
                    context->executionState = MaPLExecutionState_error;
                    context->errorType = MaPLRuntimeError_malformedBytecode;
                    break;
            }
            return tagStringAsAllocated(returnString);
        }
        default:
            context->executionState = MaPLExecutionState_error;
            context->errorType = MaPLRuntimeError_malformedBytecode;
            break;
    }
    return NULL;
}

MaPLParameter applyOperatorAssign(MaPLExecutionContext *context, enum MaPLInstruction operatorAssignInstruction, MaPLParameter *initialValue, MaPLParameter *incrementValue) {
    MaPLParameter result = MaPLUninitialized();
    if (verifyReturnValue(context, initialValue, incrementValue->dataType)) {
        result.dataType = typeForInstruction(operatorAssignInstruction);
        switch (operatorAssignInstruction) {
            case MaPLInstruction_int32_add:
                result.int32Value = initialValue->int32Value + incrementValue->int32Value;
                break;
            case MaPLInstruction_int32_subtract:
                result.int32Value = initialValue->int32Value - incrementValue->int32Value;
                break;
            case MaPLInstruction_int32_divide:
                result.int32Value = initialValue->int32Value / incrementValue->int32Value;
                break;
            case MaPLInstruction_int32_multiply:
                result.int32Value = initialValue->int32Value * incrementValue->int32Value;
                break;
            case MaPLInstruction_int32_modulo:
                result.int32Value = initialValue->int32Value % incrementValue->int32Value;
                break;
            case MaPLInstruction_int32_bitwise_and:
                result.int32Value = initialValue->int32Value & incrementValue->int32Value;
                break;
            case MaPLInstruction_int32_bitwise_or:
                result.int32Value = initialValue->int32Value | incrementValue->int32Value;
                break;
            case MaPLInstruction_int32_bitwise_xor:
                result.int32Value = initialValue->int32Value ^ incrementValue->int32Value;
                break;
            case MaPLInstruction_int32_bitwise_shift_left:
                result.int32Value = initialValue->int32Value << incrementValue->int32Value;
                break;
            case MaPLInstruction_int32_bitwise_shift_right:
                result.int32Value = initialValue->int32Value >> incrementValue->int32Value;
                break;
            case MaPLInstruction_float32_add:
                result.float32Value = initialValue->float32Value + incrementValue->float32Value;
                break;
            case MaPLInstruction_float32_subtract:
                result.float32Value = initialValue->float32Value - incrementValue->float32Value;
                break;
            case MaPLInstruction_float32_divide:
                result.float32Value = initialValue->float32Value / incrementValue->float32Value;
                break;
            case MaPLInstruction_float32_multiply:
                result.float32Value = initialValue->float32Value * incrementValue->float32Value;
                break;
            case MaPLInstruction_float32_modulo:
                result.float32Value = fmodf(initialValue->float32Value, incrementValue->float32Value);
                break;
            case MaPLInstruction_string_concat:
                result.stringValue = concatenateStrings((char *)initialValue->stringValue, (char *)incrementValue->stringValue);
                break;
            case MaPLInstruction_int64_add:
                result.int64Value = initialValue->int64Value + incrementValue->int64Value;
                break;
            case MaPLInstruction_int64_subtract:
                result.int64Value = initialValue->int64Value - incrementValue->int64Value;
                break;
            case MaPLInstruction_int64_divide:
                result.int64Value = initialValue->int64Value / incrementValue->int64Value;
                break;
            case MaPLInstruction_int64_multiply:
                result.int64Value = initialValue->int64Value * incrementValue->int64Value;
                break;
            case MaPLInstruction_int64_modulo:
                result.int64Value = initialValue->int64Value % incrementValue->int64Value;
                break;
            case MaPLInstruction_int64_bitwise_and:
                result.int64Value = initialValue->int64Value & incrementValue->int64Value;
                break;
            case MaPLInstruction_int64_bitwise_or:
                result.int64Value = initialValue->int64Value | incrementValue->int64Value;
                break;
            case MaPLInstruction_int64_bitwise_xor:
                result.int64Value = initialValue->int64Value ^ incrementValue->int64Value;
                break;
            case MaPLInstruction_int64_bitwise_shift_left:
                result.int64Value = initialValue->int64Value << incrementValue->int64Value;
                break;
            case MaPLInstruction_int64_bitwise_shift_right:
                result.int64Value = initialValue->int64Value >> incrementValue->int64Value;
                break;
            case MaPLInstruction_float64_add:
                result.float64Value = initialValue->float64Value + incrementValue->float64Value;
                break;
            case MaPLInstruction_float64_subtract:
                result.float64Value = initialValue->float64Value - incrementValue->float64Value;
                break;
            case MaPLInstruction_float64_divide:
                result.float64Value = initialValue->float64Value / incrementValue->float64Value;
                break;
            case MaPLInstruction_float64_multiply:
                result.float64Value = initialValue->float64Value * incrementValue->float64Value;
                break;
            case MaPLInstruction_float64_modulo:
                result.float64Value = fmod(initialValue->float64Value, incrementValue->float64Value);
                break;
            case MaPLInstruction_uint32_add:
                result.uint32Value = initialValue->uint32Value + incrementValue->uint32Value;
                break;
            case MaPLInstruction_uint32_subtract:
                result.uint32Value = initialValue->uint32Value - incrementValue->uint32Value;
                break;
            case MaPLInstruction_uint32_divide:
                result.uint32Value = initialValue->uint32Value / incrementValue->uint32Value;
                break;
            case MaPLInstruction_uint32_multiply:
                result.uint32Value = initialValue->uint32Value * incrementValue->uint32Value;
                break;
            case MaPLInstruction_uint32_modulo:
                result.uint32Value = initialValue->uint32Value % incrementValue->uint32Value;
                break;
            case MaPLInstruction_uint32_bitwise_and:
                result.uint32Value = initialValue->uint32Value & incrementValue->uint32Value;
                break;
            case MaPLInstruction_uint32_bitwise_or:
                result.uint32Value = initialValue->uint32Value | incrementValue->uint32Value;
                break;
            case MaPLInstruction_uint32_bitwise_xor:
                result.uint32Value = initialValue->uint32Value ^ incrementValue->uint32Value;
                break;
            case MaPLInstruction_uint32_bitwise_shift_left:
                result.uint32Value = initialValue->uint32Value << incrementValue->uint32Value;
                break;
            case MaPLInstruction_uint32_bitwise_shift_right:
                result.uint32Value = initialValue->uint32Value >> incrementValue->uint32Value;
                break;
            case MaPLInstruction_uint64_add:
                result.uint64Value = initialValue->uint64Value + incrementValue->uint64Value;
                break;
            case MaPLInstruction_uint64_subtract:
                result.uint64Value = initialValue->uint64Value - incrementValue->uint64Value;
                break;
            case MaPLInstruction_uint64_divide:
                result.uint64Value = initialValue->uint64Value / incrementValue->uint64Value;
                break;
            case MaPLInstruction_uint64_multiply:
                result.uint64Value = initialValue->uint64Value * incrementValue->uint64Value;
                break;
            case MaPLInstruction_uint64_modulo:
                result.uint64Value = initialValue->uint64Value % incrementValue->uint64Value;
                break;
            case MaPLInstruction_uint64_bitwise_and:
                result.uint64Value = initialValue->uint64Value & incrementValue->uint64Value;
                break;
            case MaPLInstruction_uint64_bitwise_or:
                result.uint64Value = initialValue->uint64Value | incrementValue->uint64Value;
                break;
            case MaPLInstruction_uint64_bitwise_xor:
                result.uint64Value = initialValue->uint64Value ^ incrementValue->uint64Value;
                break;
            case MaPLInstruction_uint64_bitwise_shift_left:
                result.uint64Value = initialValue->uint64Value << incrementValue->uint64Value;
                break;
            case MaPLInstruction_uint64_bitwise_shift_right:
                result.uint64Value = initialValue->uint64Value >> incrementValue->uint64Value;
                break;
            case MaPLInstruction_char_add:
                result.charValue = initialValue->charValue + incrementValue->charValue;
                break;
            case MaPLInstruction_char_subtract:
                result.charValue = initialValue->charValue - incrementValue->charValue;
                break;
            case MaPLInstruction_char_divide:
                result.charValue = initialValue->charValue / incrementValue->charValue;
                break;
            case MaPLInstruction_char_multiply:
                result.charValue = initialValue->charValue * incrementValue->charValue;
                break;
            case MaPLInstruction_char_modulo:
                result.charValue = initialValue->charValue % incrementValue->charValue;
                break;
            case MaPLInstruction_char_bitwise_and:
                result.charValue = initialValue->charValue & incrementValue->charValue;
                break;
            case MaPLInstruction_char_bitwise_or:
                result.charValue = initialValue->charValue | incrementValue->charValue;
                break;
            case MaPLInstruction_char_bitwise_xor:
                result.charValue = initialValue->charValue ^ incrementValue->charValue;
                break;
            case MaPLInstruction_char_bitwise_shift_left:
                result.charValue = initialValue->charValue << incrementValue->charValue;
                break;
            case MaPLInstruction_char_bitwise_shift_right:
                result.charValue = initialValue->charValue >> incrementValue->charValue;
                break;
            default:
                context->executionState = MaPLExecutionState_error;
                context->errorType = MaPLRuntimeError_malformedBytecode;
                return MaPLUninitialized();
        }
    }
    return result;
}

void evaluateStatement(MaPLExecutionContext *context) {
    switch(readInstruction(context)) {
        case MaPLInstruction_unused_return_function_invocation: {
            MaPLParameter returnedValue = evaluateFunctionInvocation(context);
            if (returnedValue.dataType == MaPLDataType_string) {
                freeStringIfNeeded((char *)returnedValue.stringValue);
            }
        }
            break;
        case MaPLInstruction_char_assign: {
            MaPLMemoryAddress variableAddress = readMemoryAddress(context);
            *(u_int8_t *)(context->primitiveTable+variableAddress) = evaluateChar(context);
        }
            break;
        case MaPLInstruction_int32_assign: {
            MaPLMemoryAddress variableAddress = readMemoryAddress(context);
            *(int32_t *)(context->primitiveTable+variableAddress) = evaluateInt32(context);
        }
            break;
        case MaPLInstruction_int64_assign: {
            MaPLMemoryAddress variableAddress = readMemoryAddress(context);
            *(int64_t *)(context->primitiveTable+variableAddress) = evaluateInt64(context);
        }
            break;
        case MaPLInstruction_uint32_assign: {
            MaPLMemoryAddress variableAddress = readMemoryAddress(context);
            *(u_int32_t *)(context->primitiveTable+variableAddress) = evaluateUint32(context);
        }
            break;
        case MaPLInstruction_uint64_assign: {
            MaPLMemoryAddress variableAddress = readMemoryAddress(context);
            *(u_int64_t *)(context->primitiveTable+variableAddress) = evaluateUint64(context);
        }
            break;
        case MaPLInstruction_float32_assign: {
            MaPLMemoryAddress variableAddress = readMemoryAddress(context);
            *(float *)(context->primitiveTable+variableAddress) = evaluateFloat32(context);
        }
            break;
        case MaPLInstruction_float64_assign: {
            MaPLMemoryAddress variableAddress = readMemoryAddress(context);
            *(double *)(context->primitiveTable+variableAddress) = evaluateFloat64(context);
        }
            break;
        case MaPLInstruction_boolean_assign: {
            MaPLMemoryAddress variableAddress = readMemoryAddress(context);
            *(u_int8_t *)(context->primitiveTable+variableAddress) = (u_int8_t)evaluateBool(context);
        }
            break;
        case MaPLInstruction_string_assign: {
            MaPLMemoryAddress stringIndex = readMemoryAddress(context);
            const char *assignedString = evaluateString(context);
            const char *existingString = context->stringTable[stringIndex];
            if (untagString(assignedString) == untagString(existingString)) {
                // In the edge case where a string is assigned to itself, do nothing.
                break;
            }
            freeStringIfNeeded(existingString);
            if (isStringStored(assignedString)) {
                // In the edge case that this value is the same pointer that's stored elsewhere
                // in the table, we need to do a copy to manage the memory correctly.
                const char *untaggedString = untagString(assignedString);
                size_t strLen = strlen(untaggedString)+1;
                char *copiedAssignedString = malloc(strLen);
                memcpy(copiedAssignedString, untaggedString, strLen);
                assignedString = tagStringAsAllocated(copiedAssignedString);
            }
            if (isStringAllocated(assignedString)) {
                // Only allocated strings need to track stored vs not stored.
                assignedString = tagStringAsStored(assignedString);
            }
            context->stringTable[stringIndex] = assignedString;
        }
            break;
        case MaPLInstruction_pointer_assign: {
            MaPLMemoryAddress variableAddress = readMemoryAddress(context);
            *(void **)(context->primitiveTable+variableAddress) = evaluatePointer(context);
        }
            break;
        case MaPLInstruction_assign_subscript: {
            void *invokedOnPointer = evaluatePointer(context);
            if (!invokedOnPointer) {
                context->executionState = MaPLExecutionState_error;
                context->errorType = MaPLRuntimeError_invocationOnNullPointer;
            }
            
            MaPLParameter subscriptIndex = evaluateParameter(context);
            char *taggedIndex = NULL;
            if (subscriptIndex.dataType == MaPLDataType_string) {
                // Untag the string and store the tagged pointer for later release.
                taggedIndex = (char *)subscriptIndex.stringValue;
                subscriptIndex.stringValue = untagString((char *)subscriptIndex.stringValue);
            }
            
            enum MaPLInstruction operatorAssignInstruction = readInstruction(context);
            MaPLParameter assignedExpression = evaluateParameter(context);
            
            if (operatorAssignInstruction != MaPLInstruction_no_op) {
                // This is an increment operator. Read from the subscript to get the initial value.
                if (!context->callbacks->invokeSubscript) {
                    context->executionState = MaPLExecutionState_error;
                    context->errorType = MaPLRuntimeError_missingCallback;
                }
                MaPLParameter initialValue = MaPLUninitialized();
                if (context->executionState == MaPLExecutionState_continue) {
                    initialValue = context->callbacks->invokeSubscript(invokedOnPointer, subscriptIndex);
                    MaPLParameter incrementedValue = applyOperatorAssign(context, operatorAssignInstruction, &initialValue, &assignedExpression);
                    freeMaPLParameterIfNeeded(&initialValue);
                    freeMaPLParameterIfNeeded(&assignedExpression);
                    assignedExpression = incrementedValue;
                }
            }
            
            // Assign the subscript.
            if (!context->callbacks->assignSubscript) {
                context->executionState = MaPLExecutionState_error;
                context->errorType = MaPLRuntimeError_missingCallback;
            }
            if (context->executionState == MaPLExecutionState_continue) {
                char *taggedAssignedString = NULL;
                if (assignedExpression.dataType == MaPLDataType_string) {
                    // Untag the string and store the tagged pointer for later release.
                    taggedAssignedString = (char *)assignedExpression.stringValue;
                    assignedExpression.stringValue = untagString((char *)assignedExpression.stringValue);
                }
                context->callbacks->assignSubscript(invokedOnPointer, subscriptIndex, assignedExpression);
                assignedExpression.stringValue = taggedAssignedString;
            }
            
            // Clean up strings.
            freeStringIfNeeded(taggedIndex);
            freeMaPLParameterIfNeeded(&assignedExpression);
        }
            break;
        case MaPLInstruction_assign_property: {
            void *invokedOnPointer = NULL;
            if (context->scriptBuffer[context->cursorPosition] == MaPLInstruction_no_op) {
                // This property is not invoked on another pointer, it's a global call.
                context->cursorPosition++;
            } else {
                invokedOnPointer = evaluatePointer(context);
                if (!invokedOnPointer) {
                    context->executionState = MaPLExecutionState_error;
                    context->errorType = MaPLRuntimeError_invocationOnNullPointer;
                }
            }
            
            MaPLSymbol symbol = readSymbol(context);
            enum MaPLInstruction operatorAssignInstruction = readInstruction(context);
            MaPLParameter assignedExpression = evaluateParameter(context);
            
            if (operatorAssignInstruction != MaPLInstruction_no_op) {
                // This is an increment operator. Read from the property to get the initial value.
                if (!context->callbacks->invokeFunction) {
                    context->executionState = MaPLExecutionState_error;
                    context->errorType = MaPLRuntimeError_missingCallback;
                }
                MaPLParameter initialValue = MaPLUninitialized();
                if (context->executionState == MaPLExecutionState_continue) {
                    initialValue = context->callbacks->invokeFunction(invokedOnPointer, symbol, NULL, 0);
                    MaPLParameter incrementedValue = applyOperatorAssign(context, operatorAssignInstruction, &initialValue, &assignedExpression);
                    freeMaPLParameterIfNeeded(&initialValue);
                    freeMaPLParameterIfNeeded(&assignedExpression);
                    assignedExpression = incrementedValue;
                }
            }
            
            // Assign the property.
            if (!context->callbacks->assignProperty) {
                context->executionState = MaPLExecutionState_error;
                context->errorType = MaPLRuntimeError_missingCallback;
            }
            if (context->executionState == MaPLExecutionState_continue) {
                char *taggedAssignedString = NULL;
                if (assignedExpression.dataType == MaPLDataType_string) {
                    // Untag the string and store the tagged pointer for later release.
                    taggedAssignedString = (char *)assignedExpression.stringValue;
                    assignedExpression.stringValue = untagString((char *)assignedExpression.stringValue);
                }
                context->callbacks->assignProperty(invokedOnPointer, symbol, assignedExpression);
                assignedExpression.stringValue = taggedAssignedString;
            }
            
            // Clean up assigned expression.
            freeMaPLParameterIfNeeded(&assignedExpression);
        }
            break;
        case MaPLInstruction_conditional: {
            bool conditional = evaluateBool(context);
            MaPLBytecodeLength move = readCursorMove(context);
            if (!conditional) {
                context->cursorPosition += move;
            }
        }
            break;
        case MaPLInstruction_cursor_move_forward: {
            MaPLBytecodeLength move = readCursorMove(context);
            context->cursorPosition += move;
        }
            break;
        case MaPLInstruction_cursor_move_back: {
            MaPLBytecodeLength move = readCursorMove(context);
            context->cursorPosition -= move;
        }
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
        case MaPLInstruction_debug_line: {
            MaPLLineNumber lineNumber = *((MaPLLineNumber *)(context->scriptBuffer+context->cursorPosition));
            context->cursorPosition += sizeof(MaPLLineNumber);
            if (context->callbacks->debugLine) {
                context->callbacks->debugLine(lineNumber);
            }
        }
            break;
        case MaPLInstruction_debug_update_variable: {
            const char *variableName = readString(context);
            MaPLParameter variableValue = evaluateParameter(context);
            if (context->callbacks->debugVariableUpdate) {
                char *taggedString = NULL;
                if (variableValue.dataType == MaPLDataType_string) {
                    // Untag the string and store the tagged pointer for later release.
                    taggedString = (char *)variableValue.stringValue;
                    variableValue.stringValue = untagString((char *)variableValue.stringValue);
                }
                context->callbacks->debugVariableUpdate(variableName, variableValue);
                freeStringIfNeeded(taggedString);
            }
        }
            break;
        case MaPLInstruction_debug_delete_variable: {
            const char *variableName = readString(context);
            if (context->callbacks->debugVariableDelete) {
                context->callbacks->debugVariableDelete(variableName);
            }
        }
            break;
        default:
            context->executionState = MaPLExecutionState_error;
            context->errorType = MaPLRuntimeError_malformedBytecode;
            break;
    }
}

void executeMaPLScript(const void* scriptBuffer, MaPLBytecodeLength bufferLength, const MaPLCallbacks *callbacks) {
    _Static_assert(sizeof(float) == 4, "MaPL assumes that 'float' type is 32-bit.");
    _Static_assert(sizeof(double) == 8, "MaPL assumes that 'double' type is 64-bit.");
    
    MaPLExecutionContext context;
    context.scriptBuffer = (u_int8_t *)scriptBuffer;
    context.callbacks = callbacks;
    context.isDeadCodepath = false;
    context.executionState = MaPLExecutionState_continue;

    // The first byte indicates big vs little endian (equals 1 if little endian).
    // If these bytes don't match, then the script was compiled with a different
    // endianness and needs to be recompiled.
    uint16_t endianShort = 1;
    uint8_t endianByte = *(uint8_t *)&endianShort;
    if (context.scriptBuffer[0] != endianByte) {
        if (context.callbacks->error) {
            context.callbacks->error(MaPLRuntimeError_incompatibleEndianness);
        }
        return;
    }
    
    // The next bytes are always two instances of MaPLMemoryAddress that describe the table sizes.
    // The entire script execution happens synchronously inside this function, so these tables can be stack allocated.
    MaPLMemoryAddress primitiveTableSize = *((MaPLMemoryAddress *)(context.scriptBuffer+sizeof(uint8_t)));
    MaPLMemoryAddress stringTableSize = *((MaPLMemoryAddress *)(context.scriptBuffer+sizeof(MaPLMemoryAddress)+sizeof(uint8_t)));
    u_int8_t primitiveTable[primitiveTableSize];
    const char *stringTable[stringTableSize];
    memset(stringTable, 0, sizeof(stringTable));
    context.primitiveTable = primitiveTable;
    context.stringTable = stringTable;
    context.cursorPosition = sizeof(MaPLMemoryAddress)*2+sizeof(uint8_t);
    
    while (context.executionState == MaPLExecutionState_continue && context.cursorPosition < bufferLength) {
        evaluateStatement(&context);
    }
    if (context.executionState == MaPLExecutionState_error && context.callbacks->error) {
        context.callbacks->error(context.errorType);
    }
    
    // Free any remaining allocated strings.
    for(MaPLMemoryAddress i = 0; i < stringTableSize; i++) {
        freeStringIfNeeded(stringTable[i]);
    }
}

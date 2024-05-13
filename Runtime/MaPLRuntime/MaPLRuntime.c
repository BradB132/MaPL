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
    const uint8_t* scriptBuffer;
    size_t cursorPosition;
    uint8_t *primitiveTable;
    const char **stringTable;
    const MaPLCallbacks *callbacks;
    bool isDeadCodepath;
    MaPLExecutionState executionState;
    MaPLRuntimeError errorType;
    MaPLParameter *parameterList;
    const char **taggedStringParameterList;
    MaPLParameterCount parameterListCount;
} MaPLExecutionContext;

uint8_t evaluateChar(MaPLExecutionContext *context);
int32_t evaluateInt32(MaPLExecutionContext *context);
int64_t evaluateInt64(MaPLExecutionContext *context);
uint32_t evaluateUint32(MaPLExecutionContext *context);
uint64_t evaluateUint64(MaPLExecutionContext *context);
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
MaPLParameter MaPLChar(uint8_t charValue) {
    return (MaPLParameter){ MaPLDataType_char, .charValue = charValue };
}
MaPLParameter MaPLInt32(int32_t int32Value) {
    return (MaPLParameter){ MaPLDataType_int32, .int32Value = int32Value };
}
MaPLParameter MaPLInt64(int64_t int64Value) {
    return (MaPLParameter){ MaPLDataType_int64, .int64Value = int64Value };
}
MaPLParameter MaPLUint32(uint32_t uint32Value) {
    return (MaPLParameter){ MaPLDataType_uint32, .uint32Value = uint32Value };
}
MaPLParameter MaPLUint64(uint64_t uint64Value) {
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

MaPLDataType typeForInstruction(MaPLInstruction instruction) {
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

MaPLInstruction readInstruction(MaPLExecutionContext *context) {
    MaPLInstruction instruction = context->scriptBuffer[context->cursorPosition];
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

MaPLParameterCount readParameterCount(MaPLExecutionContext *context) {
    MaPLParameterCount parameterCount = *((MaPLParameterCount *)(context->scriptBuffer+context->cursorPosition));
    context->cursorPosition += sizeof(MaPLParameterCount);
    return parameterCount;
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
    MaPLParameterCount paramCount = readParameterCount(context);
    
    // This requires an allocation because the size of these arrays is not compile time constant,
    // but they can be reused over the lifetime of the script. This optimization is possible because
    // MaPL doesn't do recursion within a single `MaPLExecutionContext`.
    if (paramCount > context->parameterListCount) {
        context->parameterListCount = paramCount;
        context->parameterList = realloc(context->parameterList, sizeof(MaPLParameter) * paramCount);
        context->taggedStringParameterList = realloc(context->taggedStringParameterList, sizeof(char *) * paramCount);
    }
    
    memset(context->taggedStringParameterList, 0, sizeof(char *) * paramCount);
    for (MaPLParameterCount i = 0; i < paramCount; i++) {
        context->parameterList[i] = evaluateParameter(context);
        if (context->parameterList[i].dataType == MaPLDataType_string) {
            // Untag the string and store the tagged pointer for later release.
            context->taggedStringParameterList[i] = (char *)context->parameterList[i].stringValue;
            context->parameterList[i].stringValue = untagString((char *)context->parameterList[i].stringValue);
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
                                                         context->parameterList,
                                                         paramCount);
    }
    
    // Clean up string params.
    for (MaPLParameterCount i = 0; i < paramCount; i++) {
        freeStringIfNeeded(context->taggedStringParameterList[i]);
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

uint8_t evaluateChar(MaPLExecutionContext *context) {
    switch(readInstruction(context)) {
        case MAPL_INSTRUCTION_CHAR_LITERAL: {
            uint8_t literal = *((uint8_t *)(context->scriptBuffer+context->cursorPosition));
            context->cursorPosition += sizeof(uint8_t);
            return literal;
        }
        case MAPL_INSTRUCTION_CHAR_VARIABLE:
            return *((uint8_t *)(context->primitiveTable+readMemoryAddress(context)));
        case MAPL_INSTRUCTION_CHAR_ADD:
            return evaluateChar(context) + evaluateChar(context);
        case MAPL_INSTRUCTION_CHAR_SUBTRACT:
            return evaluateChar(context) - evaluateChar(context);
        case MAPL_INSTRUCTION_CHAR_DIVIDE:
            return evaluateChar(context) / evaluateChar(context);
        case MAPL_INSTRUCTION_CHAR_MULTIPLY:
            return evaluateChar(context) * evaluateChar(context);
        case MAPL_INSTRUCTION_CHAR_MODULO:
            return evaluateChar(context) % evaluateChar(context);
        case MAPL_INSTRUCTION_CHAR_BITWISE_AND:
            return evaluateChar(context) & evaluateChar(context);
        case MAPL_INSTRUCTION_CHAR_BITWISE_OR:
            return evaluateChar(context) | evaluateChar(context);
        case MAPL_INSTRUCTION_CHAR_BITWISE_XOR:
            return evaluateChar(context) ^ evaluateChar(context);
        case MAPL_INSTRUCTION_CHAR_BITWISE_NEGATION:
            return ~evaluateChar(context);
        case MAPL_INSTRUCTION_CHAR_BITWISE_SHIFT_LEFT:
            return evaluateChar(context) << evaluateChar(context);
        case MAPL_INSTRUCTION_CHAR_BITWISE_SHIFT_RIGHT:
            return evaluateChar(context) >> evaluateChar(context);
        case MAPL_INSTRUCTION_CHAR_FUNCTION_INVOCATION: {
            MaPLParameter returnedValue = evaluateFunctionInvocation(context);
            return verifyReturnValue(context, &returnedValue, MaPLDataType_char) ? returnedValue.charValue : 0;
        }
        case MAPL_INSTRUCTION_CHAR_SUBSCRIPT_INVOCATION: {
            MaPLParameter returnedValue = evaluateSubscriptInvocation(context);
            return verifyReturnValue(context, &returnedValue, MaPLDataType_char) ? returnedValue.charValue : 0;
        }
        case MAPL_INSTRUCTION_CHAR_TERNARY_CONDITIONAL: {
            bool previousDeadCodepath = context->isDeadCodepath;
            uint8_t result;
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
        case MAPL_INSTRUCTION_CHAR_TYPECAST:
            switch (typeForInstruction(context->scriptBuffer[context->cursorPosition])) {
                case MaPLDataType_int32:
                    return (uint8_t)evaluateInt32(context);
                case MaPLDataType_int64:
                    return (uint8_t)evaluateInt64(context);
                case MaPLDataType_uint32:
                    return (uint8_t)evaluateUint32(context);
                case MaPLDataType_uint64:
                    return (uint8_t)evaluateUint64(context);
                case MaPLDataType_float32:
                    return (uint8_t)evaluateFloat32(context);
                case MaPLDataType_float64:
                    return (uint8_t)evaluateFloat64(context);
                case MaPLDataType_string: {
                    const char *taggedString = evaluateString(context);
                    uint8_t returnChar = (uint8_t)atoi(untagString(taggedString));
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
        case MAPL_INSTRUCTION_INT32_LITERAL: {
            int32_t literal = *((int32_t *)(context->scriptBuffer+context->cursorPosition));
            context->cursorPosition += sizeof(int32_t);
            return literal;
        }
        case MAPL_INSTRUCTION_INT32_VARIABLE:
            return *((int32_t *)(context->primitiveTable+readMemoryAddress(context)));
        case MAPL_INSTRUCTION_INT32_ADD:
            return evaluateInt32(context) + evaluateInt32(context);
        case MAPL_INSTRUCTION_INT32_SUBTRACT:
            return evaluateInt32(context) - evaluateInt32(context);
        case MAPL_INSTRUCTION_INT32_DIVIDE:
            return evaluateInt32(context) / evaluateInt32(context);
        case MAPL_INSTRUCTION_INT32_MULTIPLY:
            return evaluateInt32(context) * evaluateInt32(context);
        case MAPL_INSTRUCTION_INT32_MODULO:
            return evaluateInt32(context) % evaluateInt32(context);
        case MAPL_INSTRUCTION_INT32_NUMERIC_NEGATION:
            return -evaluateInt32(context);
        case MAPL_INSTRUCTION_INT32_BITWISE_AND:
            return evaluateInt32(context) & evaluateInt32(context);
        case MAPL_INSTRUCTION_INT32_BITWISE_OR:
            return evaluateInt32(context) | evaluateInt32(context);
        case MAPL_INSTRUCTION_INT32_BITWISE_XOR:
            return evaluateInt32(context) ^ evaluateInt32(context);
        case MAPL_INSTRUCTION_INT32_BITWISE_NEGATION:
            return ~evaluateInt32(context);
        case MAPL_INSTRUCTION_INT32_BITWISE_SHIFT_LEFT:
            return evaluateInt32(context) << evaluateInt32(context);
        case MAPL_INSTRUCTION_INT32_BITWISE_SHIFT_RIGHT:
            return evaluateInt32(context) >> evaluateInt32(context);
        case MAPL_INSTRUCTION_INT32_FUNCTION_INVOCATION: {
            MaPLParameter returnedValue = evaluateFunctionInvocation(context);
            return verifyReturnValue(context, &returnedValue, MaPLDataType_int32) ? returnedValue.int32Value : 0;
        }
        case MAPL_INSTRUCTION_INT32_SUBSCRIPT_INVOCATION: {
            MaPLParameter returnedValue = evaluateSubscriptInvocation(context);
            return verifyReturnValue(context, &returnedValue, MaPLDataType_int32) ? returnedValue.int32Value : 0;
        }
        case MAPL_INSTRUCTION_INT32_TERNARY_CONDITIONAL: {
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
        case MAPL_INSTRUCTION_INT32_TYPECAST:
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
        case MAPL_INSTRUCTION_INT64_LITERAL: {
            int64_t literal = *((int64_t *)(context->scriptBuffer+context->cursorPosition));
            context->cursorPosition += sizeof(int64_t);
            return literal;
        }
        case MAPL_INSTRUCTION_INT64_VARIABLE:
            return *((int64_t *)(context->primitiveTable+readMemoryAddress(context)));
        case MAPL_INSTRUCTION_INT64_ADD:
            return evaluateInt64(context) + evaluateInt64(context);
        case MAPL_INSTRUCTION_INT64_SUBTRACT:
            return evaluateInt64(context) - evaluateInt64(context);
        case MAPL_INSTRUCTION_INT64_DIVIDE:
            return evaluateInt64(context) / evaluateInt64(context);
        case MAPL_INSTRUCTION_INT64_MULTIPLY:
            return evaluateInt64(context) * evaluateInt64(context);
        case MAPL_INSTRUCTION_INT64_MODULO:
            return evaluateInt64(context) % evaluateInt64(context);
        case MAPL_INSTRUCTION_INT64_NUMERIC_NEGATION:
            return -evaluateInt64(context);
        case MAPL_INSTRUCTION_INT64_BITWISE_AND:
            return evaluateInt64(context) & evaluateInt64(context);
        case MAPL_INSTRUCTION_INT64_BITWISE_OR:
            return evaluateInt64(context) | evaluateInt64(context);
        case MAPL_INSTRUCTION_INT64_BITWISE_XOR:
            return evaluateInt64(context) ^ evaluateInt64(context);
        case MAPL_INSTRUCTION_INT64_BITWISE_NEGATION:
            return ~evaluateInt64(context);
        case MAPL_INSTRUCTION_INT64_BITWISE_SHIFT_LEFT:
            return evaluateInt64(context) << evaluateInt64(context);
        case MAPL_INSTRUCTION_INT64_BITWISE_SHIFT_RIGHT:
            return evaluateInt64(context) >> evaluateInt64(context);
        case MAPL_INSTRUCTION_INT64_FUNCTION_INVOCATION: {
            MaPLParameter returnedValue = evaluateFunctionInvocation(context);
            return verifyReturnValue(context, &returnedValue, MaPLDataType_int64) ? returnedValue.int64Value : 0;
        }
        case MAPL_INSTRUCTION_INT64_SUBSCRIPT_INVOCATION: {
            MaPLParameter returnedValue = evaluateSubscriptInvocation(context);
            return verifyReturnValue(context, &returnedValue, MaPLDataType_int64) ? returnedValue.int64Value : 0;
        }
        case MAPL_INSTRUCTION_INT64_TERNARY_CONDITIONAL: {
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
        case MAPL_INSTRUCTION_INT64_TYPECAST:
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

uint32_t evaluateUint32(MaPLExecutionContext *context) {
    switch(readInstruction(context)) {
        case MAPL_INSTRUCTION_UINT32_LITERAL: {
            uint32_t literal = *((uint32_t *)(context->scriptBuffer+context->cursorPosition));
            context->cursorPosition += sizeof(uint32_t);
            return literal;
        }
        case MAPL_INSTRUCTION_UINT32_VARIABLE:
            return *((uint32_t *)(context->primitiveTable+readMemoryAddress(context)));
        case MAPL_INSTRUCTION_UINT32_ADD:
            return evaluateUint32(context) + evaluateUint32(context);
        case MAPL_INSTRUCTION_UINT32_SUBTRACT:
            return evaluateUint32(context) - evaluateUint32(context);
        case MAPL_INSTRUCTION_UINT32_DIVIDE:
            return evaluateUint32(context) / evaluateUint32(context);
        case MAPL_INSTRUCTION_UINT32_MULTIPLY:
            return evaluateUint32(context) * evaluateUint32(context);
        case MAPL_INSTRUCTION_UINT32_MODULO:
            return evaluateUint32(context) % evaluateUint32(context);
        case MAPL_INSTRUCTION_UINT32_BITWISE_AND:
            return evaluateUint32(context) & evaluateUint32(context);
        case MAPL_INSTRUCTION_UINT32_BITWISE_OR:
            return evaluateUint32(context) | evaluateUint32(context);
        case MAPL_INSTRUCTION_UINT32_BITWISE_XOR:
            return evaluateUint32(context) ^ evaluateUint32(context);
        case MAPL_INSTRUCTION_UINT32_BITWISE_NEGATION:
            return ~evaluateUint32(context);
        case MAPL_INSTRUCTION_UINT32_BITWISE_SHIFT_LEFT:
            return evaluateUint32(context) << evaluateUint32(context);
        case MAPL_INSTRUCTION_UINT32_BITWISE_SHIFT_RIGHT:
            return evaluateUint32(context) >> evaluateUint32(context);
        case MAPL_INSTRUCTION_UINT32_FUNCTION_INVOCATION: {
            MaPLParameter returnedValue = evaluateFunctionInvocation(context);
            return verifyReturnValue(context, &returnedValue, MaPLDataType_uint32) ? returnedValue.uint32Value : 0;
        }
        case MAPL_INSTRUCTION_UINT32_SUBSCRIPT_INVOCATION: {
            MaPLParameter returnedValue = evaluateSubscriptInvocation(context);
            return verifyReturnValue(context, &returnedValue, MaPLDataType_uint32) ? returnedValue.uint32Value : 0;
        }
        case MAPL_INSTRUCTION_UINT32_TERNARY_CONDITIONAL: {
            bool previousDeadCodepath = context->isDeadCodepath;
            uint32_t result;
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
        case MAPL_INSTRUCTION_UINT32_TYPECAST:
            switch (typeForInstruction(context->scriptBuffer[context->cursorPosition])) {
                case MaPLDataType_char:
                    return (uint32_t)evaluateChar(context);
                case MaPLDataType_int32:
                    return (uint32_t)evaluateInt32(context);
                case MaPLDataType_int64:
                    return (uint32_t)evaluateInt64(context);
                case MaPLDataType_uint64:
                    return (uint32_t)evaluateUint64(context);
                case MaPLDataType_float32:
                    return (uint32_t)evaluateFloat32(context);
                case MaPLDataType_float64:
                    return (uint32_t)evaluateFloat64(context);
                case MaPLDataType_string: {
                    const char *taggedString = evaluateString(context);
                    uint32_t returnInt = (uint32_t)atol(untagString(taggedString));
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

uint64_t evaluateUint64(MaPLExecutionContext *context) {
    switch(readInstruction(context)) {
        case MAPL_INSTRUCTION_UINT64_LITERAL: {
            uint64_t literal = *((uint64_t *)(context->scriptBuffer+context->cursorPosition));
            context->cursorPosition += sizeof(uint64_t);
            return literal;
        }
        case MAPL_INSTRUCTION_UINT64_VARIABLE:
            return *((uint64_t *)(context->primitiveTable+readMemoryAddress(context)));
        case MAPL_INSTRUCTION_UINT64_ADD:
            return evaluateUint64(context) + evaluateUint64(context);
        case MAPL_INSTRUCTION_UINT64_SUBTRACT:
            return evaluateUint64(context) - evaluateUint64(context);
        case MAPL_INSTRUCTION_UINT64_DIVIDE:
            return evaluateUint64(context) / evaluateUint64(context);
        case MAPL_INSTRUCTION_UINT64_MULTIPLY:
            return evaluateUint64(context) * evaluateUint64(context);
        case MAPL_INSTRUCTION_UINT64_MODULO:
            return evaluateUint64(context) % evaluateUint64(context);
        case MAPL_INSTRUCTION_UINT64_BITWISE_AND:
            return evaluateUint64(context) & evaluateUint64(context);
        case MAPL_INSTRUCTION_UINT64_BITWISE_OR:
            return evaluateUint64(context) | evaluateUint64(context);
        case MAPL_INSTRUCTION_UINT64_BITWISE_XOR:
            return evaluateUint64(context) ^ evaluateUint64(context);
        case MAPL_INSTRUCTION_UINT64_BITWISE_NEGATION:
            return ~evaluateUint64(context);
        case MAPL_INSTRUCTION_UINT64_BITWISE_SHIFT_LEFT:
            return evaluateUint64(context) << evaluateUint64(context);
        case MAPL_INSTRUCTION_UINT64_BITWISE_SHIFT_RIGHT:
            return evaluateUint64(context) >> evaluateUint64(context);
        case MAPL_INSTRUCTION_UINT64_FUNCTION_INVOCATION: {
            MaPLParameter returnedValue = evaluateFunctionInvocation(context);
            return verifyReturnValue(context, &returnedValue, MaPLDataType_uint64) ? returnedValue.uint64Value : 0;
        }
        case MAPL_INSTRUCTION_UINT64_SUBSCRIPT_INVOCATION: {
            MaPLParameter returnedValue = evaluateSubscriptInvocation(context);
            return verifyReturnValue(context, &returnedValue, MaPLDataType_uint64) ? returnedValue.uint64Value : 0;
        }
        case MAPL_INSTRUCTION_UINT64_TERNARY_CONDITIONAL: {
            bool previousDeadCodepath = context->isDeadCodepath;
            uint64_t result;
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
        case MAPL_INSTRUCTION_UINT64_TYPECAST:
            switch (typeForInstruction(context->scriptBuffer[context->cursorPosition])) {
                case MaPLDataType_char:
                    return (uint64_t)evaluateChar(context);
                case MaPLDataType_int32:
                    return (uint64_t)evaluateInt32(context);
                case MaPLDataType_int64:
                    return (uint64_t)evaluateInt64(context);
                case MaPLDataType_uint32:
                    return (uint64_t)evaluateUint32(context);
                case MaPLDataType_float32:
                    return (uint64_t)evaluateFloat32(context);
                case MaPLDataType_float64:
                    return (uint64_t)evaluateFloat64(context);
                case MaPLDataType_string: {
                    const char *taggedString = evaluateString(context);
                    uint64_t returnInt = (uint64_t)atol(untagString(taggedString));
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
        case MAPL_INSTRUCTION_FLOAT32_LITERAL: {
            float literal = *((float *)(context->scriptBuffer+context->cursorPosition));
            context->cursorPosition += sizeof(float);
            return literal;
        }
        case MAPL_INSTRUCTION_FLOAT32_VARIABLE:
            return *((float *)(context->primitiveTable+readMemoryAddress(context)));
        case MAPL_INSTRUCTION_FLOAT32_ADD:
            return evaluateFloat32(context) + evaluateFloat32(context);
        case MAPL_INSTRUCTION_FLOAT32_SUBTRACT:
            return evaluateFloat32(context) - evaluateFloat32(context);
        case MAPL_INSTRUCTION_FLOAT32_DIVIDE:
            return evaluateFloat32(context) / evaluateFloat32(context);
        case MAPL_INSTRUCTION_FLOAT32_MULTIPLY:
            return evaluateFloat32(context) * evaluateFloat32(context);
        case MAPL_INSTRUCTION_FLOAT32_MODULO: {
            float f1 = evaluateFloat32(context);
            float f2 = evaluateFloat32(context);
            return fmodf(f1, f2);
        }
        case MAPL_INSTRUCTION_FLOAT32_NUMERIC_NEGATION:
            return -evaluateFloat32(context);
        case MAPL_INSTRUCTION_FLOAT32_FUNCTION_INVOCATION: {
            MaPLParameter returnedValue = evaluateFunctionInvocation(context);
            return verifyReturnValue(context, &returnedValue, MaPLDataType_float32) ? returnedValue.float32Value : 0.0f;
        }
        case MAPL_INSTRUCTION_FLOAT32_SUBSCRIPT_INVOCATION: {
            MaPLParameter returnedValue = evaluateSubscriptInvocation(context);
            return verifyReturnValue(context, &returnedValue, MaPLDataType_float32) ? returnedValue.float32Value : 0.0f;
        }
        case MAPL_INSTRUCTION_FLOAT32_TERNARY_CONDITIONAL: {
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
        case MAPL_INSTRUCTION_FLOAT32_TYPECAST:
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
        case MAPL_INSTRUCTION_FLOAT64_LITERAL: {
            double literal = *((double *)(context->scriptBuffer+context->cursorPosition));
            context->cursorPosition += sizeof(double);
            return literal;
        }
        case MAPL_INSTRUCTION_FLOAT64_VARIABLE:
            return *((double *)(context->primitiveTable+readMemoryAddress(context)));
        case MAPL_INSTRUCTION_FLOAT64_ADD:
            return evaluateFloat64(context) + evaluateFloat64(context);
        case MAPL_INSTRUCTION_FLOAT64_SUBTRACT:
            return evaluateFloat64(context) - evaluateFloat64(context);
        case MAPL_INSTRUCTION_FLOAT64_DIVIDE:
            return evaluateFloat64(context) / evaluateFloat64(context);
        case MAPL_INSTRUCTION_FLOAT64_MULTIPLY:
            return evaluateFloat64(context) * evaluateFloat64(context);
        case MAPL_INSTRUCTION_FLOAT64_MODULO: {
            double d1 = evaluateFloat64(context);
            double d2 = evaluateFloat64(context);
            return fmod(d1, d2);
        }
        case MAPL_INSTRUCTION_FLOAT64_NUMERIC_NEGATION:
            return -evaluateFloat64(context);
        case MAPL_INSTRUCTION_FLOAT64_FUNCTION_INVOCATION: {
            MaPLParameter returnedValue = evaluateFunctionInvocation(context);
            return verifyReturnValue(context, &returnedValue, MaPLDataType_float64) ? returnedValue.float64Value : 0.0;
        }
        case MAPL_INSTRUCTION_FLOAT64_SUBSCRIPT_INVOCATION: {
            MaPLParameter returnedValue = evaluateSubscriptInvocation(context);
            return verifyReturnValue(context, &returnedValue, MaPLDataType_float64) ? returnedValue.float64Value : 0.0;
        }
        case MAPL_INSTRUCTION_FLOAT64_TERNARY_CONDITIONAL: {
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
        case MAPL_INSTRUCTION_FLOAT64_TYPECAST:
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
        case MAPL_INSTRUCTION_LITERAL_TRUE:
            return true;
        case MAPL_INSTRUCTION_LITERAL_FALSE:
            return false;
        case MAPL_INSTRUCTION_BOOLEAN_VARIABLE: {
            uint8_t variableValue = *((uint8_t *)(context->primitiveTable+readMemoryAddress(context)));
            return variableValue != 0;
        }
        case MAPL_INSTRUCTION_BOOLEAN_FUNCTION_INVOCATION: {
            MaPLParameter returnedValue = evaluateFunctionInvocation(context);
            return verifyReturnValue(context, &returnedValue, MaPLDataType_boolean) ? returnedValue.booleanValue : false;
        }
        case MAPL_INSTRUCTION_BOOLEAN_SUBSCRIPT_INVOCATION: {
            MaPLParameter returnedValue = evaluateSubscriptInvocation(context);
            return verifyReturnValue(context, &returnedValue, MaPLDataType_boolean) ? returnedValue.booleanValue : false;
        }
        case MAPL_INSTRUCTION_BOOLEAN_TERNARY_CONDITIONAL: {
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
        case MAPL_INSTRUCTION_BOOLEAN_TYPECAST:
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
        case MAPL_INSTRUCTION_LOGICAL_EQUALITY_CHAR:
            return evaluateChar(context) == evaluateChar(context);
        case MAPL_INSTRUCTION_LOGICAL_EQUALITY_INT32:
            return evaluateInt32(context) == evaluateInt32(context);
        case MAPL_INSTRUCTION_LOGICAL_EQUALITY_INT64:
            return evaluateInt64(context) == evaluateInt64(context);
        case MAPL_INSTRUCTION_LOGICAL_EQUALITY_UINT32:
            return evaluateUint32(context) == evaluateUint32(context);
        case MAPL_INSTRUCTION_LOGICAL_EQUALITY_UINT64:
            return evaluateUint64(context) == evaluateUint64(context);
        case MAPL_INSTRUCTION_LOGICAL_EQUALITY_FLOAT32:
            return evaluateFloat32(context) == evaluateFloat32(context);
        case MAPL_INSTRUCTION_LOGICAL_EQUALITY_FLOAT64:
            return evaluateFloat64(context) == evaluateFloat64(context);
        case MAPL_INSTRUCTION_LOGICAL_EQUALITY_BOOLEAN:
            return evaluateBool(context) == evaluateBool(context);
        case MAPL_INSTRUCTION_LOGICAL_EQUALITY_STRING: {
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
        case MAPL_INSTRUCTION_LOGICAL_EQUALITY_POINTER:
            return evaluatePointer(context) == evaluatePointer(context);
        case MAPL_INSTRUCTION_LOGICAL_INEQUALITY_CHAR:
            return evaluateChar(context) != evaluateChar(context);
        case MAPL_INSTRUCTION_LOGICAL_INEQUALITY_INT32:
            return evaluateInt32(context) != evaluateInt32(context);
        case MAPL_INSTRUCTION_LOGICAL_INEQUALITY_INT64:
            return evaluateInt64(context) != evaluateInt64(context);
        case MAPL_INSTRUCTION_LOGICAL_INEQUALITY_UINT32:
            return evaluateUint32(context) != evaluateUint32(context);
        case MAPL_INSTRUCTION_LOGICAL_INEQUALITY_UINT64:
            return evaluateUint64(context) != evaluateUint64(context);
        case MAPL_INSTRUCTION_LOGICAL_INEQUALITY_FLOAT32:
            return evaluateFloat32(context) != evaluateFloat32(context);
        case MAPL_INSTRUCTION_LOGICAL_INEQUALITY_FLOAT64:
            return evaluateFloat64(context) != evaluateFloat64(context);
        case MAPL_INSTRUCTION_LOGICAL_INEQUALITY_BOOLEAN:
            return evaluateBool(context) != evaluateBool(context);
        case MAPL_INSTRUCTION_LOGICAL_INEQUALITY_STRING: {
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
        case MAPL_INSTRUCTION_LOGICAL_INEQUALITY_POINTER:
            return evaluatePointer(context) != evaluatePointer(context);
        case MAPL_INSTRUCTION_LOGICAL_LESS_THAN_CHAR:
            return evaluateChar(context) < evaluateChar(context);
        case MAPL_INSTRUCTION_LOGICAL_LESS_THAN_INT32:
            return evaluateInt32(context) < evaluateInt32(context);
        case MAPL_INSTRUCTION_LOGICAL_LESS_THAN_INT64:
            return evaluateInt64(context) < evaluateInt64(context);
        case MAPL_INSTRUCTION_LOGICAL_LESS_THAN_UINT32:
            return evaluateUint32(context) < evaluateUint32(context);
        case MAPL_INSTRUCTION_LOGICAL_LESS_THAN_UINT64:
            return evaluateUint64(context) < evaluateUint64(context);
        case MAPL_INSTRUCTION_LOGICAL_LESS_THAN_FLOAT32:
            return evaluateFloat32(context) < evaluateFloat32(context);
        case MAPL_INSTRUCTION_LOGICAL_LESS_THAN_FLOAT64:
            return evaluateFloat64(context) < evaluateFloat64(context);
        case MAPL_INSTRUCTION_LOGICAL_LESS_THAN_EQUAL_CHAR:
            return evaluateChar(context) <= evaluateChar(context);
        case MAPL_INSTRUCTION_LOGICAL_LESS_THAN_EQUAL_INT32:
            return evaluateInt32(context) <= evaluateInt32(context);
        case MAPL_INSTRUCTION_LOGICAL_LESS_THAN_EQUAL_INT64:
            return evaluateInt64(context) <= evaluateInt64(context);
        case MAPL_INSTRUCTION_LOGICAL_LESS_THAN_EQUAL_UINT32:
            return evaluateUint32(context) <= evaluateUint32(context);
        case MAPL_INSTRUCTION_LOGICAL_LESS_THAN_EQUAL_UINT64:
            return evaluateUint64(context) <= evaluateUint64(context);
        case MAPL_INSTRUCTION_LOGICAL_LESS_THAN_EQUAL_FLOAT32:
            return evaluateFloat32(context) <= evaluateFloat32(context);
        case MAPL_INSTRUCTION_LOGICAL_LESS_THAN_EQUAL_FLOAT64:
            return evaluateFloat64(context) <= evaluateFloat64(context);
        case MAPL_INSTRUCTION_LOGICAL_GREATER_THAN_CHAR:
            return evaluateChar(context) > evaluateChar(context);
        case MAPL_INSTRUCTION_LOGICAL_GREATER_THAN_INT32:
            return evaluateInt32(context) > evaluateInt32(context);
        case MAPL_INSTRUCTION_LOGICAL_GREATER_THAN_INT64:
            return evaluateInt64(context) > evaluateInt64(context);
        case MAPL_INSTRUCTION_LOGICAL_GREATER_THAN_UINT32:
            return evaluateUint32(context) > evaluateUint32(context);
        case MAPL_INSTRUCTION_LOGICAL_GREATER_THAN_UINT64:
            return evaluateUint64(context) > evaluateUint64(context);
        case MAPL_INSTRUCTION_LOGICAL_GREATER_THAN_FLOAT32:
            return evaluateFloat32(context) > evaluateFloat32(context);
        case MAPL_INSTRUCTION_LOGICAL_GREATER_THAN_FLOAT64:
            return evaluateFloat64(context) > evaluateFloat64(context);
        case MAPL_INSTRUCTION_LOGICAL_GREATER_THAN_EQUAL_CHAR:
            return evaluateChar(context) >= evaluateChar(context);
        case MAPL_INSTRUCTION_LOGICAL_GREATER_THAN_EQUAL_INT32:
            return evaluateInt32(context) >= evaluateInt32(context);
        case MAPL_INSTRUCTION_LOGICAL_GREATER_THAN_EQUAL_INT64:
            return evaluateInt64(context) >= evaluateInt64(context);
        case MAPL_INSTRUCTION_LOGICAL_GREATER_THAN_EQUAL_UINT32:
            return evaluateUint32(context) >= evaluateUint32(context);
        case MAPL_INSTRUCTION_LOGICAL_GREATER_THAN_EQUAL_UINT64:
            return evaluateUint64(context) >= evaluateUint64(context);
        case MAPL_INSTRUCTION_LOGICAL_GREATER_THAN_EQUAL_FLOAT32:
            return evaluateFloat32(context) >= evaluateFloat32(context);
        case MAPL_INSTRUCTION_LOGICAL_GREATER_THAN_EQUAL_FLOAT64:
            return evaluateFloat64(context) >= evaluateFloat64(context);
        case MAPL_INSTRUCTION_LOGICAL_AND: {
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
        case MAPL_INSTRUCTION_LOGICAL_OR: {
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
        case MAPL_INSTRUCTION_LOGICAL_NEGATION:
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
        case MAPL_INSTRUCTION_LITERAL_NULL:
            return NULL;
        case MAPL_INSTRUCTION_POINTER_VARIABLE:
            return *((void **)(context->primitiveTable+readMemoryAddress(context)));
        case MAPL_INSTRUCTION_POINTER_NULL_COALESCING: {
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
        case MAPL_INSTRUCTION_POINTER_FUNCTION_INVOCATION: {
            MaPLParameter returnedValue = evaluateFunctionInvocation(context);
            return verifyReturnValue(context, &returnedValue, MaPLDataType_pointer) ? returnedValue.pointerValue : NULL;
        }
        case MAPL_INSTRUCTION_POINTER_SUBSCRIPT_INVOCATION: {
            MaPLParameter returnedValue = evaluateSubscriptInvocation(context);
            return verifyReturnValue(context, &returnedValue, MaPLDataType_pointer) ? returnedValue.pointerValue : NULL;
        }
        case MAPL_INSTRUCTION_POINTER_TERNARY_CONDITIONAL: {
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
        case MAPL_INSTRUCTION_STRING_LITERAL: {
            return readString(context);
        }
        case MAPL_INSTRUCTION_STRING_VARIABLE:
            return tagStringAsNotAllocated(context->stringTable[readMemoryAddress(context)]);
        case MAPL_INSTRUCTION_STRING_CONCAT: {
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
        case MAPL_INSTRUCTION_STRING_FUNCTION_INVOCATION: {
            MaPLParameter returnedValue = evaluateFunctionInvocation(context);
            return verifyReturnValue(context, &returnedValue, MaPLDataType_string) ? (char *)returnedValue.stringValue : NULL;
        }
        case MAPL_INSTRUCTION_STRING_SUBSCRIPT_INVOCATION: {
            MaPLParameter returnedValue = evaluateSubscriptInvocation(context);
            return verifyReturnValue(context, &returnedValue, MaPLDataType_string) ? (char *)returnedValue.stringValue : NULL;
        }
        case MAPL_INSTRUCTION_STRING_TERNARY_CONDITIONAL: {
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
        case MAPL_INSTRUCTION_STRING_TYPECAST: {
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

MaPLParameter applyOperatorAssign(MaPLExecutionContext *context, MaPLInstruction operatorAssignInstruction, MaPLParameter *initialValue, MaPLParameter *incrementValue) {
    MaPLParameter result = MaPLUninitialized();
    if (verifyReturnValue(context, initialValue, incrementValue->dataType)) {
        result.dataType = typeForInstruction(operatorAssignInstruction);
        switch (operatorAssignInstruction) {
            case MAPL_INSTRUCTION_INT32_ADD:
                result.int32Value = initialValue->int32Value + incrementValue->int32Value;
                break;
            case MAPL_INSTRUCTION_INT32_SUBTRACT:
                result.int32Value = initialValue->int32Value - incrementValue->int32Value;
                break;
            case MAPL_INSTRUCTION_INT32_DIVIDE:
                result.int32Value = initialValue->int32Value / incrementValue->int32Value;
                break;
            case MAPL_INSTRUCTION_INT32_MULTIPLY:
                result.int32Value = initialValue->int32Value * incrementValue->int32Value;
                break;
            case MAPL_INSTRUCTION_INT32_MODULO:
                result.int32Value = initialValue->int32Value % incrementValue->int32Value;
                break;
            case MAPL_INSTRUCTION_INT32_BITWISE_AND:
                result.int32Value = initialValue->int32Value & incrementValue->int32Value;
                break;
            case MAPL_INSTRUCTION_INT32_BITWISE_OR:
                result.int32Value = initialValue->int32Value | incrementValue->int32Value;
                break;
            case MAPL_INSTRUCTION_INT32_BITWISE_XOR:
                result.int32Value = initialValue->int32Value ^ incrementValue->int32Value;
                break;
            case MAPL_INSTRUCTION_INT32_BITWISE_SHIFT_LEFT:
                result.int32Value = initialValue->int32Value << incrementValue->int32Value;
                break;
            case MAPL_INSTRUCTION_INT32_BITWISE_SHIFT_RIGHT:
                result.int32Value = initialValue->int32Value >> incrementValue->int32Value;
                break;
            case MAPL_INSTRUCTION_FLOAT32_ADD:
                result.float32Value = initialValue->float32Value + incrementValue->float32Value;
                break;
            case MAPL_INSTRUCTION_FLOAT32_SUBTRACT:
                result.float32Value = initialValue->float32Value - incrementValue->float32Value;
                break;
            case MAPL_INSTRUCTION_FLOAT32_DIVIDE:
                result.float32Value = initialValue->float32Value / incrementValue->float32Value;
                break;
            case MAPL_INSTRUCTION_FLOAT32_MULTIPLY:
                result.float32Value = initialValue->float32Value * incrementValue->float32Value;
                break;
            case MAPL_INSTRUCTION_FLOAT32_MODULO:
                result.float32Value = fmodf(initialValue->float32Value, incrementValue->float32Value);
                break;
            case MAPL_INSTRUCTION_STRING_CONCAT:
                result.stringValue = concatenateStrings((char *)initialValue->stringValue, (char *)incrementValue->stringValue);
                break;
            case MAPL_INSTRUCTION_INT64_ADD:
                result.int64Value = initialValue->int64Value + incrementValue->int64Value;
                break;
            case MAPL_INSTRUCTION_INT64_SUBTRACT:
                result.int64Value = initialValue->int64Value - incrementValue->int64Value;
                break;
            case MAPL_INSTRUCTION_INT64_DIVIDE:
                result.int64Value = initialValue->int64Value / incrementValue->int64Value;
                break;
            case MAPL_INSTRUCTION_INT64_MULTIPLY:
                result.int64Value = initialValue->int64Value * incrementValue->int64Value;
                break;
            case MAPL_INSTRUCTION_INT64_MODULO:
                result.int64Value = initialValue->int64Value % incrementValue->int64Value;
                break;
            case MAPL_INSTRUCTION_INT64_BITWISE_AND:
                result.int64Value = initialValue->int64Value & incrementValue->int64Value;
                break;
            case MAPL_INSTRUCTION_INT64_BITWISE_OR:
                result.int64Value = initialValue->int64Value | incrementValue->int64Value;
                break;
            case MAPL_INSTRUCTION_INT64_BITWISE_XOR:
                result.int64Value = initialValue->int64Value ^ incrementValue->int64Value;
                break;
            case MAPL_INSTRUCTION_INT64_BITWISE_SHIFT_LEFT:
                result.int64Value = initialValue->int64Value << incrementValue->int64Value;
                break;
            case MAPL_INSTRUCTION_INT64_BITWISE_SHIFT_RIGHT:
                result.int64Value = initialValue->int64Value >> incrementValue->int64Value;
                break;
            case MAPL_INSTRUCTION_FLOAT64_ADD:
                result.float64Value = initialValue->float64Value + incrementValue->float64Value;
                break;
            case MAPL_INSTRUCTION_FLOAT64_SUBTRACT:
                result.float64Value = initialValue->float64Value - incrementValue->float64Value;
                break;
            case MAPL_INSTRUCTION_FLOAT64_DIVIDE:
                result.float64Value = initialValue->float64Value / incrementValue->float64Value;
                break;
            case MAPL_INSTRUCTION_FLOAT64_MULTIPLY:
                result.float64Value = initialValue->float64Value * incrementValue->float64Value;
                break;
            case MAPL_INSTRUCTION_FLOAT64_MODULO:
                result.float64Value = fmod(initialValue->float64Value, incrementValue->float64Value);
                break;
            case MAPL_INSTRUCTION_UINT32_ADD:
                result.uint32Value = initialValue->uint32Value + incrementValue->uint32Value;
                break;
            case MAPL_INSTRUCTION_UINT32_SUBTRACT:
                result.uint32Value = initialValue->uint32Value - incrementValue->uint32Value;
                break;
            case MAPL_INSTRUCTION_UINT32_DIVIDE:
                result.uint32Value = initialValue->uint32Value / incrementValue->uint32Value;
                break;
            case MAPL_INSTRUCTION_UINT32_MULTIPLY:
                result.uint32Value = initialValue->uint32Value * incrementValue->uint32Value;
                break;
            case MAPL_INSTRUCTION_UINT32_MODULO:
                result.uint32Value = initialValue->uint32Value % incrementValue->uint32Value;
                break;
            case MAPL_INSTRUCTION_UINT32_BITWISE_AND:
                result.uint32Value = initialValue->uint32Value & incrementValue->uint32Value;
                break;
            case MAPL_INSTRUCTION_UINT32_BITWISE_OR:
                result.uint32Value = initialValue->uint32Value | incrementValue->uint32Value;
                break;
            case MAPL_INSTRUCTION_UINT32_BITWISE_XOR:
                result.uint32Value = initialValue->uint32Value ^ incrementValue->uint32Value;
                break;
            case MAPL_INSTRUCTION_UINT32_BITWISE_SHIFT_LEFT:
                result.uint32Value = initialValue->uint32Value << incrementValue->uint32Value;
                break;
            case MAPL_INSTRUCTION_UINT32_BITWISE_SHIFT_RIGHT:
                result.uint32Value = initialValue->uint32Value >> incrementValue->uint32Value;
                break;
            case MAPL_INSTRUCTION_UINT64_ADD:
                result.uint64Value = initialValue->uint64Value + incrementValue->uint64Value;
                break;
            case MAPL_INSTRUCTION_UINT64_SUBTRACT:
                result.uint64Value = initialValue->uint64Value - incrementValue->uint64Value;
                break;
            case MAPL_INSTRUCTION_UINT64_DIVIDE:
                result.uint64Value = initialValue->uint64Value / incrementValue->uint64Value;
                break;
            case MAPL_INSTRUCTION_UINT64_MULTIPLY:
                result.uint64Value = initialValue->uint64Value * incrementValue->uint64Value;
                break;
            case MAPL_INSTRUCTION_UINT64_MODULO:
                result.uint64Value = initialValue->uint64Value % incrementValue->uint64Value;
                break;
            case MAPL_INSTRUCTION_UINT64_BITWISE_AND:
                result.uint64Value = initialValue->uint64Value & incrementValue->uint64Value;
                break;
            case MAPL_INSTRUCTION_UINT64_BITWISE_OR:
                result.uint64Value = initialValue->uint64Value | incrementValue->uint64Value;
                break;
            case MAPL_INSTRUCTION_UINT64_BITWISE_XOR:
                result.uint64Value = initialValue->uint64Value ^ incrementValue->uint64Value;
                break;
            case MAPL_INSTRUCTION_UINT64_BITWISE_SHIFT_LEFT:
                result.uint64Value = initialValue->uint64Value << incrementValue->uint64Value;
                break;
            case MAPL_INSTRUCTION_UINT64_BITWISE_SHIFT_RIGHT:
                result.uint64Value = initialValue->uint64Value >> incrementValue->uint64Value;
                break;
            case MAPL_INSTRUCTION_CHAR_ADD:
                result.charValue = initialValue->charValue + incrementValue->charValue;
                break;
            case MAPL_INSTRUCTION_CHAR_SUBTRACT:
                result.charValue = initialValue->charValue - incrementValue->charValue;
                break;
            case MAPL_INSTRUCTION_CHAR_DIVIDE:
                result.charValue = initialValue->charValue / incrementValue->charValue;
                break;
            case MAPL_INSTRUCTION_CHAR_MULTIPLY:
                result.charValue = initialValue->charValue * incrementValue->charValue;
                break;
            case MAPL_INSTRUCTION_CHAR_MODULO:
                result.charValue = initialValue->charValue % incrementValue->charValue;
                break;
            case MAPL_INSTRUCTION_CHAR_BITWISE_AND:
                result.charValue = initialValue->charValue & incrementValue->charValue;
                break;
            case MAPL_INSTRUCTION_CHAR_BITWISE_OR:
                result.charValue = initialValue->charValue | incrementValue->charValue;
                break;
            case MAPL_INSTRUCTION_CHAR_BITWISE_XOR:
                result.charValue = initialValue->charValue ^ incrementValue->charValue;
                break;
            case MAPL_INSTRUCTION_CHAR_BITWISE_SHIFT_LEFT:
                result.charValue = initialValue->charValue << incrementValue->charValue;
                break;
            case MAPL_INSTRUCTION_CHAR_BITWISE_SHIFT_RIGHT:
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
        case MAPL_INSTRUCTION_UNUSED_RETURN_FUNCTION_INVOCATION: {
            MaPLParameter returnedValue = evaluateFunctionInvocation(context);
            if (returnedValue.dataType == MaPLDataType_string) {
                freeStringIfNeeded((char *)returnedValue.stringValue);
            }
        }
            break;
        case MAPL_INSTRUCTION_CHAR_ASSIGN: {
            MaPLMemoryAddress variableAddress = readMemoryAddress(context);
            *(uint8_t *)(context->primitiveTable+variableAddress) = evaluateChar(context);
        }
            break;
        case MAPL_INSTRUCTION_INT32_ASSIGN: {
            MaPLMemoryAddress variableAddress = readMemoryAddress(context);
            *(int32_t *)(context->primitiveTable+variableAddress) = evaluateInt32(context);
        }
            break;
        case MAPL_INSTRUCTION_INT64_ASSIGN: {
            MaPLMemoryAddress variableAddress = readMemoryAddress(context);
            *(int64_t *)(context->primitiveTable+variableAddress) = evaluateInt64(context);
        }
            break;
        case MAPL_INSTRUCTION_UINT32_ASSIGN: {
            MaPLMemoryAddress variableAddress = readMemoryAddress(context);
            *(uint32_t *)(context->primitiveTable+variableAddress) = evaluateUint32(context);
        }
            break;
        case MAPL_INSTRUCTION_UINT64_ASSIGN: {
            MaPLMemoryAddress variableAddress = readMemoryAddress(context);
            *(uint64_t *)(context->primitiveTable+variableAddress) = evaluateUint64(context);
        }
            break;
        case MAPL_INSTRUCTION_FLOAT32_ASSIGN: {
            MaPLMemoryAddress variableAddress = readMemoryAddress(context);
            *(float *)(context->primitiveTable+variableAddress) = evaluateFloat32(context);
        }
            break;
        case MAPL_INSTRUCTION_FLOAT64_ASSIGN: {
            MaPLMemoryAddress variableAddress = readMemoryAddress(context);
            *(double *)(context->primitiveTable+variableAddress) = evaluateFloat64(context);
        }
            break;
        case MAPL_INSTRUCTION_BOOLEAN_ASSIGN: {
            MaPLMemoryAddress variableAddress = readMemoryAddress(context);
            *(uint8_t *)(context->primitiveTable+variableAddress) = (uint8_t)evaluateBool(context);
        }
            break;
        case MAPL_INSTRUCTION_STRING_ASSIGN: {
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
        case MAPL_INSTRUCTION_POINTER_ASSIGN: {
            MaPLMemoryAddress variableAddress = readMemoryAddress(context);
            *(void **)(context->primitiveTable+variableAddress) = evaluatePointer(context);
        }
            break;
        case MAPL_INSTRUCTION_ASSIGN_SUBSCRIPT: {
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
            
            MaPLInstruction operatorAssignInstruction = readInstruction(context);
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
        case MAPL_INSTRUCTION_ASSIGN_PROPERTY: {
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
            MaPLInstruction operatorAssignInstruction = readInstruction(context);
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
        case MAPL_INSTRUCTION_CONDITIONAL: {
            bool conditional = evaluateBool(context);
            MaPLBytecodeLength move = readCursorMove(context);
            if (!conditional) {
                context->cursorPosition += move;
            }
        }
            break;
        case MAPL_INSTRUCTION_CURSOR_MOVE_FORWARD: {
            MaPLBytecodeLength move = readCursorMove(context);
            context->cursorPosition += move;
        }
            break;
        case MAPL_INSTRUCTION_CURSOR_MOVE_BACK: {
            MaPLBytecodeLength move = readCursorMove(context);
            context->cursorPosition -= move;
        }
            break;
        case MAPL_INSTRUCTION_PROGRAM_EXIT:
            context->executionState = MaPLExecutionState_exit;
            break;
        case MAPL_INSTRUCTION_METADATA: {
            MaPLParameterCount paramCount = readParameterCount(context);
            for (MaPLParameterCount i = 0; i < paramCount; i++) {
                const char *metadataString = evaluateString(context);
                if (context->callbacks->metadata) {
                    context->callbacks->metadata(untagString(metadataString));
                }
                freeStringIfNeeded(metadataString);
            }
        }
            break;
        case MAPL_INSTRUCTION_DEBUG_LINE: {
            MaPLLineNumber lineNumber = *((MaPLLineNumber *)(context->scriptBuffer+context->cursorPosition));
            context->cursorPosition += sizeof(MaPLLineNumber);
            if (context->callbacks->debugLine) {
                context->callbacks->debugLine(lineNumber);
            }
        }
            break;
        case MAPL_INSTRUCTION_DEBUG_UPDATE_VARIABLE: {
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
        case MAPL_INSTRUCTION_DEBUG_DELETE_VARIABLE: {
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
    context.scriptBuffer = (uint8_t *)scriptBuffer;
    context.callbacks = callbacks;
    context.isDeadCodepath = false;
    context.executionState = MaPLExecutionState_continue;
    context.parameterList = NULL;
    context.taggedStringParameterList = NULL;
    context.parameterListCount = 0;

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
    uint8_t *allocatedTables = (uint8_t *)malloc(primitiveTableSize + sizeof(char *) * stringTableSize);
    context.primitiveTable = allocatedTables;
    context.stringTable = allocatedTables+primitiveTableSize;
    context.cursorPosition = sizeof(MaPLMemoryAddress)*2+sizeof(uint8_t);
    
    memset(context.stringTable, 0, sizeof(char *) * stringTableSize);
    
    while (context.executionState == MaPLExecutionState_continue && context.cursorPosition < bufferLength) {
        evaluateStatement(&context);
    }
    if (context.executionState == MaPLExecutionState_error && context.callbacks->error) {
        context.callbacks->error(context.errorType);
    }
    
    // Free any remaining allocated strings.
    for(MaPLMemoryAddress i = 0; i < stringTableSize; i++) {
        freeStringIfNeeded(context.stringTable[i]);
    }
    
    free(allocatedTables);
    if (context.parameterListCount) {
        free(context.parameterList);
        free(context.taggedStringParameterList);
    }
}

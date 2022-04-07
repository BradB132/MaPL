//
//  MaPLRuntime.c
//  MaPLRuntime
//
//  Created by Brad Bambara on 4/5/22.
//

#include "MaPLRuntime.h"
#include "MaPLBytecodeConstants.h"
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
    bool skipInvocations;
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

char *incrementReferenceCount(char *taggedString) {
    u_int16_t referenceCount = (uintptr_t)taggedString >> 48;
    if (!referenceCount) {
        // This is an untagged string, don't change it.
        return taggedString;
    }
    referenceCount++;
    return (char *)(((uintptr_t)taggedString & 0x0000FFFFFFFFFFFF) | (uintptr_t)referenceCount << 48);
}

char *decrementReferenceCount(char *taggedString) {
    u_int16_t referenceCount = (uintptr_t)taggedString >> 48;
    if (!referenceCount) {
        // This is an untagged string, don't change it.
        return taggedString;
    }
    referenceCount--;
    taggedString = (char *)(((uintptr_t)taggedString & 0x0000FFFFFFFFFFFF) | (uintptr_t)referenceCount << 48);
    if (!referenceCount) {
        free(taggedString);
        return NULL;
    }
    return taggedString;
}

char *tagString(char *untaggedString) {
    // Initial tag sets the reference count at +1.
    return (char *)((uintptr_t)untaggedString | 0x0001000000000000);
}

char *untagString(char *taggedString) {
    return (char *)((uintptr_t)taggedString & 0x0000FFFFFFFFFFFF);
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
    parameter.stringValue = tagString((char *)parameter.stringValue);
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

// Returned string parameters have a reference count of +1. You need to decrement when done using them.
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
        !context->skipInvocations &&
        context->callbacks->invokeFunction) {
        returnValue = context->callbacks->invokeFunction(invokedOnPointer,
                                                         symbol,
                                                         functionParams,
                                                         paramCount);
    }
    
    // Clean up string params.
    for (MaPLParameterCount i = 0; i < paramCount; i++) {
        decrementReferenceCount(taggedStringParams[i]);
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
        !context->skipInvocations &&
        context->callbacks->invokeSubscript) {
        returnValue = context->callbacks->invokeSubscript(invokedOnPointer, subscriptIndex);
    }
    
    // Clean up string index.
    decrementReferenceCount(taggedIndex);
    
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
                if (!context->skipInvocations) {
                    context->executionState = MaPLExecutionState_error;
                    if (returnedValue.dataType == MaPLDataType_string) {
                        decrementReferenceCount((char *)returnedValue.stringValue);
                    }
                }
                return 0;
            }
            return returnedValue.charValue;
        }
        case MaPLInstruction_char_subscript_invocation: {
            MaPLParameter returnedValue = evaluateSubscriptInvocation(context);
            if (returnedValue.dataType != MaPLDataType_char) {
                if (!context->skipInvocations) {
                    context->executionState = MaPLExecutionState_error;
                    if (returnedValue.dataType == MaPLDataType_string) {
                        decrementReferenceCount((char *)returnedValue.stringValue);
                    }
                }
                return 0;
            }
            return returnedValue.charValue;
        }
        case MaPLInstruction_char_ternary_conditional: {
            bool conditional = evaluateBool(context);
            u_int8_t char1 = evaluateChar(context);
            u_int8_t char2 = evaluateChar(context);
            return conditional ? char1 : char2;
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
                    decrementReferenceCount(taggedString);
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
    return 0; // TODO: Implement this.
}

int64_t evaluateInt64(MaPLExecutionContext *context) {
    return 0; // TODO: Implement this.
}

u_int32_t evaluateUint32(MaPLExecutionContext *context) {
    return 0; // TODO: Implement this.
}

u_int64_t evaluateUint64(MaPLExecutionContext *context) {
    return 0; // TODO: Implement this.
}

float evaluateFloat32(MaPLExecutionContext *context) {
    return 0; // TODO: Implement this.
}

double evaluateFloat64(MaPLExecutionContext *context) {
    return 0; // TODO: Implement this.
}

bool evaluateBool(MaPLExecutionContext *context) {
    return false; // TODO: Implement this.
}

void *evaluatePointer(MaPLExecutionContext *context) {
    return NULL; // TODO: Implement this.
}

// Returned strings have a reference count of +1. You need to decrement when done using them.
char *evaluateString(MaPLExecutionContext *context) {
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
        decrementReferenceCount(stringTable[i]);
    }
}

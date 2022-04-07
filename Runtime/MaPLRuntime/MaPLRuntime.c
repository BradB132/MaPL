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

typedef struct {
    const u_int8_t* scriptBuffer;
    MaPLCursorMove bufferLength;
    MaPLCursorMove cursorPosition;
    u_int8_t *primitiveTable;
    char **stringTable;
    const MaPLCallbacks *callbacks;
} MaPLExecutionContext;

char *tagStringAsAllocated(const char *taggedString) {
    return (char *)((uintptr_t)taggedString | 0x8000000000000000);
}

bool isStringAllocated(const char *taggedString) {
    return ((uintptr_t)taggedString & 0x8000000000000000) != 0;
}

char *untaggedString(const char *taggedString) {
    return (char *)((uintptr_t)taggedString & 0x7FFFFFFFFFFFFFFF);
}

void freeTaggedString(const char *taggedString) {
    if (taggedString && isStringAllocated(taggedString)) {
        free(untaggedString(taggedString));
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
MaPLParameter MaPLPointer(void* pointerValue) {
    MaPLParameter parameter = { MaPLDataType_pointer };
    parameter.pointerValue = pointerValue;
    return parameter;
}
MaPLParameter MaPLStringByReference(char* stringValue) {
    MaPLParameter parameter = { MaPLDataType_string };
    parameter.stringValue = stringValue;
    return parameter;
}
MaPLParameter MaPLStringByValue(char* stringValue) {
    MaPLParameter parameter = { MaPLDataType_string };
    parameter.stringValue = malloc(strlen(stringValue)+1);
    strcpy((char *)parameter.stringValue, stringValue);
    parameter.stringValue = tagStringAsAllocated(parameter.stringValue);
    return parameter;
}

enum MaPLInstruction readInstruction(MaPLExecutionContext *context) {
    enum MaPLInstruction instruction = context->scriptBuffer[context->cursorPosition];
    context->cursorPosition++;
    return instruction;
}

const char *readString(MaPLExecutionContext *context) {
    const char *string = (const char *)(context->scriptBuffer+context->cursorPosition);
    context->cursorPosition += strlen(string)+1;
    return string;
}

bool evaluateStatement(MaPLExecutionContext *context) {
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
            return false;
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
            if (context->callbacks->error) {
                context->callbacks->error("Encountered invalid bytecode instruction");
            }
            return false;
    }
    return true;
}

void executeMaPLScript(const void* scriptBuffer, u_int16_t bufferLength, const MaPLCallbacks *callbacks) {
    MaPLExecutionContext context;
    context.scriptBuffer = (u_int8_t *)scriptBuffer;
    context.bufferLength = bufferLength;
    context.callbacks = callbacks;
    
    // The first bytes are always two instances of MaPLMemoryAddress that describe the table sizes.
    // The entire script execution happens synchronously inside this function, so these tables can be stack allocated.
    MaPLMemoryAddress primitiveTableSize = *((MaPLMemoryAddress *)context.scriptBuffer);
    MaPLMemoryAddress stringTableSize = *((MaPLMemoryAddress *)context.scriptBuffer+sizeof(MaPLMemoryAddress));
    u_int8_t primitiveTable[primitiveTableSize];
    char *stringTable[stringTableSize];
    memset(stringTable, 0, sizeof(char *)*stringTableSize);
    context.primitiveTable = primitiveTable;
    context.stringTable = stringTable;
    context.cursorPosition = sizeof(MaPLMemoryAddress)*2;
    
    // TODO: Execute script.
    // TODO: Here and in compiler assert the byte sizes of all types ( https://stackoverflow.com/a/18511691 ).
    
    // Free any remaining allocated strings.
    for(MaPLMemoryAddress i = 0; i < stringTableSize; i++) {
        freeTaggedString(stringTable[i]);
    }
}

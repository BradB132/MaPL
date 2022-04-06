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
MaPLParameter MaPLFloat32(float_t float32Value) {
    MaPLParameter parameter = { MaPLDataType_float32 };
    parameter.float32Value = float32Value;
    return parameter;
}
MaPLParameter MaPLFloat64(double_t float64Value) {
    MaPLParameter parameter = { MaPLDataType_float64 };
    parameter.float64Value = float64Value;
    return parameter;
}
MaPLParameter MaPLBool(u_int8_t booleanValue) {
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
    parameter.stringValue = stringValue;// TODO: Pointer tagging.
    return parameter;
}
MaPLParameter MaPLStringByValue(char* stringValue) {
    MaPLParameter parameter = { MaPLDataType_string };
    parameter.stringValue = (char *)malloc(strlen(stringValue)+1);
    strcpy(parameter.stringValue, stringValue);// TODO: Pointer tagging.
    return parameter;
}

void executeMaPLScript(const void* scriptBuffer, u_int16_t bufferLength, const MaPLCallbacks *callbacks) {
    // TODO: Execute script.
}

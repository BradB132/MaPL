//
//  MaPLRuntime.h
//  MaPLRuntime
//
//  Created by Brad Bambara on 4/5/22.
//

#ifndef MaPLRuntime_h
#define MaPLRuntime_h

#include <stdio.h>
#include <stdbool.h>

#include "MaPLTypedefs.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Lists all possible data types that can exist within a MaPLParameter.
 */
typedef enum {
    MaPLDataType_uninitialized,
    MaPLDataType_void,
    MaPLDataType_char,
    MaPLDataType_int32,
    MaPLDataType_int64,
    MaPLDataType_uint32,
    MaPLDataType_uint64,
    MaPLDataType_float32,
    MaPLDataType_float64,
    MaPLDataType_string,
    MaPLDataType_boolean,
    MaPLDataType_pointer,
} MaPLDataType;

/**
 * Represents a single value passed to or from a MaPL script.
 */
typedef struct {
    MaPLDataType dataType;
    union
    {
        u_int8_t charValue;
        int32_t int32Value;
        int64_t int64Value;
        u_int32_t uint32Value;
        u_int64_t uint64Value;
        float float32Value;
        double float64Value;
        const char* stringValue;
        bool booleanValue;
        void* pointerValue;
    };
} MaPLParameter;

/**
 * Convenience initializers that construct a MaPLParameter of each MaPL data type.
 */
MaPLParameter MaPLUninitialized(void);
MaPLParameter MaPLVoid(void);
MaPLParameter MaPLChar(u_int8_t charValue);
MaPLParameter MaPLInt32(int32_t int32Value);
MaPLParameter MaPLInt64(int64_t int64Value);
MaPLParameter MaPLUint32(u_int32_t uint32Value);
MaPLParameter MaPLUint64(u_int64_t uint64Value);
MaPLParameter MaPLFloat32(float float32Value);
MaPLParameter MaPLFloat64(double float64Value);
MaPLParameter MaPLBool(bool booleanValue);
/**
 * Only passes the pointer, and does not copy the pointer contents. MaPL will make no attempt to free this object.
 * MaPL may reference this object throughout script execution, so the object referenced by this pointer must not freed
 * before the completion of the MaPL script execution.
 */
MaPLParameter MaPLPointer(void *pointerValue);
/**
 * Only passes the string pointer, and does not copy the string contents. MaPL will make no attempt to free the string.
 * MaPL may reference this string throughout script execution, so the string referenced by this pointer must not freed
 * before the completion of the MaPL script execution.
 */
MaPLParameter MaPLStringByReference(char *stringValue);
/**
 * Makes a copy of the string by invoking malloc, and assumes responsibility for freeing that copied string.
 */
MaPLParameter MaPLStringByValue(char *stringValue);

/**
 * A struct that holds all the function pointers which the MaPL runtime uses to interact with the host program.
 */
typedef struct {
    MaPLParameter (*invokeFunction)(const void *invokedOnPointer, MaPLSymbol functionSymbol, const MaPLParameter *argv, MaPLParameterCount argc);
    MaPLParameter (*invokeSubscript)(const void *invokedOnPointer, MaPLParameter index);
    MaPLParameter (*assignSubscript)(const void *invokedOnPointer, MaPLParameter index, MaPLParameter assignedValue);
    MaPLParameter (*assignProperty)(const void *invokedOnPointer, MaPLSymbol propertySymbol, MaPLParameter assignedValue);
    void (*metadata)(const char* metadataString);
    void (*debugLine)(MaPLLineNumber lineNumber);
    void (*debugVariableUpdate)(const char *variableName, MaPLParameter newValue);
    void (*debugVariableDelete)(const char *variableName);
    void (*error)(void);
} MaPLCallbacks;

/**
 * Executes a compiled MaPL script.
 *
 * @param scriptBuffer The compiled script generated by the MaPL compiler.
 * @param bufferLength The byte length of @c scriptBuffer.
 * @param callbacks A struct of callbacks that indicate which functions the MaPL runtime should invoke during script execution.
 */
void executeMaPLScript(const void* scriptBuffer, u_int16_t bufferLength, const MaPLCallbacks *callbacks);

#ifdef __cplusplus
}
#endif

#endif /* MaPLRuntime_h */

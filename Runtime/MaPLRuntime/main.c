//
//  main.c
//  MaPLRuntime
//
//  Created by Brad Bambara on 4/5/22.
//

#include <stdio.h>
#include "MaPLRuntime.h"

enum MaPLFunction {
    MaPLFunction_GLOBAL_global = 1,
    MaPLFunction_GLOBAL_print_string = 2,
};

static char dummyPointer;

MaPLParameter testInvokeFunction(void *invokedOnPointer, MaPLSymbol functionSymbol, const MaPLParameter *argv, MaPLParameterCount argc) {
    switch (functionSymbol) {
        case MaPLFunction_GLOBAL_global:
            return MaPLPointer(&dummyPointer);
        case MaPLFunction_GLOBAL_print_string:
            printf("Print: %s\n", argv[0].stringValue);
            return MaPLVoid();
        default: return MaPLUninitialized();
    }
}

MaPLParameter testInvokeSubscript(void *invokedOnPointer, MaPLParameter index) {
    if (invokedOnPointer == &dummyPointer) {
        return MaPLStringByReference("World!");
    }
    return MaPLUninitialized();
}

void testAssignProperty(void *invokedOnPointer, MaPLSymbol propertySymbol, MaPLParameter assignedValue) {
    // Unused.
}

void testAssignSubscript(void *invokedOnPointer, MaPLParameter index, MaPLParameter assignedValue) {
    // Unused.
}

void testDebugLine(MaPLLineNumber lineNumber) {
    printf("Debug line: %d\n", lineNumber);
}

void testDebugVariableUpdate(const char *variableName, MaPLParameter newValue) {
    // Assuming the only variable is of string type.
    printf("Variable '%s' assigned value '%s'.\n", variableName, newValue.stringValue);
}

void testDebugVariableDelete(const char *variableName) {
    printf("Variable '%s' left scope.\n", variableName);
}

/// Optional. Error callback is invoked when the script has encountered an error and cannot continue. If callback is NULL, script will fail silently.
void error(MaPLRuntimeError error) {
    printf("MaPL script encountered an error.\n");
}

int main(int argc, const char * argv[]) {
    
    // Convoluted "Hello World!" example:
    /*
    #global readonly Global global;
    #global void print(string printedString);
    #type Global {
        string [string];
    }
    
    string s1 = "Hello";
    string s2 = global[s1];
    
    print(s1+" "+s2);
    */
    uint16_t endianShort = 1;
    uint8_t endianByte = *(uint8_t *)&endianShort;
    unsigned char bytecode[] = {
        endianByte, 0x00, 0x00, 0x02, 0x00, 0xc9, 0x07, 0x00, 0xc0, 0x00, 0x00, 0x1f, 0x48,
        0x65, 0x6c, 0x6c, 0x6f, 0x00, 0xca, 0x73, 0x31, 0x00, 0x20, 0x00, 0x00, 0xc9, 0x08,
        0x00, 0xc0, 0x01, 0x00, 0x23, 0x29, 0xcc, 0x01, 0x00, 0x00, 0x20, 0x00, 0x00,
        0xca, 0x73, 0x32, 0x00, 0x20, 0x01, 0x00, 0xc9, 0x0a, 0x00, 0xb7, 0xcc, 0x02,
        0x00, 0x01, 0x21, 0x21, 0x20, 0x00, 0x00, 0x1f, 0x20, 0x00, 0x20, 0x01, 0x00,
    };
    MaPLCallbacks callbacks = {
        testInvokeFunction,
        testInvokeSubscript,
        testAssignProperty,
        testAssignSubscript,
        NULL,
        testDebugLine,
        testDebugVariableUpdate,
        testDebugVariableDelete,
        error,
    };
    executeMaPLScript(bytecode, sizeof(bytecode), &callbacks);
    
    return 0;
}

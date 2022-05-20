//
//  MaPLHandler.cpp
//  MaPLGenerator
//
//  Created by Brad Bambara on 5/17/22.
//

#include "MaPLHandler.h"

#include <stdlib.h>
#include <string>
#include <unordered_map>
#include <vector>

#include "MaPLCompiler.h"
#include "MaPLSymbols.h"

// One of the MaPL APIs allows it to invoke another script.
// Create a stack of metadata about the scripts that are currently running.
struct MaPLStackFrame {
    std::filesystem::path path;
    std::unordered_map<std::string, MaPLParameter> variables;
    MaPLLineNumber currentLineNumber = 0;
};
static std::vector<MaPLStackFrame> stackFrames;

static MaPLParameter invokeFunction(const void *invokedOnPointer, MaPLSymbol functionSymbol, const MaPLParameter *argv, MaPLParameterCount argc) {
    switch (functionSymbol) { // TODO: Implement cases.
        default: break;
    }
    return MaPLUninitialized();
}

static MaPLParameter invokeSubscript(const void *invokedOnPointer, MaPLParameter index) {
    // TODO: Implement this. Only subscript is in "Array" type.
    return MaPLUninitialized();
}

static void assignProperty(const void *invokedOnPointer, MaPLSymbol propertySymbol, MaPLParameter assignedValue) {
    // No-op. API contains no write-able properties.
}

static void assignSubscript(const void *invokedOnPointer, MaPLParameter index, MaPLParameter assignedValue) {
    // No-op. API contains no write-able subscripts.
}

static void metadata(const char* metadataString) {
    // TODO: Make a regex and replace all occurrences of character names.
}

static void debugLine(MaPLLineNumber lineNumber) {
    stackFrames[stackFrames.size()-1].currentLineNumber = lineNumber;
}

static void debugVariableUpdate(const char *variableName, MaPLParameter newValue) {
    stackFrames[stackFrames.size()-1].variables[variableName] = newValue;
}

static void debugVariableDelete(const char *variableName) {
    stackFrames[stackFrames.size()-1].variables.erase(variableName);
}

static void error(MaPLRuntimeError error) {
    const char *errString;
    switch (error) {
        case MaPLRuntimeError_missingCallback:
            errString = "Missing MaPL callback.";
            break;
        case MaPLRuntimeError_malformedBytecode:
            errString = "Malformed MaPL bytecode.";
            break;
        case MaPLRuntimeError_invocationOnNullPointer:
            errString = "MaPL invocation on NULL pointer.";
            break;
        case MaPLRuntimeError_returnValueTypeMismatch:
            errString = "MaPL return value type mismatch.";
            break;
        default:
            errString = "Unknown MaPL error.";
            break;
    }
    MaPLStackFrame frame = stackFrames[stackFrames.size()-1];
    fprintf(stderr, "%s:%d: error: %s\n", frame.path.c_str(), frame.currentLineNumber, errString);
    exit(1);
}

void invokeScript(const std::filesystem::path &scriptPath) {
    stackFrames.push_back({ scriptPath });
    
    MaPLCompileOptions options{ true };
    MaPLCompileResult result = compileMaPL({ scriptPath }, options);
    
    // There should be only one compiled file, grab the first one.
    std::vector<u_int8_t> bytecode = result.compiledFiles.begin()->second;
    
    MaPLCallbacks callbacks{
        invokeFunction,
        invokeSubscript,
        assignProperty,
        assignSubscript,
        metadata,
        debugLine,
        debugVariableUpdate,
        debugVariableDelete,
        error,
    };
    executeMaPLScript(&(bytecode[0]), bytecode.size(), &callbacks);
    
    stackFrames.pop_back();
}

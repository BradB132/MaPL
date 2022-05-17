//
//  MaPLHandler.cpp
//  MaPLGenerator
//
//  Created by Brad Bambara on 5/17/22.
//

#include <stdlib.h>
#include <filesystem>
#include <string>
#include <unordered_map>
#include <vector>

#include "MaPLHandler.h"
#include "MaPLSymbols.h"

// One of the MaPL APIs allows it to invoke another script.
// Create a stack of metadata about the scripts that are currently running.
struct MaPLStackFrame {
    std::filesystem::path path;
    std::unordered_map<std::string, MaPLParameter> variables;
    MaPLLineNumber currentLineNumber;
};
static std::vector<MaPLStackFrame> stackFrames;

static MaPLParameter invokeFunction(const void *invokedOnPointer, MaPLSymbol functionSymbol, const MaPLParameter *argv, MaPLParameterCount argc) {
    switch (functionSymbol) { // TODO: Implement cases.
        case MaPLSymbols_ArrayMap_contains_string:
            
            break;
        case MaPLSymbols_Array_count:
            
            break;
        case MaPLSymbols_GLOBAL_commandLineFlagEnabled_string:
            
            break;
        case MaPLSymbols_GLOBAL_executeMaPLScript_string:
            
            break;
        case MaPLSymbols_GLOBAL_hash_string:
            
            break;
        case MaPLSymbols_GLOBAL_outputToFile_string:
            
            break;
        case MaPLSymbols_GLOBAL_pluralize_string:
            
            break;
        case MaPLSymbols_GLOBAL_schema:
            
            break;
        case MaPLSymbols_GLOBAL_writeToFile_VARIADIC:
            
            break;
        case MaPLSymbols_GLOBAL_xmlFiles:
            
            break;
        case MaPLSymbols_SchemaAttribute_annotations:
            
            break;
        case MaPLSymbols_SchemaAttribute_defaultValues:
            
            break;
        case MaPLSymbols_SchemaAttribute_isPrimitiveType:
            
            break;
        case MaPLSymbols_SchemaAttribute_maxOccurrences:
            
            break;
        case MaPLSymbols_SchemaAttribute_minOccurrences:
            
            break;
        case MaPLSymbols_SchemaAttribute_name:
            
            break;
        case MaPLSymbols_SchemaAttribute_typeIsUIDReference:
            
            break;
        case MaPLSymbols_SchemaAttribute_typeName:
            
            break;
        case MaPLSymbols_SchemaClass_annotations:
            
            break;
        case MaPLSymbols_SchemaClass_attributes:
            
            break;
        case MaPLSymbols_SchemaClass_name:
            
            break;
        case MaPLSymbols_SchemaClass_superclass:
            
            break;
        case MaPLSymbols_SchemaEnum_annotations:
            
            break;
        case MaPLSymbols_SchemaEnum_cases:
            
            break;
        case MaPLSymbols_SchemaEnum_name:
            
            break;
        case MaPLSymbols_Schema_classes:
            
            break;
        case MaPLSymbols_Schema_enums:
            
            break;
        case MaPLSymbols_XMLAttribute_name:
            
            break;
        case MaPLSymbols_XMLAttribute_value:
            
            break;
        case MaPLSymbols_XMLFile_outputPath:
            
            break;
        case MaPLSymbols_XMLFile_rootNode:
            
            break;
        case MaPLSymbols_XMLNode_attributes:
            
            break;
        case MaPLSymbols_XMLNode_children:
            
            break;
        case MaPLSymbols_XMLNode_name:
            
            break;
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

MaPLCallbacks getMaPLCallbacks() {
    return {
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
}

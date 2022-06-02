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
    // TODO: Add ofstream object.
};
static std::vector<MaPLStackFrame> _stackFrames;
static const MaPLArray<XmlNode *> *_xmlNodes;
static const MaPLArrayMap<Schema *> *_schemas;
static const std::unordered_map<std::string, std::string> *_flags;

bool pluralize(const std::string &singular, std::string &plural, const std::string &singularSuffix, const std::string &pluralSuffix) {
    if (singular.length() < singularSuffix.length() ||
        singular.compare(singular.length()-singularSuffix.length(), singularSuffix.length(), singularSuffix)) {
        return false;
    }
    plural = singular.substr(0, singular.length() - singularSuffix.length()) + pluralSuffix;
    return true;
}

static MaPLParameter invokeFunction(const void *invokedOnPointer, MaPLSymbol functionSymbol, const MaPLParameter *argv, MaPLParameterCount argc) {
    switch (functionSymbol) {
        case MaPLSymbols_GLOBAL_commandLineFlag_string: {
            const std::string &argString = argv[0].stringValue;
            if (!_flags->count(argString)) {
                MaPLStackFrame frame = _stackFrames[_stackFrames.size()-1];
                fprintf(stderr, "%s:%d: error: Attempted to reference missing flag '%s'.\n", frame.path.c_str(), frame.currentLineNumber, argv[0].stringValue);
                exit(1);
            }
            return MaPLStringByReference(_flags->at(argString).c_str());
        }
        case MaPLSymbols_GLOBAL_executeMaPLScript_string:
            // TODO: Resolve relative path to absolute path relative to the current file (on top of stack).
            break;
        case MaPLSymbols_GLOBAL_hash_string: {
            // sdbm algorithm.
            const char *str = argv[0].stringValue;
            uint64_t hash = 0;
            char c;
            while ((c = *str++)) {
                hash = c + (hash << 6) + (hash << 16) - hash;
            }
            return MaPLUint64(hash);
        }
            break;
        case MaPLSymbols_GLOBAL_outputToFile_string:
            // TODO: Resolve relative path to absolute path relative to the current file (on top of stack).
            // TODO: set ofstream to new object.
            break;
        case MaPLSymbols_GLOBAL_pluralize_string: {
            std::string singular = argv[0].stringValue;
            std::string plural;
            
            // Attempt a list of irregular english language singular->plural substitutions.
            if (pluralize(singular, plural, "us", "i")) { // Cactus -> Cacti
            } else if (pluralize(singular, plural, "is", "es")) { // Crisis -> Crises
            } else if (pluralize(singular, plural, "ix", "ices")) { // Matrix -> Matrices
            } else if (pluralize(singular, plural, "y", "ies")) { // Tally -> Tallies
            } else if (pluralize(singular, plural, "ss", "sses")) { // Pass -> Passes
            } else {
                // If by process of elimination the string appears to be regular, just add 's'.
                plural = singular + "s";
            }
            return MaPLStringByValue(plural.c_str());
        }
            break;
        case MaPLSymbols_GLOBAL_schemas:
            return MaPLPointer(_schemas);
        case MaPLSymbols_GLOBAL_writeToFile_VARIADIC:
            // TODO: Output values to ofstream.
            break;
        case MaPLSymbols_GLOBAL_xmlFiles:
            return MaPLPointer(_xmlNodes);
        default:
            // TODO: Check if invokedOnPointer is a MaPLInterface.
            break;
    }
    return MaPLUninitialized();
}

static MaPLParameter invokeSubscript(const void *invokedOnPointer, MaPLParameter index) {
    // TODO: Check if invokedOnPointer is a MaPLInterface.
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
    // TODO: Output to ofstream.
}

static void debugLine(MaPLLineNumber lineNumber) {
    _stackFrames[_stackFrames.size()-1].currentLineNumber = lineNumber;
}

static void debugVariableUpdate(const char *variableName, MaPLParameter newValue) {
    _stackFrames[_stackFrames.size()-1].variables[variableName] = newValue;
}

static void debugVariableDelete(const char *variableName) {
    _stackFrames[_stackFrames.size()-1].variables.erase(variableName);
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
    MaPLStackFrame frame = _stackFrames[_stackFrames.size()-1];
    fprintf(stderr, "%s:%d: error: %s\n", frame.path.c_str(), frame.currentLineNumber, errString);
    exit(1);
}

void invokeScript(const std::filesystem::path &scriptPath,
                  const MaPLArray<XmlNode *> *xmlNodes,
                  const MaPLArrayMap<Schema *> *schemas,
                  const std::unordered_map<std::string, std::string> &flags) {
    _xmlNodes = xmlNodes;
    _schemas = schemas;
    _flags = &flags;
    _stackFrames.push_back({ scriptPath });
    
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
    
    _stackFrames.pop_back();
}

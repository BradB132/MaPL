//
//  MaPLHandler.cpp
//  MaPLGenerator
//
//  Created by Brad Bambara on 5/17/22.
//

#include "MaPLHandler.h"

#include <fstream>
#include <regex>
#include <stdlib.h>
#include <string>
#include <unordered_map>
#include <vector>

#include "MaPLCompiler.h"
#include "MaPLSymbols.h"

void invokeScript(const std::filesystem::path &scriptPath);

// One of the MaPL APIs allows it to invoke another script.
// Create a stack of metadata about the scripts that are currently running.
struct MaPLStackFrame {
    std::filesystem::path path;
    std::unordered_map<std::string, MaPLParameter> variables;
    MaPLLineNumber currentLineNumber = 0;
    std::ofstream *outputStream = NULL;
};
static std::vector<MaPLStackFrame> _stackFrames;
static MaPLArray<XmlNode *> *_xmlNodes;
static MaPLArrayMap<Schema *> *_schemas;
static const std::unordered_map<std::string, std::string> *_flags;

bool pluralize(const std::string &singular, std::string &plural, const std::string &singularSuffix, const std::string &pluralSuffix) {
    if (singular.length() < singularSuffix.length() ||
        singular.compare(singular.length()-singularSuffix.length(), singularSuffix.length(), singularSuffix)) {
        return false;
    }
    plural = singular.substr(0, singular.length() - singularSuffix.length()) + pluralSuffix;
    return true;
}

std::filesystem::path normalizedParamPath(const char *pathParam) {
    std::filesystem::path path = pathParam;
    if (path.is_relative()) {
        MaPLStackFrame &frame = _stackFrames[_stackFrames.size()-1];
        path = frame.path.parent_path() / path;
    }
    return path.lexically_normal();
}

static MaPLParameter invokeFunction(void *invokedOnPointer, MaPLSymbol functionSymbol, const MaPLParameter *argv, MaPLParameterCount argc) {
    switch (functionSymbol) {
        case MaPLSymbols_GLOBAL_commandLineFlag_string: {
            const std::string &argString = argv[0].stringValue;
            if (!_flags->count(argString)) {
                MaPLStackFrame &frame = _stackFrames[_stackFrames.size()-1];
                fprintf(stderr, "%s:%d: error: Attempted to reference missing flag '%s'.\n", frame.path.c_str(), frame.currentLineNumber, argv[0].stringValue);
                exit(1);
            }
            return MaPLStringByReference(_flags->at(argString).c_str());
        }
        case MaPLSymbols_GLOBAL_executeMaPLScript_string: {
            std::filesystem::path normalizedPath = normalizedParamPath(argv[0].stringValue);
            invokeScript(normalizedPath);
        }
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
        case MaPLSymbols_GLOBAL_outputToFile_string: {
            MaPLStackFrame &frame = _stackFrames[_stackFrames.size()-1];
            delete frame.outputStream;
            std::filesystem::path normalizedPath = normalizedParamPath(argv[0].stringValue);
            frame.outputStream = new std::ofstream(normalizedPath);
        }
            break;
        case MaPLSymbols_GLOBAL_pluralize_string: {
            std::string singular = argv[0].stringValue;
            std::string plural;
            
            // Attempt a list of irregular english language singular->plural substitutions.
            // There are more types of irregular plurals in English, but these patterns seem
            // the most reliable.
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
        case MaPLSymbols_GLOBAL_writeToFile_VARIADIC: {
            MaPLStackFrame &frame = _stackFrames[_stackFrames.size()-1];
            if (!frame.outputStream) {
                fprintf(stderr, "%s:%d: error: Attempted to write to file before any output file was specified.\n", frame.path.c_str(), frame.currentLineNumber);
                exit(1);
            }
            for (MaPLParameterCount i = 0; i < argc; i++) {
                switch (argv[i].dataType) {
                    case MaPLDataType_char:
                        frame.outputStream->write((char *)&(argv[i].charValue), sizeof(argv[i].charValue));
                        break;
                    case MaPLDataType_int32:
                        frame.outputStream->write((char *)&(argv[i].int32Value), sizeof(argv[i].int32Value));
                        break;
                    case MaPLDataType_int64:
                        frame.outputStream->write((char *)&(argv[i].int64Value), sizeof(argv[i].int64Value));
                        break;
                    case MaPLDataType_uint32:
                        frame.outputStream->write((char *)&(argv[i].uint32Value), sizeof(argv[i].uint32Value));
                        break;
                    case MaPLDataType_uint64:
                        frame.outputStream->write((char *)&(argv[i].uint64Value), sizeof(argv[i].uint64Value));
                        break;
                    case MaPLDataType_float32:
                        frame.outputStream->write((char *)&(argv[i].float32Value), sizeof(argv[i].float32Value));
                        break;
                    case MaPLDataType_float64:
                        frame.outputStream->write((char *)&(argv[i].float64Value), sizeof(argv[i].float64Value));
                        break;
                    case MaPLDataType_string:
                        frame.outputStream->write(argv[i].stringValue, strlen(argv[i].stringValue)+1);
                        break;
                    case MaPLDataType_boolean:
                        frame.outputStream->write((char *)&(argv[i].booleanValue), sizeof(argv[i].booleanValue));
                        break;
                    default:
                        fprintf(stderr, "%s:%d: error: MaPL argument at index %d was invalid type.\n", frame.path.c_str(), frame.currentLineNumber, i);
                        exit(1);
                        break;
                }
            }
        }
            break;
        case MaPLSymbols_GLOBAL_xmlFiles:
            return MaPLPointer(_xmlNodes);
        default:
            if (invokedOnPointer) {
                MaPLInterface *maPLInterface = static_cast<MaPLInterface *>(invokedOnPointer);
                return maPLInterface->invokeFunction(functionSymbol, argv, argc);
            }
            break;
    }
    return MaPLUninitialized();
}

static MaPLParameter invokeSubscript(void *invokedOnPointer, MaPLParameter index) {
    if (invokedOnPointer) {
        MaPLInterface *maPLInterface = static_cast<MaPLInterface *>(invokedOnPointer);
        return maPLInterface->invokeSubscript(index);
    }
    return MaPLUninitialized();
}

static void assignProperty(void *invokedOnPointer, MaPLSymbol propertySymbol, MaPLParameter assignedValue) {
    // No-op. API contains no write-able properties.
}

static void assignSubscript(void *invokedOnPointer, MaPLParameter index, MaPLParameter assignedValue) {
    // No-op. API contains no write-able subscripts.
}

static void metadata(const char* metadataString) {
    MaPLStackFrame &frame = _stackFrames[_stackFrames.size()-1];
    if (!frame.outputStream) {
        fprintf(stderr, "%s:%d: error: Attempted to write metadata to file before any output file was specified.\n", frame.path.c_str(), frame.currentLineNumber);
        exit(1);
    }
    std::string outputString = metadataString;
    std::smatch match;
    std::regex variableSubstitutionRegex("\\$\\{([_a-zA-Z][_a-zA-Z0-9]*)\\}");
    auto searchStart = cbegin(outputString);
    while (std::regex_search(searchStart, cend(outputString), match, variableSubstitutionRegex)) {
        std::string variableName = match[1].str();
        MaPLParameter matchedVariable = frame.variables.at(variableName);
        std::string variableValue;
        switch (matchedVariable.dataType) {
            case MaPLDataType_char:
                variableValue = std::to_string(matchedVariable.charValue);
                break;
            case MaPLDataType_int32:
                variableValue = std::to_string(matchedVariable.int32Value);
                break;
            case MaPLDataType_int64:
                variableValue = std::to_string(matchedVariable.int64Value);
                break;
            case MaPLDataType_uint32:
                variableValue = std::to_string(matchedVariable.uint32Value);
                break;
            case MaPLDataType_uint64:
                variableValue = std::to_string(matchedVariable.uint64Value);
                break;
            case MaPLDataType_float32:
                variableValue = std::to_string(matchedVariable.float32Value);
                break;
            case MaPLDataType_float64:
                variableValue = std::to_string(matchedVariable.float64Value);
                break;
            case MaPLDataType_string:
                variableValue = matchedVariable.stringValue;
                break;
            case MaPLDataType_boolean:
                variableValue = std::to_string(matchedVariable.booleanValue);
                break;
            default:
                fprintf(stderr, "%s:%d: error: Variable '%s' is not a printable data type.\n", frame.path.c_str(), frame.currentLineNumber, variableName.c_str());
                exit(1);
                break;
        }
        
        outputString.replace(match[0].first, match[0].second, variableValue);
        searchStart = match[0].first;
    }
    *frame.outputStream << outputString;
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
    MaPLStackFrame &frame = _stackFrames[_stackFrames.size()-1];
    fprintf(stderr, "%s:%d: error: %s\n", frame.path.c_str(), frame.currentLineNumber, errString);
    exit(1);
}

void invokeScript(const std::filesystem::path &scriptPath) {
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
    
    MaPLStackFrame &frame = _stackFrames[_stackFrames.size()-1];
    delete frame.outputStream;
    _stackFrames.pop_back();
}

void invokeScript(const std::filesystem::path &scriptPath,
                  MaPLArray<XmlNode *> *xmlNodes,
                  MaPLArrayMap<Schema *> *schemas,
                  const std::unordered_map<std::string, std::string> &flags) {
    _xmlNodes = xmlNodes;
    _schemas = schemas;
    _flags = &flags;
    invokeScript(scriptPath);
}

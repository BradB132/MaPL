//
//  MaPLHandler.cpp
//  MaPLGenerator
//
//  Created by Brad Bambara on 5/17/22.
//

#include "MaPLHandler.h"

#include <fstream>
#include <filesystem>
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
    std::unordered_map<std::string, void *> inParameters;
    std::unordered_map<std::string, void *> outParameters;
    std::unordered_map<std::string, MaPLParameter> variables;
    MaPLLineNumber currentLineNumber = 0;
};
static std::ofstream *_outputStream = NULL;
static std::vector<MaPLStackFrame> _stackFrames;
static MaPLArray<XmlFile *> *_xmlFiles;
static MaPLArrayMap<Schema *> *_schemas;
static const std::unordered_map<std::string, std::string> *_flags;
static std::unordered_map<std::string, std::vector<u_int8_t>> _bytecodeCache;

std::filesystem::path normalizedParamPath(const char *pathParam) {
    std::filesystem::path path = pathParam;
    if (path.is_relative()) {
        MaPLStackFrame &frame = _stackFrames[_stackFrames.size()-1];
        path = frame.path.parent_path() / path;
    }
    return path.lexically_normal();
}

void createDirectoriesIfNeeded(const std::filesystem::path &path) {
    std::filesystem::path parentDirectory = path.parent_path();
    if (std::filesystem::exists(parentDirectory)) {
        return;
    }
    createDirectoriesIfNeeded(parentDirectory);
    std::filesystem::create_directory(parentDirectory);
}

static MaPLParameter invokeFunction(void *invokedOnPointer, MaPLSymbol functionSymbol, const MaPLParameter *argv, MaPLParameterCount argc) {
    switch (functionSymbol) {
        case MaPLSymbols_GLOBAL_commandLineFlag_string: {
            const std::string &argString = argv[0].stringValue;
            if (!_flags->count(argString)) {
                MaPLStackFrame &frame = _stackFrames[_stackFrames.size()-1];
                fprintf(stderr, "%s:%d: error: Attempted to reference missing flag '%s'. (Runtime)\n", frame.path.c_str(), frame.currentLineNumber, argv[0].stringValue);
                exit(1);
            }
            return MaPLStringByReference(_flags->at(argString).c_str());
        }
        case MaPLSymbols_GLOBAL_error_string: {
            MaPLStackFrame &frame = _stackFrames[_stackFrames.size()-1];
            fprintf(stderr, "%s:%d: error: %s (Runtime)\n", frame.path.c_str(), frame.currentLineNumber, argv[0].stringValue);
            exit(1);
        }
            break;
        case MaPLSymbols_GLOBAL_executeMaPLScript_string: {
            std::filesystem::path normalizedPath = normalizedParamPath(argv[0].stringValue);
            invokeScript(normalizedPath);
            return MaPLVoid();
        }
        case MaPLSymbols_GLOBAL_fileExistsAtPath_string: {
            std::filesystem::path path = argv[0].stringValue;
            return MaPLBool(std::filesystem::exists(path));
        }
        case MaPLSymbols_GLOBAL_fileStemForPath_string: {
            std::filesystem::path path = argv[0].stringValue;
            std::string stem = path.stem();
            return MaPLStringByValue(stem.c_str());
        }
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
        case MaPLSymbols_GLOBAL_outputToFile_string: {
            delete _outputStream;
            std::filesystem::path normalizedPath = normalizedParamPath(argv[0].stringValue);
            createDirectoriesIfNeeded(normalizedPath);
            _outputStream = new std::ofstream(normalizedPath);
            return MaPLVoid();
        }
        case MaPLSymbols_GLOBAL_schemas:
            return MaPLPointer(_schemas);
        case MaPLSymbols_GLOBAL_inParameters: {
            MaPLStackFrame &frame = _stackFrames[_stackFrames.size()-1];
            return MaPLPointer(&frame.inParameters);
        }
        case MaPLSymbols_GLOBAL_outParameters: {
            MaPLStackFrame &frame = _stackFrames[_stackFrames.size()-1];
            return MaPLPointer(&frame.outParameters);
        }
        case MaPLSymbols_GLOBAL_writeToFile_VARIADIC: {
            MaPLStackFrame &frame = _stackFrames[_stackFrames.size()-1];
            if (!_outputStream) {
                fprintf(stderr, "%s:%d: error: Attempted to write to file before any output file was specified. (Runtime)\n", frame.path.c_str(), frame.currentLineNumber);
                exit(1);
            }
            for (MaPLParameterCount i = 0; i < argc; i++) {
                switch (argv[i].dataType) {
                    case MaPLDataType_char:
                        _outputStream->write((char *)&(argv[i].charValue), sizeof(argv[i].charValue));
                        break;
                    case MaPLDataType_int32:
                        _outputStream->write((char *)&(argv[i].int32Value), sizeof(argv[i].int32Value));
                        break;
                    case MaPLDataType_int64:
                        _outputStream->write((char *)&(argv[i].int64Value), sizeof(argv[i].int64Value));
                        break;
                    case MaPLDataType_uint32:
                        _outputStream->write((char *)&(argv[i].uint32Value), sizeof(argv[i].uint32Value));
                        break;
                    case MaPLDataType_uint64:
                        _outputStream->write((char *)&(argv[i].uint64Value), sizeof(argv[i].uint64Value));
                        break;
                    case MaPLDataType_float32:
                        _outputStream->write((char *)&(argv[i].float32Value), sizeof(argv[i].float32Value));
                        break;
                    case MaPLDataType_float64:
                        _outputStream->write((char *)&(argv[i].float64Value), sizeof(argv[i].float64Value));
                        break;
                    case MaPLDataType_string:
                        _outputStream->write(argv[i].stringValue, strlen(argv[i].stringValue)+1);
                        break;
                    case MaPLDataType_boolean:
                        _outputStream->write((char *)&(argv[i].booleanValue), sizeof(argv[i].booleanValue));
                        break;
                    default:
                        fprintf(stderr, "%s:%d: error: MaPL argument at index %d was invalid type. (Runtime)\n", frame.path.c_str(), frame.currentLineNumber, i);
                        exit(1);
                        break;
                }
            }
            return MaPLVoid();
        }
        case MaPLSymbols_GLOBAL_xmlFiles:
            return MaPLPointer(_xmlFiles);
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
    if (!invokedOnPointer) {
        return MaPLUninitialized();
    }
    if (index.dataType == MaPLDataType_string) {
        MaPLStackFrame &frame = _stackFrames[_stackFrames.size()-1];
        std::string stringKey = index.stringValue;
        if (invokedOnPointer == &frame.inParameters) {
            return MaPLPointer(frame.inParameters.count(stringKey) ? frame.inParameters.at(stringKey) : NULL);
        } else if (invokedOnPointer == &frame.outParameters) {
            return MaPLPointer(frame.outParameters.count(stringKey) ? frame.outParameters.at(stringKey) : NULL);
        }
    }
    MaPLInterface *maPLInterface = static_cast<MaPLInterface *>(invokedOnPointer);
    return maPLInterface->invokeSubscript(index);
}

static void assignProperty(void *invokedOnPointer, MaPLSymbol propertySymbol, MaPLParameter assignedValue) {
    // No-op. API contains no write-able properties.
}

static void assignSubscript(void *invokedOnPointer, MaPLParameter index, MaPLParameter assignedValue) {
    std::unordered_map<std::string, void *> *invokedOnMap = (std::unordered_map<std::string, void *> *)invokedOnPointer;
    if (index.dataType == MaPLDataType_string &&
        assignedValue.dataType == MaPLDataType_pointer) {
        (*invokedOnMap)[index.stringValue] = assignedValue.pointerValue;
    }
}

static void metadata(const char* metadataString) {
    MaPLStackFrame &frame = _stackFrames[_stackFrames.size()-1];
    if (!_outputStream) {
        fprintf(stderr, "%s:%d: error: Attempted to write metadata to file before any output file was specified. (Runtime)\n", frame.path.c_str(), frame.currentLineNumber);
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
                fprintf(stderr, "%s:%d: error: Variable '%s' is not a printable data type. (Runtime)\n", frame.path.c_str(), frame.currentLineNumber, variableName.c_str());
                exit(1);
                break;
        }
        
        outputString.replace(match[0].first, match[0].second, variableValue);
        searchStart = cbegin(outputString);
    }
    *_outputStream << outputString;
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
    fprintf(stderr, "%s:%d: error: %s (Runtime)\n", frame.path.c_str(), frame.currentLineNumber, errString);
    exit(1);
}

void invokeScript(const std::filesystem::path &scriptPath) {
    if (_stackFrames.size()) {
        // When a previous stack frame exists, pass the script parameters into the new frame.
        MaPLStackFrame &previousFrame = _stackFrames[_stackFrames.size()-1];
        _stackFrames.push_back({ scriptPath, previousFrame.outParameters });
    } else {
        _stackFrames.push_back({ scriptPath });
    }
    
    std::string pathString = scriptPath.string();
    std::vector<u_int8_t> bytecode;
    if (_bytecodeCache.count(pathString) == 0) {
        MaPLCompileOptions options{ true };
        MaPLCompileResult result = compileMaPL({ scriptPath }, options);
        
        if (result.errorMessages.size()) {
            for (const std::string &errorMessage : result.errorMessages) {
                fputs(errorMessage.c_str(), stderr);
            }
            exit(1);
        }
        
        // There should be only one compiled file, grab the first one.
        bytecode = result.compiledFiles.begin()->second;
        _bytecodeCache[pathString] = bytecode;
    } else {
        bytecode = _bytecodeCache.at(pathString);
    }
    
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

void invokeScript(const std::filesystem::path &scriptPath,
                  MaPLArray<XmlFile *> *xmlFiles,
                  MaPLArrayMap<Schema *> *schemas,
                  const std::unordered_map<std::string, std::string> &flags) {
    _xmlFiles = xmlFiles;
    _schemas = schemas;
    _flags = &flags;
    invokeScript(scriptPath);
    delete _outputStream;
    _outputStream = NULL;
}

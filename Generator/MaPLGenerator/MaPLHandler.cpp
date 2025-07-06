//
//  MaPLHandler.cpp
//  MaPLGenerator
//
//  Created by Brad Bambara on 5/17/22.
//

#include "MaPLHandler.h"

#include <fstream>
#include <filesystem>
#include <stdlib.h>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <stdint.h>

#include "MaPLCompiler.h"
#include "MaPLSymbols.h"

void invokeScript(const std::filesystem::path &scriptPath);

// One of the MaPL APIs allows it to invoke another script.
// Create a stack of metadata about the scripts that are currently running.
struct MaPLStackFrame {
    std::filesystem::path path;
    std::unordered_map<std::string, void *> inParameters;
    std::unordered_map<std::string, void *> outParameters;
    MaPLLineNumber currentLineNumber = 0;
};
static std::ofstream *_outputStream = NULL;
static std::vector<MaPLStackFrame> _stackFrames;
static MaPLArray<XmlFile *> *_xmlFiles;
static MaPLArrayMap<Schema *> *_schemas;
static const std::unordered_map<std::string, std::string> *_flags;
static std::unordered_map<std::string, std::vector<uint8_t>> _bytecodeCache;
static std::unordered_set<std::string> _stringSet;
static std::unordered_set<std::string> _spellcheckDictionary;

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

std::string removeSuffix(const std::string& str, const std::string& suffix) {
    if (suffix.size() <= str.size() &&
        std::equal(suffix.rbegin(), suffix.rend(), str.rbegin())) {
        return str.substr(0, str.size() - suffix.size());
    }
    return str;
}

bool spellcheckWord(const std::string& word) {
    // Spellcheck by removing the possessive "'s" if it exists.
    if (_spellcheckDictionary.find(removeSuffix(word, "'s")) == _spellcheckDictionary.end()) {
        return false;
    }
    return true;
}

static MaPLParameter invokeFunction(void *invokedOnPointer, MaPLSymbol functionSymbol, const MaPLParameter *argv, MaPLParameterCount argc) {
    switch (functionSymbol) {
        case MaPLSymbols_GLOBAL_commandLineFlag_string: {
            const std::string &argString = argv[0].stringValue;
            if (!_flags->count(argString)) {
                MaPLStackFrame &frame = _stackFrames[_stackFrames.size()-1];
                fprintf(stderr, "%s:%d: error: Attempted to reference missing flag '%s'. (Runtime)\n", frame.path.u8string().c_str(), frame.currentLineNumber, argv[0].stringValue);
                exit(1);
            }
            return MaPLStringByReference(_flags->at(argString).c_str());
        }
        case MaPLSymbols_GLOBAL_deviceIsLittleEndian: {
            uint16_t endianShort = 1;
            uint8_t endianByte = *(uint8_t *)&endianShort;
            return MaPLBool(endianByte == 1);
        }
        case MaPLSymbols_GLOBAL_error_string: {
            MaPLStackFrame &frame = _stackFrames[_stackFrames.size()-1];
            fprintf(stderr, "%s:%d: error: %s (Runtime)\n", frame.path.u8string().c_str(), frame.currentLineNumber, argv[0].stringValue);
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
            std::string stem = path.stem().u8string();
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
        case MaPLSymbols_GLOBAL_length_string: {
            return MaPLUint64(strlen(argv[0].stringValue));
        }
        case MaPLSymbols_GLOBAL_substring_string_uint64: {
            const char *string = argv[0].stringValue;
            unsigned long stringLength = strlen(string);
            unsigned long startIndex = argv[1].uint64Value;
            if (startIndex >= stringLength) {
                return MaPLStringByValue("");
            }
            return MaPLStringByValue(string+startIndex);
        }
        case MaPLSymbols_GLOBAL_substring_string_uint64_uint64: {
            const char *string = argv[0].stringValue;
            unsigned long stringLength = strlen(string);
            unsigned long startIndex = argv[1].uint64Value;
            if (startIndex >= stringLength) {
                return MaPLStringByValue("");
            }
            unsigned long substringLength = argv[2].uint64Value;
            if ((startIndex+substringLength) >= stringLength) {
                substringLength = stringLength - startIndex;
            }
#if defined(_MSC_VER)
            // MSVC doesn't support variable length arrays, use malloc.
            char *substring = (char *)malloc(sizeof(char) * (substringLength+1));
#else
            char substring[substringLength+1];
#endif
            strncpy(substring, string+startIndex, substringLength);
            substring[substringLength] = 0;
            MaPLParameter returnValue = MaPLStringByValue(substring);
#if defined(_MSC_VER)
            free(substring);
#endif
            return returnValue;
        }
        case MaPLSymbols_GLOBAL_indexOf_string_string: {
            const char *searchedString = argv[0].stringValue;
            const char *found = strstr(searchedString, argv[1].stringValue);
            if(!found) {
                return MaPLInt64(-1);
            }
            // Subtract memory addresses to calculate the index.
            return MaPLInt64(found - searchedString);
        }
        case MaPLSymbols_GLOBAL_replaceAll_string_string_string: {
            // Check to make sure the substring exists in the searched string.
            if(!strstr(argv[0].stringValue, argv[1].stringValue)) {
                // No match found, return the original string.
                return MaPLStringByValue(argv[0].stringValue);
            }
            
            // A matching substring exists. Leverage `std::string` to perform replacement.
            std::string searchedString = argv[0].stringValue;
            std::string oldSubstring = argv[1].stringValue;
            std::string newSubstring = argv[2].stringValue;
            size_t pos = 0;
            while ((pos = searchedString.find(oldSubstring, pos)) != std::string::npos) {
                searchedString.replace(pos, oldSubstring.length(), newSubstring);
                 pos += newSubstring.length();
            }
            return MaPLStringByValue(searchedString.c_str());
        }
        case MaPLSymbols_GLOBAL_lastIndexOf_string_string: {
            const char* found;
            const char* nextFound = argv[0].stringValue;
            do {
                found = nextFound;
                nextFound = strstr(found+1, argv[1].stringValue);
            } while(nextFound);
            
            if (!found) {
                return MaPLInt64(-1);
            }
            // Subtract memory addresses to calculate the index.
            return MaPLInt64(found - argv[0].stringValue);
        }
        case MaPLSymbols_GLOBAL_toUpper_string: {
            const char *string = argv[0].stringValue;
            unsigned long stringLength = strlen(string);
            
#if defined(_MSC_VER)
            // MSVC doesn't support variable length arrays, use malloc.
            char *copiedString = (char *)malloc(sizeof(char) * (stringLength+1));
#else
            char copiedString[stringLength+1];
#endif
            
            for(int i = 0; i < stringLength; i++) {
                copiedString[i] = toupper(string[i]);
            }
            copiedString[stringLength] = 0;
            
            MaPLParameter returnValue = MaPLStringByValue(copiedString);
#if defined(_MSC_VER)
            free(copiedString);
#endif
            return returnValue;
        }
        case MaPLSymbols_GLOBAL_toLower_string: {
            const char *string = argv[0].stringValue;
            unsigned long stringLength = strlen(string);
            
#if defined(_MSC_VER)
            // MSVC doesn't support variable length arrays, use malloc.
            char *copiedString = (char *)malloc(sizeof(char) * (stringLength+1));
#else
            char copiedString[stringLength+1];
#endif
            
            for(int i = 0; i < stringLength; i++) {
                copiedString[i] = tolower(string[i]);
            }
            copiedString[stringLength] = 0;
            
            MaPLParameter returnValue = MaPLStringByValue(copiedString);
#if defined(_MSC_VER)
            free(copiedString);
#endif
            return returnValue;
        }
        case MaPLSymbols_GLOBAL_outputToFile_string: {
            delete _outputStream;
            std::filesystem::path normalizedPath = normalizedParamPath(argv[0].stringValue);
            createDirectoriesIfNeeded(normalizedPath);
            _outputStream = new std::ofstream(normalizedPath, std::ios::out | std::ios::binary);
            printf("Outputting to file: '%s'\n", normalizedPath.u8string().c_str());
            return MaPLVoid();
        }
        case MaPLSymbols_GLOBAL_schemas:
            return MaPLPointer(_schemas);
        case MaPLSymbols_GLOBAL_stringSet:
            return MaPLPointer(&_stringSet);
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
                fprintf(stderr, "%s:%d: error: Attempted to write to file before any output file was specified. (Runtime)\n", frame.path.u8string().c_str(), frame.currentLineNumber);
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
                        fprintf(stderr, "%s:%d: error: MaPL argument at index %d was invalid type. (Runtime)\n", frame.path.u8string().c_str(), frame.currentLineNumber, i);
                        exit(1);
                        break;
                }
            }
            return MaPLVoid();
        }
        case MaPLSymbols_GLOBAL_xmlFiles:
            return MaPLPointer(_xmlFiles);
        case MaPLSymbols_StringSet_clear:
            ((std::unordered_set<std::string> *)invokedOnPointer)->clear();
            return MaPLVoid();
        case MaPLSymbols_StringSet_contains_string:
            return MaPLBool(((std::unordered_set<std::string> *)invokedOnPointer)->count(argv[0].stringValue));
        case MaPLSymbols_StringSet_insert_string:
            ((std::unordered_set<std::string> *)invokedOnPointer)->insert(argv[0].stringValue);
            return MaPLVoid();
        case MaPLSymbols_StringSet_remove_string: {
            size_t eraseCount = ((std::unordered_set<std::string> *)invokedOnPointer)->erase(argv[0].stringValue);
            return MaPLBool(eraseCount != 0);
        }
        case MaPLSymbols_GLOBAL_loadSpellcheckDictionary_string: {
            std::filesystem::path normalizedPath = normalizedParamPath(argv[0].stringValue);
            std::ifstream file(normalizedPath);
            std::string word;
            while (std::getline(file, word)) {
                for (auto& ch : word) ch = std::tolower(static_cast<unsigned char>(ch));
                _spellcheckDictionary.insert(word);
            }
            return MaPLVoid();
        }
        case MaPLSymbols_GLOBAL_spellcheck_string: {
            // Tokenize the string into words. Spellcheck each word.
            std::string token;
            std::string spellingErrors;
            bool success = true;
            bool hasLetter = false;
            for (const char* p = argv[0].stringValue; *p != '\0'; ++p) {
                char ch = *p;
                if (std::isalpha(static_cast<unsigned char>(ch))) {
                    token += std::tolower(ch);
                    hasLetter = true;
                } else if (std::isdigit(static_cast<unsigned char>(ch)) || ch == '\'') {
                    token += ch;
                } else {
                    if (!token.empty() && hasLetter) {
                        if (!spellcheckWord(token)) {
                            success = false;
                            spellingErrors += token;
                            spellingErrors += "\n";
                        }
                    }
                    token.clear();
                    hasLetter = false;
                }
            }
            if (!token.empty() && hasLetter) {
                if (!spellcheckWord(token)) {
                    success = false;
                    spellingErrors += token;
                    spellingErrors += "\n";
                }
            }
            if (!success) {
                fprintf(stderr, "Spelling error in \"%s\":\n%s", argv[0].stringValue, spellingErrors.c_str());
            }
            return MaPLBool(success);
        }
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
        fprintf(stderr, "%s:%d: error: Attempted to write metadata to file before any output file was specified. (Runtime)\n", frame.path.u8string().c_str(), frame.currentLineNumber);
        exit(1);
    }
    *_outputStream << metadataString;
}

static void debugLine(MaPLLineNumber lineNumber) {
    _stackFrames[_stackFrames.size()-1].currentLineNumber = lineNumber;
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
        case MaPLRuntimeError_incompatibleEndianness:
            errString = "Incompatible endianness in MaPL bytecode.";
            break;
        default:
            errString = "Unknown MaPL error.";
            break;
    }
    MaPLStackFrame &frame = _stackFrames[_stackFrames.size()-1];
    fprintf(stderr, "%s:%d: error: %s (Runtime)\n", frame.path.u8string().c_str(), frame.currentLineNumber, errString);
    exit(1);
}

void invokeScript(const std::filesystem::path &scriptPath) {
    if (!std::filesystem::exists(scriptPath)) {
        if (_stackFrames.size() > 0) {
            MaPLStackFrame &frame = _stackFrames[_stackFrames.size()-1];
            fprintf(stderr, "%s:%d: error: Attempted to run script in non-existent file '%s'. (Runtime)\n", frame.path.u8string().c_str(), frame.currentLineNumber, scriptPath.u8string().c_str());
        } else {
            fprintf(stderr, "Attempted to run script in non-existent file '%s'.\n", scriptPath.u8string().c_str());
        }
        exit(1);
    }
    if (_stackFrames.size()) {
        // When a previous stack frame exists, pass the script parameters into the new frame.
        MaPLStackFrame &previousFrame = _stackFrames[_stackFrames.size()-1];
        _stackFrames.push_back({ scriptPath, previousFrame.outParameters });
    } else {
        _stackFrames.push_back({ scriptPath });
    }
    
    std::string pathString = scriptPath.string();
    std::vector<uint8_t> bytecode;
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
        NULL,
        NULL,
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
    _stringSet.clear();
}

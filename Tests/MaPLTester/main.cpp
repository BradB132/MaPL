//
//  main.cpp
//  MaPLTester
//
//  Created by Brad Bambara on 4/12/22.
//

#include <filesystem>
#include <fstream>
#include <sstream>
#include <vector>

#include "MaPLCompiler.h"
#include "MaPLRuntime.h"
#include "TestSymbols.h"

// Generating the various expected files can be laborious. Enabling this option overwrites all expected files with the generated values.
#define OUTPUT_EXPECTED_FILES 0
#define PRINT_ERROR_OUTPUT 0

struct TestDirectoryContents {
    std::filesystem::path bytecodePath;
    std::filesystem::path printPath;
    std::filesystem::path callbacksPath;
};

char fakeGlobalObject;
char fakeChildObject;
int32_t fakeIntProperty;
float fakeFloatProperty;
std::string fakeStringProperty;
int32_t fakeIntSubscript;
float fakeFloatSubscript;
std::string fakeStringSubscript;

std::string scriptPrintString;
std::string scriptCallbacksString;
bool scriptEncounteredError = false;

std::string pointerToString(const void *pointer) {
    // Memory address can (and almost certainly will be) different each time.
    // Only meaningful distinction is NULL vs non-NULL.
    if (!pointer) {
        return "(pointer)NULL";
    } else if (pointer == &fakeGlobalObject) {
        return "(Object)globalObject";
    } else if (pointer == &fakeChildObject) {
        return "(ChildType)childObject";
    }
    return "(pointer)[unrecognized non-NULL address]";
}

std::string parameterToString(const MaPLParameter &parameter) {
    switch (parameter.dataType) {
        case MaPLDataType_char:
            return "(char)"+std::to_string(parameter.charValue);
        case MaPLDataType_int32:
            return "(int32)"+std::to_string(parameter.int32Value);
        case MaPLDataType_int64:
            return "(int64)"+std::to_string(parameter.int64Value);
        case MaPLDataType_uint32:
            return "(uint32)"+std::to_string(parameter.uint32Value);
        case MaPLDataType_uint64:
            return "(uint64)"+std::to_string(parameter.uint64Value);
        case MaPLDataType_float32:
            return "(float32)"+std::to_string(parameter.float32Value);
        case MaPLDataType_float64:
            return "(float64)"+std::to_string(parameter.float64Value);
        case MaPLDataType_string:
            return "(string)\""+std::string(parameter.stringValue)+"\"";
        case MaPLDataType_boolean:
            return "(bool)"+std::string(parameter.booleanValue ? "true" : "false");
        case MaPLDataType_pointer:
            return pointerToString(parameter.pointerValue);
        default:
            printf("Invalid parameter type passed to host program.\n");
            exit(1);
    }
}

MaPLParameter invokeFunction(const void *invokedOnPointer, MaPLSymbol functionSymbol, const MaPLParameter *argv, MaPLParameterCount argc) {
    scriptCallbacksString += "Invoke function: pointer="+pointerToString(invokedOnPointer)+", symbol="+std::to_string(functionSymbol)+", ";
    for (MaPLParameterCount i = 0; i < argc; i++) {
        scriptCallbacksString += "parameter #"+std::to_string(i)+"="+parameterToString(argv[i]);
        if (i != argc-1) {
            scriptCallbacksString += ", ";
        }
    }
    scriptCallbacksString += "\n";
    
    if (!invokedOnPointer) {
        // invokedOnPointer is NULL when accessing a global.
        switch (functionSymbol) {
            case TestSymbols_GLOBAL_print_VARIADIC:
                for (MaPLParameterCount i = 0; i < argc; i++) {
                    scriptPrintString += parameterToString(argv[i]);
                    if (i == argc-1) {
                        scriptPrintString += "\n";
                    } else {
                        scriptPrintString += ", ";
                    }
                }
                break;
            case TestSymbols_GLOBAL_globalObject:
                return MaPLPointer(&fakeGlobalObject);
            case TestSymbols_GLOBAL_childObject:
                return MaPLPointer(&fakeChildObject);
            case TestSymbols_GLOBAL_falseFunc:
                return MaPLBool(false);
            case TestSymbols_GLOBAL_trueFunc:
                return MaPLBool(true);
            default: break;
        }
    } else if (invokedOnPointer == &fakeGlobalObject) {
        switch (functionSymbol) {
            case TestSymbols_Object_intProperty:
                return MaPLInt32(fakeIntProperty);
            case TestSymbols_Object_floatProperty:
                return MaPLFloat32(fakeFloatProperty);
            case TestSymbols_Object_stringProperty:
                return MaPLStringByValue(fakeStringProperty.c_str());
            default: break;
        }
    } else if (invokedOnPointer == &fakeChildObject) {
        switch (functionSymbol) {
            case TestSymbols_ChildType_childProperty:
                return MaPLInt32(1234);
            case TestSymbols_ParentType_parentProperty:
                return MaPLInt32(5678);
            default: break;
        }
    }
    
    return MaPLUninitialized();
}

MaPLParameter invokeSubscript(const void *invokedOnPointer, MaPLParameter index) {
    scriptCallbacksString += "Invoke subscript: pointer="+pointerToString(invokedOnPointer)+", index="+parameterToString(index)+"\n";
    if (invokedOnPointer == &fakeGlobalObject) {
        switch (index.dataType) {
            case MaPLDataType_int32:
                if (index.int32Value == 0) {
                    return MaPLInt32(fakeIntSubscript);
                }
                break;
            case MaPLDataType_float32:
                if (index.float32Value == 0) {
                    return MaPLFloat32(fakeFloatSubscript);
                }
                break;
            case MaPLDataType_string:
                if (!strcmp(index.stringValue, "0")) {
                    return MaPLStringByValue(fakeStringSubscript.c_str());
                }
                break;
            default: break;
        }
    }
    return MaPLUninitialized();
}

void assignProperty(const void *invokedOnPointer, MaPLSymbol propertySymbol, MaPLParameter assignedValue) {
    scriptCallbacksString += "Assign property: pointer="+pointerToString(invokedOnPointer)+", symbol="+std::to_string(propertySymbol)+", value="+parameterToString(assignedValue)+"\n";
    if (invokedOnPointer == &fakeGlobalObject) {
        switch (propertySymbol) {
            case TestSymbols_Object_intProperty:
                fakeIntProperty = assignedValue.int32Value;
                break;
            case TestSymbols_Object_floatProperty:
                fakeFloatProperty = assignedValue.float32Value;
                break;
            case TestSymbols_Object_stringProperty:
                fakeStringProperty = assignedValue.stringValue;
                break;
            default: break;
        }
    }
}

void assignSubscript(const void *invokedOnPointer, MaPLParameter index, MaPLParameter assignedValue) {
    scriptCallbacksString += "Assign subscript: pointer="+pointerToString(invokedOnPointer)+", index="+parameterToString(index)+", value="+parameterToString(assignedValue)+"\n";
    if (invokedOnPointer == &fakeGlobalObject) {
        switch (index.dataType) {
            case MaPLDataType_int32:
                if (index.int32Value == 0) {
                    fakeIntSubscript = assignedValue.int32Value;
                }
                break;
            case MaPLDataType_float32:
                if (index.float32Value == 0) {
                    fakeFloatSubscript = assignedValue.float32Value;
                }
                break;
            case MaPLDataType_string:
                if (!strcmp(index.stringValue, "0")) {
                    fakeStringSubscript = assignedValue.stringValue;
                }
                break;
            default: break;
        }
    }
}

void metadata(const char* metadataString) {
    scriptCallbacksString += "Debug metadata: '"+std::string(metadataString)+"'\n";
}

void debugLine(MaPLLineNumber lineNumber) {
    scriptCallbacksString += "Debug line: "+std::to_string(lineNumber)+"\n";
}

void debugVariableUpdate(const char *variableName, MaPLParameter newValue) {
    scriptCallbacksString += "Debug variable update: '"+std::string(variableName)+"' = "+parameterToString(newValue)+"\n";
}

void debugVariableDelete(const char *variableName) {
    scriptCallbacksString += "Debug variable delete: '"+std::string(variableName)+"'\n";
}

void error(MaPLRuntimeError error) {
    scriptEncounteredError = true;
}

bool pathHasExtension(const std::filesystem::path &path, const std::string &extension) {
    std::string pathExtension = path.extension();
    std::transform(pathExtension.begin(), pathExtension.end(), pathExtension.begin(), [](unsigned char c){
        return std::tolower(c);
    });
    return pathExtension == extension;
}

MaPLCompileResult runTests(const std::vector<std::filesystem::path> &scriptsUnderTest,
                           const std::map<std::filesystem::path, TestDirectoryContents> &directoryMap,
                           const MaPLCompileOptions &compileOptions) {
    MaPLCompileResult result = compileMaPL(scriptsUnderTest, compileOptions);
    
    if (result.errorMessages.size()) {
        printf("Failed to compile MaPL scripts:\n");
        for (const std::string &error : result.errorMessages) {
            printf("%s\n", error.c_str());
        }
        exit(1);
    }
    
    MaPLCallbacks testCallbacks = {
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
    
    // Compare the bytecode for each script against its expected bytecode.
    for (const auto&[path, bytecode] : result.compiledFiles) {
        TestDirectoryContents contents = directoryMap.at(path);
#if OUTPUT_EXPECTED_FILES
        std::ofstream bytecodeOutputStream(contents.bytecodePath);
        bytecodeOutputStream.write((char *)(&bytecode[0]), bytecode.size());
#else
        // Compare the length of expected bytecode.
        std::ifstream inputStream(contents.bytecodePath);
        if (!inputStream) {
            printf("Unable to find expected bytecode file '%s'.\n", contents.bytecodePath.c_str());
            exit(1);
        }
        inputStream.seekg(0, std::ios::end);
        size_t length = inputStream.tellg();
        if (length != bytecode.size()) {
            printf("Compiled bytecode is a different length than expected bytecode '%s'.\n", contents.bytecodePath.c_str());
            exit(1);
        }
        
        // Compare the contents of the expected bytecode.
        inputStream.seekg(0, std::ios::beg);
        u_int8_t expectedBytecode[length];
        inputStream.read((char *)expectedBytecode, length);
        if (memcmp(expectedBytecode, &bytecode[0], length)) {
            printf("Compiled bytecode has different content than expected bytecode '%s'.\n", contents.bytecodePath.c_str());
            exit(1);
        }
#endif
        
        // Reset all global test variables.
        scriptPrintString.clear();
        scriptCallbacksString.clear();
        scriptEncounteredError = false;
        fakeIntProperty = 0;
        fakeFloatProperty = 0;
        fakeIntSubscript = 0;
        fakeFloatSubscript = 0;
        
        // Run the script.
        executeMaPLScript(&bytecode[0], bytecode.size(), &testCallbacks);
        if (scriptEncounteredError) {
            printf("Script encountered a runtime error '%s'.\n", path.c_str());
#if !OUTPUT_EXPECTED_FILES
            exit(1);
#endif
        }

#if OUTPUT_EXPECTED_FILES
        std::ofstream printOutputStream(contents.printPath);
        printOutputStream << scriptPrintString;
#else
        // Compare the log of actual output to expected output.
        std::ifstream printInputStream(contents.printPath);
        if (!printInputStream) {
            printf("Unable to read expected print output: '%s'.\n", contents.printPath.c_str());
            exit(1);
        }
        std::stringstream printStringBuffer;
        printStringBuffer << printInputStream.rdbuf();
        std::string expectedPrintString = printStringBuffer.str();
        if (expectedPrintString != scriptPrintString) {
            printf("Expected print output did not match actual print output for '%s'.\n\nExpected:\n\n%s\nActual:\n\n%s\n", contents.printPath.c_str(), expectedPrintString.c_str(), scriptPrintString.c_str());
            exit(1);
        }
#endif
        
#if OUTPUT_EXPECTED_FILES
        std::ofstream callbackOutputStream(contents.callbacksPath);
        callbackOutputStream << scriptCallbacksString;
#else
        // Compare the log of actual callbacks to expected callbacks.
        std::ifstream callbackInputStream(contents.callbacksPath);
        if (!callbackInputStream) {
            printf("Unable to read expected callback output: '%s'.\n", contents.callbacksPath.c_str());
            exit(1);
        }
        std::stringstream callbackStringBuffer;
        callbackStringBuffer << callbackInputStream.rdbuf();
        std::string expectedCallbackString = callbackStringBuffer.str();
        if (expectedCallbackString != scriptCallbacksString) {
            printf("Expected callback output did not match actual callback output for '%s'.\n\nExpected:\n\n%s\nActual:\n\n%s\n", contents.callbacksPath.c_str(), expectedCallbackString.c_str(), scriptCallbacksString.c_str());
            exit(1);
        }
#endif
    }
    return result;
}

int main(int argc, const char * argv[]) {
    // Get the directory which contains all test scripts.
    if (argc != 3) {
        printf("MaPLTester expects two args:\n");
        printf("1- An absolute path to the root test directory.\n");
        printf("2- An absolute path to the root directory of scripts with intentional compile errors.\n");
        return 1;
    }
    std::filesystem::path testRootDirectory = argv[1];
    if (!testRootDirectory.is_absolute()) {
        printf("The following path must be expressed as an absolute path: '%s'.\n", argv[1]);
        return 1;
    }
    testRootDirectory = testRootDirectory.lexically_normal();
    
    std::filesystem::path errorRootDirectory = argv[2];
    if (!errorRootDirectory.is_absolute()) {
        printf("The following path must be expressed as an absolute path: '%s'.\n", argv[2]);
        return 1;
    }
    errorRootDirectory = errorRootDirectory.lexically_normal();
    
    // Each directory is expected to contain:
    // 1- script.mapl - The script under test.
    // 2- expectedBytecode.maplb - The expected compiled bytecode for script.mapl.
    // 3- expectedBytecodeDebug.maplb - The expected compiled bytecode for script.mapl when debug is enabled.
    // 4- expectedPrintOutput.txt - A log of all calls that the script makes to print output, listed one call per line.
    // 5- expectedCallbacks.txt - A log of all callbacks made to the host program, listed one call per line.
    // 6- expectedCallbacksDebug.txt - A log of all callbacks made to the host program when debug is enabled, listed one call per line.
    std::vector<std::filesystem::path> scriptsUnderTest;
    std::map<std::filesystem::path, TestDirectoryContents> nonDebugDirectoryMap;
    std::map<std::filesystem::path, TestDirectoryContents> debugDirectoryMap;
    for (const std::filesystem::directory_entry &file : std::filesystem::directory_iterator(testRootDirectory)) {
        if (file.is_directory()) {
            std::filesystem::path scriptPath = file.path() / "script.mapl";
            scriptsUnderTest.push_back(scriptPath);
            
            nonDebugDirectoryMap[scriptPath] = {
                file.path() / "expectedBytecode.maplb",
                file.path() / "expectedPrintOutput.txt",
                file.path() / "expectedCallbacks.txt",
            };
            
            debugDirectoryMap[scriptPath] = {
                file.path() / "expectedBytecodeDebug.maplb",
                file.path() / "expectedPrintOutput.txt",
                file.path() / "expectedCallbacksDebug.txt",
            };
        }
    }
    
    MaPLCompileOptions debugOptions = { true };
    MaPLCompileOptions nonDebugOptions = { false, "TestSymbols" };
    runTests(scriptsUnderTest, debugDirectoryMap, debugOptions);
    MaPLCompileResult result = runTests(scriptsUnderTest, nonDebugDirectoryMap, nonDebugOptions);
    
#if OUTPUT_EXPECTED_FILES
    if (result.symbolTable.size() > 0) {
        std::ofstream symbolOutputStream(testRootDirectory / "../MaPLTester/TestSymbols.h");
        symbolOutputStream << result.symbolTable;
    }
    printf("All expected files formatted.\n");
#else
    printf("All tests completed successfully.\n");
#endif
    
    for (const std::filesystem::directory_entry &file : std::filesystem::directory_iterator(errorRootDirectory)) {
        if (file.is_directory()) {
            continue;
        }
        std::filesystem::path scriptPath = file.path();
        std::string fileName = scriptPath.filename();
        if (fileName[0] == '.') {
            // MacOS is, annoyingly, always creating ".DS_Store".
            continue;
        }
        if (!pathHasExtension(scriptPath, ".mapl")) {
            printf("All files in the error directory are expected to have '.mapl' extension: '%s'.\n", fileName.c_str());
            return 1;
        }
        
        MaPLCompileResult errorResult = compileMaPL({scriptPath}, nonDebugOptions);
#if PRINT_ERROR_OUTPUT
        printf("Error output for '%s':\n", fileName.c_str());
        for (const std::string &errorMessage : errorResult.errorMessages) {
            printf("%s", errorMessage.c_str());
        }
        printf("\n");
#endif
        if (errorResult.errorMessages.size() == 0) {
            printf("Script at path '%s' was expected to produce a compile error but produced none.\n", fileName.c_str());
            return 1;
        }
    }
    printf("All error cases successfully logged errors.\n");
    
    return 0;
}

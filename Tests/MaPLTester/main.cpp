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

struct TestDirectoryContents {
    std::filesystem::path bytecodePath;
    std::filesystem::path printPath;
    std::filesystem::path callbacksPath;
};

std::string scriptPrintString;
std::string scriptCallbacksString;
bool scriptEncounteredError = false;

std::string pointerToString(const void *pointer) {
    // Memory address can (and almost certainly will be) different each time.
    // Only meaningful distinction is NULL vs non-NULL.
    return "(pointer)"+std::string(pointer ? "[memory address]" : "NULL");
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
        if (i == argc-1) {
            scriptCallbacksString += "\n";
        } else {
            scriptCallbacksString += ", ";
        }
    }
    if (!invokedOnPointer && functionSymbol == TestSymbols_GLOBAL_print_string) {
        scriptPrintString += argv[0].stringValue;
        scriptPrintString += "\n";
    }
    
    return MaPLUninitialized();
}

MaPLParameter invokeSubscript(const void *invokedOnPointer, MaPLParameter index) {
    scriptCallbacksString += "Invoke subscript: pointer="+pointerToString(invokedOnPointer)+", index="+parameterToString(index)+"\n";
    
    return MaPLUninitialized();
}

void assignProperty(const void *invokedOnPointer, MaPLSymbol propertySymbol, MaPLParameter assignedValue) {
    scriptCallbacksString += "Assign property: pointer="+pointerToString(invokedOnPointer)+", symbol="+std::to_string(propertySymbol)+", value="+parameterToString(assignedValue)+"\n";
    
}

void assignSubscript(const void *invokedOnPointer, MaPLParameter index, MaPLParameter assignedValue) {
    scriptCallbacksString += "Assign subscript: pointer="+pointerToString(invokedOnPointer)+", index="+parameterToString(index)+", value="+parameterToString(assignedValue)+"\n";
    
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

void error(void) {
    scriptEncounteredError = true;
}

void runTests(const std::vector<std::filesystem::path> &scriptsUnderTest,
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
        
        // Compare the length of expected bytecode.
        std::ifstream inputStream(contents.bytecodePath);
        if (!inputStream) {
            printf("Unable to find expected bytecode file '%s'\n", contents.bytecodePath.c_str());
            exit(1);
        }
        inputStream.seekg(0, std::ios::end);
        size_t length = inputStream.tellg();
        if (length != bytecode.size()) {
            printf("Compiled bytecode is a different length than expected bytecode '%s'\n", contents.bytecodePath.c_str());
            exit(1);
        }
        
        // Compare the contents of the expected bytecode.
        inputStream.seekg(0, std::ios::beg);
        u_int8_t expectedBytecode[length];
        inputStream.read((char *)expectedBytecode, length);
        if (memcmp(expectedBytecode, &bytecode[0], length)) {
            printf("Compiled bytecode has different content than expected bytecode '%s'\n", contents.bytecodePath.c_str());
            exit(1);
        }
        
        // Run the script.
        scriptPrintString.clear();
        scriptCallbacksString.clear();
        scriptEncounteredError = false;
        executeMaPLScript(&bytecode[0], bytecode.size(), &testCallbacks);
        
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
    }
}

int main(int argc, const char * argv[]) {
    // Get the directory which contains all test scripts.
    if (argc != 2) {
        printf("MaPLTester expects a single arg: an absolute path to the root test directory.\n");
        return 1;
    }
    std::filesystem::path rootDir = argv[1];
    if (!rootDir.is_absolute()) {
        printf("The root path must be an absolute path.\n");
        return 1;
    }
    rootDir = rootDir.lexically_normal();
    
    // Each directory is expected to contain:
    // 1- script.mapl - The script under test.
    // 2- expectedBytecode.maplb - The expected bytecode output for script.mapl.
    // 3- expectedBytecodeDebug.maplb - The expected bytecode output for script.mapl when debug is enabled.
    // 4- expectedPrintOutput.txt - Calls that the script makes to print output, listed one call per line.
    // 5- expectedCallbacks.txt - Describes the callbacks made to the host program, listed one call per line.
    // 6- expectedCallbacksDebug.txt - Describes the callbacks made to the host program when debug is enabled, listed one call per line.
    std::vector<std::filesystem::path> scriptsUnderTest;
    std::map<std::filesystem::path, TestDirectoryContents> nonDebugDirectoryMap;
    std::map<std::filesystem::path, TestDirectoryContents> debugDirectoryMap;
    for (const std::filesystem::directory_entry &file : std::filesystem::directory_iterator(rootDir)) {
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
    
    MaPLCompileOptions nonDebugOptions = { false };
    MaPLCompileOptions debugOptions = { true };
    
    runTests(scriptsUnderTest, nonDebugDirectoryMap, nonDebugOptions);
    runTests(scriptsUnderTest, debugDirectoryMap, debugOptions);
    
    printf("All tests completed successfully.\n");
    return 0;
}

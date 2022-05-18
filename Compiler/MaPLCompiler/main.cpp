//
//  main.cpp
//  MaPLCompiler
//
//  Created by Brad Bambara on 3/27/21.
//

#include <stdio.h>
#include <fstream>
#include "MaPLCompiler.h"

enum ArgumentExpectation {
    ArgumentExpectation_InputPath,
    ArgumentExpectation_OutputPath,
    ArgumentExpectation_SymbolTablePath,
};

bool pathHasExtension(const std::filesystem::path &path, const std::string &extension) {
    std::string pathExtension = path.extension();
    std::transform(pathExtension.begin(), pathExtension.end(), pathExtension.begin(), [](unsigned char c){
        return std::tolower(c);
    });
    return pathExtension == extension;
}

int main(int argc, const char ** argv) {
    if (argc < 2) {
        printf("No arguments specified. Specify one or more file paths along with an output path for the symbol table.\n");
        printf("Example usage: MaPLCompiler /path/to/file.mapl -o /path/to/output.maplb -s /path/to/symbol/table.h\n");
        printf("Specify the --debug flag to include debug info in the bytecode. This option increases bloat for bytecode size and runtime speed.\n");
        return 1;
    }
    
    // Interpret the command line args.
    std::vector<std::filesystem::path> scriptPaths;
    std::map<std::filesystem::path, std::filesystem::path> outputFileMap;
    std::filesystem::path symbolOutputPath;
    bool canTakeOutputFile = false;
    MaPLCompileOptions options{ false };
    ArgumentExpectation expectation = ArgumentExpectation_InputPath;
    for(int i = 1; i < argc; i++) {
        std::string arg = argv[i];
        
        // Check if this is a flag (which might signal the expecation of a subsequent path).
        bool isFlag = false;
        ArgumentExpectation previousExpectation = expectation;
        if (arg == "-o") {
            // This is specifying that the next arg is an output path for the previous file.
            expectation = ArgumentExpectation_OutputPath;
            isFlag = true;
        } else if (arg == "-s") {
            expectation = ArgumentExpectation_SymbolTablePath;
            isFlag = true;
        } else if (arg == "--debug") {
            options.includeDebugBytes = true;
            isFlag = true;
        }
        if (isFlag) {
            if (previousExpectation != ArgumentExpectation_InputPath) {
                printf("The flag '%s' was specified, but not followed by any path.\n", argv[i-1]);
                return 1;
            }
            continue;
        }
        
        // Safe to assume this arg is a file path. Normalize the path.
        std::filesystem::path argPath(arg);
        argPath = std::filesystem::absolute(argPath).lexically_normal();
        
        // Store this path as whatever type of information is expected based on previous flags.
        switch (expectation) {
            case ArgumentExpectation_InputPath:
                if (!pathHasExtension(argPath, ".mapl")) {
                    printf("The script file path '%s' must have a '.mapl' file extension.\n", argv[i]);
                    return 1;
                }
                scriptPaths.push_back(argPath);
                canTakeOutputFile = true;
                break;
            case ArgumentExpectation_OutputPath:
                if (!pathHasExtension(argPath, ".maplb")) {
                    printf("The output path '%s' must have a '.maplb' file extension.\n", argv[i]);
                    return 1;
                }
                if (canTakeOutputFile) {
                    // Associate this output path with the most recent script path.
                    outputFileMap[scriptPaths[scriptPaths.size()-1]] = argPath;
                } else {
                    printf("Output file '%s' is specified before any file.\n", argv[i]);
                    return 1;
                }
                break;
            case ArgumentExpectation_SymbolTablePath:
                if (!pathHasExtension(argPath, ".h")) {
                    printf("The symbol table path '%s' must have a '.h' file extension.\n", argv[i]);
                    return 1;
                }
                symbolOutputPath = argPath;
                break;
        }
        if (expectation != ArgumentExpectation_InputPath) {
            canTakeOutputFile = false;
        }
        expectation = ArgumentExpectation_InputPath;
    }
    if (expectation != ArgumentExpectation_InputPath) {
        printf("The flag '%s' was specified, but not followed by any path.\n", argv[argc-1]);
        return 1;
    }
    if (scriptPaths.size() == 0) {
        printf("No script file paths were specified.\n");
        return 1;
    }
    if (symbolOutputPath.empty()) {
        printf("No symbol table path was specified.\n");
        return 1;
    }
    
    // Assign the filename of the symbol output path as the prefix.
    options.symbolsPrefix = symbolOutputPath.filename();
    options.symbolsPrefix = options.symbolsPrefix.substr(0, options.symbolsPrefix.length()-2);
    
    MaPLCompileResult result = compileMaPL(scriptPaths, options);
    
    // Check for errors and write to console.
    if (result.errorMessages.size()) {
        for (const std::string &errorString : result.errorMessages) {
            fputs(errorString.c_str(), stderr);
        }
        return 1;
    }
    
    // Write symbol table to file.
    std::ofstream symbolTableOutputStream(symbolOutputPath);
    symbolTableOutputStream << result.symbolTable;
    
    // Output each compiled script to file.
    for (const auto&[scriptPath, bytecode] : result.compiledFiles) {
        std::ofstream bytecodeOutputStream(outputFileMap[scriptPath]);
        bytecodeOutputStream.write((char *)&(bytecode[0]), bytecode.size());
    }
    
    printf("All MaPL sources compiled successfully.\n");
    
    return 0;
}

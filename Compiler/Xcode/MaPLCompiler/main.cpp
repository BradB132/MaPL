//
//  main.cpp
//  MaPLCompiler
//
//  Created by Brad Bambara on 3/27/21.
//

#include <stdio.h>
#include "MaPLFileCache.h"
#include "MaPLFile.h"
#include "MaPLBuffer.h"
#include "MaPLVariableStack.h"

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
        printf("Example usage: MaPLCompiler /absolute/path/to/file.mapl -o /absolute/path/to/output.maplb -s /absolute/path/to/symbol/table.h\n");
        return 1;
    }
    
    // Interpret the command line args.
    MaPLFileCache fileCache;
    std::vector<MaPLFile *> files;
    MaPLFile *currentFile = NULL;
    std::filesystem::path symbolOutputPath;
    ArgumentExpectation expectation = ArgumentExpectation_InputPath;
    for(int i = 1; i < argc; i++) {
        // Check if this is a flag signalling the expecation of a subsequent path.
        std::string arg = argv[i];
        bool isFlag = false;
        ArgumentExpectation previousExpectation = expectation;
        if (arg == "-o") {
            // This is specifying that the next arg is an output path for the previous file.
            expectation = ArgumentExpectation_OutputPath;
            isFlag = true;
        } else if (arg == "-s") {
            expectation = ArgumentExpectation_SymbolTablePath;
            isFlag = true;
        }
        if (isFlag) {
            if (previousExpectation != ArgumentExpectation_InputPath) {
                printf("The flag '%s' was specified, but not followed by any path.\n", argv[i-1]);
                exit(1);
            }
            continue;
        }
        
        // Safe to assume this arg is a file path. Normalize the path.
        std::filesystem::path argPath = arg;
        if (!argPath.is_absolute()) {
            printf("Path '%s' must be specified as an absolute path.\n", argv[i]);
            exit(1);
        }
        argPath = argPath.lexically_normal();
        
        // Store this path as whatever type of information is expected based on previous flags.
        switch (expectation) {
            case ArgumentExpectation_InputPath:
                if (!pathHasExtension(argPath, ".mapl")) {
                    printf("The script file path '%s' must have a '.mapl' file extension.\n", argv[i]);
                    exit(1);
                }
                currentFile = fileCache.fileForNormalizedPath(argPath);
                files.push_back(currentFile);
                break;
            case ArgumentExpectation_OutputPath:
                if (!pathHasExtension(argPath, ".maplb")) {
                    printf("The output path '%s' must have a '.maplb' file extension.\n", argv[i]);
                    exit(1);
                }
                if (currentFile) {
                    currentFile->setNormalizedOutputPath(argPath);
                } else {
                    printf("Output file '%s' is specified before any file.\n", argv[i]);
                    exit(1);
                }
                break;
            case ArgumentExpectation_SymbolTablePath:
                if (!pathHasExtension(argPath, ".h")) {
                    printf("The symbol table path '%s' must have a '.h' file extension.\n", argv[i]);
                    exit(1);
                }
                symbolOutputPath = argPath;
                break;
        }
        if (expectation != ArgumentExpectation_InputPath) {
            currentFile = NULL;
        }
        expectation = ArgumentExpectation_InputPath;
    }
    if (expectation != ArgumentExpectation_InputPath) {
        printf("The flag '%s' was specified, but not followed by any path.\n", argv[argc-1]);
        exit(1);
    }
    if (files.size() == 0) {
        printf("No script file paths were specified.\n");
        exit(1);
    }
    if (symbolOutputPath.empty()) {
        printf("No symbol table path was specified.\n");
        exit(1);
    }
    
    // Check for errors and write to console.
    bool hadErrors = false;
    for (MaPLFile *file : files) {
        std::vector<std::string> errors = file->getErrors();
        if (errors.size()) {
            hadErrors = true;
            for (const std::string &errorString : errors) {
                fputs(errorString.c_str(), stderr);
            }
        }
    }
    if (hadErrors) {
        exit(1);
    }
    
    // Generate the symbol table and write to file.
    std::map<std::string, MaPLSymbol> symbolTable = symbolTableForFiles(files);
    std::string enumName = symbolOutputPath.filename();
    enumName = enumName.substr(0, enumName.length()-2);
    std::string formattedSymbolTable = "enum "+enumName+" {\n";
    for (const auto&[descriptor, symbol] : symbolTable) {
        formattedSymbolTable += "    "+enumName+"_"+descriptor+" = "+std::to_string(symbol)+",\n";
    }
    formattedSymbolTable += "};\n";
    std::ofstream symbolTableOutputStream(symbolOutputPath);
    symbolTableOutputStream << formattedSymbolTable;
    
    // Output each script to file.
    for (MaPLFile *file : files) {
        std::ofstream bytecodeOutputStream(file->getNormalizedOutputPath());
        
        MaPLMemoryAddress stackHeight = file->getVariableStack()->getMaximumMemoryUsed();
        bytecodeOutputStream.write((char *)(&stackHeight), sizeof(stackHeight));
        
        MaPLBuffer *bytecode = file->getBytecode();
        bytecode->resolveSymbolsWithTable(symbolTable);
        bytecodeOutputStream.write((char *)bytecode->getBytes(), bytecode->getByteCount());
    }
    
    printf("All MaPL sources compiled successfully.\n");
    
    return 0;
}

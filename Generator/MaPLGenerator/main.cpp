//
//  main.cpp
//  MaPLGenerator
//
//  Created by Brad Bambara on 4/25/22.
//

#include <iostream>
#include "MaPLCompiler.h"
#include <libxml/parser.h>

#include <filesystem>
#include <string>
#include <vector>

enum ArgumentExpectation {
    ArgumentExpectation_None,
    ArgumentExpectation_GeneratedCodeOutputPath,
    ArgumentExpectation_DataOutputPath,
};

void printUsage() {
    printf("Usage: MaPLGenerator -g generated/code/output/dir -d generated/data/output/dir [paths]\nPaths are expected to contain:\n- At least one '.easl' schema file.\n- At least one '.mapl' script file.\n- Optional '.xml' data file(s).\n");
}

void confirmNoExpectation(ArgumentExpectation expectation, const char * argv[], int i) {
    if (expectation != ArgumentExpectation_None) {
        printf("Missing an output path after flag '%s'.\n", argv[i-1]);
        printUsage();
        exit(1);
    }
}

int main(int argc, const char * argv[]) {
    
    // TODO: Delete this once API is settled.
//    MaPLCompileOptions options;
//    MaPLCompileResult result = compileMaPL({ "" }, options);
//    printf("%s\n", result.symbolTable.c_str());
//    return 1;
    
    std::vector<std::filesystem::path> schemaPaths;
    std::vector<std::filesystem::path> scriptPaths;
    std::vector<std::filesystem::path> xmlPaths;
    std::filesystem::path generatedCodeOutputPath;
    std::filesystem::path dataOutputPath;
    std::vector<std::string> flags;
    
    ArgumentExpectation expectation = ArgumentExpectation_None;
    for (int i = 1; i < argc; i++) {
        std::string argString = argv[i];
        if (argString.size() >= 3 && argString[0] == '-' && argString[1] == '-') {
            // This arg is a flag (beginning with "--").
            confirmNoExpectation(expectation, argv, i);
            flags.push_back(argString.substr(2));
            continue;
        }
        
        if (argString == "-g") {
            confirmNoExpectation(expectation, argv, i);
            expectation = ArgumentExpectation_GeneratedCodeOutputPath;
            continue;
        }
        if (argString == "-d") {
            confirmNoExpectation(expectation, argv, i);
            expectation = ArgumentExpectation_DataOutputPath;
            continue;
        }
        
        // This arg is not a flag, assume it's a path.
        std::filesystem::path argPath(argString);
        argPath = std::filesystem::absolute(argPath).lexically_normal();
        if (expectation != ArgumentExpectation_None) {
            if (!argPath.extension().empty()) {
                printf("Path '%s' follows flag '%s' and must point to an output directory.\n", argv[i], argv[i-1]);
                printUsage();
                return 1;
            }
            if (expectation == ArgumentExpectation_GeneratedCodeOutputPath) {
                generatedCodeOutputPath = argPath;
            } else if (expectation == ArgumentExpectation_DataOutputPath) {
                dataOutputPath = argPath;
            }
            expectation = ArgumentExpectation_None;
            continue;
        }
        
        std::string pathExtension = argPath.extension();
        std::transform(pathExtension.begin(), pathExtension.end(), pathExtension.begin(), [](unsigned char c){
            return std::tolower(c);
        });
        
        if (pathExtension == ".easl") {
            schemaPaths.push_back(argPath);
        } else if (pathExtension == ".mapl") {
            scriptPaths.push_back(argPath);
        } else if (pathExtension == ".xml") {
            xmlPaths.push_back(argPath);
        } else {
            printf("File '%s' is unexpected type.\n", argv[i]);
            printUsage();
            return 1;
        }
    }
    confirmNoExpectation(expectation, argv, argc);
    if (schemaPaths.empty()) {
        printf("Expected at least one '.easl' schema file.\n");
        printUsage();
        return 1;
    }
    if (scriptPaths.empty()) {
        printf("Expected at least one '.mapl' script file.\n");
        printUsage();
        return 1;
    }
    if (generatedCodeOutputPath.empty()) {
        printf("Expected an output directory for generated code (preceded by a -g flag).\n");
        printUsage();
        return 1;
    }
    if (!xmlPaths.empty() && dataOutputPath.empty()) {
        printf("Expected an output directory for data (preceded by a -d flag).\n");
        printUsage();
        return 1;
    }
    
    // TODO: Init an EaSL parser.
    // TODO: Parse with libxml?
    
    return 0;
}

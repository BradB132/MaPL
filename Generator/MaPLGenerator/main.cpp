//
//  main.cpp
//  MaPLGenerator
//
//  Created by Brad Bambara on 4/25/22.
//

#include <filesystem>
#include <string>
#include <vector>

#include "EaSLHandler.h"
#include "MaPLHandler.h"
#include <libxml/parser.h>

void printUsage() {
    printf("Usage: MaPLGenerator [paths] and [flags]\n  Paths are expected to contain:\n    - At least one '.easl' schema file.\n    - At least one '.mapl' script file.\n    - Optional '.xml' data file(s).\n  Flags are prepended with '--' and contain a mapping from name to value. For example, '--myVar=myValue'.\n");
}

int main(int argc, const char * argv[]) {
    
    // TODO: Delete this once API is settled.
//    MaPLCompileOptions options;
//    MaPLCompileResult result = compileMaPL({ "" }, options);
//    printf("%s\n", result.symbolTable.c_str());
//    return 1;
    
    MaPLGeneratorContext context;
    std::vector<std::filesystem::path> schemaPaths;
    std::vector<std::filesystem::path> scriptPaths;
    std::vector<std::filesystem::path> xmlPaths;
    
    for (int i = 1; i < argc; i++) {
        std::string argString = argv[i];
        if (argString.size() > 2 && argString[0] == '-' && argString[1] == '-') {
            // This arg is a flag (beginning with "--").
            std::string keyAndValue = argString.substr(2);
            size_t equalsIndex = keyAndValue.find("=");
            std::pair<std::string, std::string> flag = { keyAndValue.substr(0,equalsIndex), keyAndValue.substr(equalsIndex+1) };
            context.flags.push_back(flag);
            continue;
        }
        
        // This arg is not a flag, assume it's a path.
        std::filesystem::path argPath(argString);
        argPath = std::filesystem::absolute(argPath).lexically_normal();
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
    
    context.schemas = schemasForPaths(schemaPaths);
    
    
    // TODO: Parse with libxml.
    // TODO: Invoke MaPLHandler.
    
    return 0;
}

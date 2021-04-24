//
//  main.cpp
//  MaPLCompiler
//
//  Created by Brad Bambara on 3/27/21.
//

#include <iostream>

#include "antlr4-runtime.h"
#include "MaPLLexer.h"
#include "MaPLParser.h"
#include "MaPLFile.h"
#include <vector>

using namespace antlr4;

int main(int argc, const char ** argv) {
    // Make sure that script paths were specified.
    if (argc < 2) {
        printf("No paths specified. Pass in the paths of the script(s) as arguments.\n");
        return 1;
    }
    
    // Gather a collection of all script files from the args.
    std::vector<MaPLFile> scriptFiles;
    for(int i = 1; i < argc; i++) {
        scriptFiles.push_back(MaPLFile(argv[i]));
    }
    
    // TODO: test code, delete this line
    scriptFiles[0].readRawScriptFromDisk();
    
    ANTLRInputStream input(scriptFiles[0].getRawScriptText());
    MaPLLexer lexer(&input);
    CommonTokenStream tokens(&lexer);
    MaPLParser parser(&tokens);
    
    MaPLParser::ProgramContext *rootNode = parser.program();

    return 0;
}

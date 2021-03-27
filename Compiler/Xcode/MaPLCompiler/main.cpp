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

using namespace antlr4;

int main(int , const char **) {
    ANTLRInputStream input(""); // TODO: add the actual input.
    MaPLLexer lexer(&input);
    CommonTokenStream tokens(&lexer);

    MaPLParser parser(&tokens);
    MaPLParser::ProgramContext *rootNode = parser.program();

    return 0;
}

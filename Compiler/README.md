# How to Build the Compiler

### Generate the Parser

The translation from human-readable MaPL scripts to machine-runnable bytecode happens in 3 steps:

1. Lexing (aka Tokenizing)
2. Parsing
3. Compiling

MaPL's compiler relies on [ANTLR](https://www.antlr.org/)-generated code for the first 2 steps of the translation. To build the compiler, the ANTLR lexer and parser first need to be generated. Generate it with the following:
```
MaPL/Compiler/build_parser.sh C++
```
Make sure you include the `C++` argument, or the script will build the Java version of the parser that is used for testing. The script should output the C++ code for the lexer and parser:

* MaPL/Compiler/c++/MaPLLexer.h
* MaPL/Compiler/c++/MaPLLexer.cpp
* MaPL/Compiler/c++/MaPLParser.h
* MaPL/Compiler/c++/MaPLParser.cpp

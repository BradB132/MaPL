# MaPL Compiler

### How to Build the Compiler

The compiler was developed on macOS using Xcode. The easiest way to build is to use the Xcode project, which will automatically do everything described in this section below.

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

### High-level goal
The goal of the MaPL compiler is to absorb as much of the complexity and computational cost of interpreting scripts as possible, and to surface errors at compile time rather than script execution time.

### Optimizations

* **Constant folding** - Expressions which are entirely constant will be resolved at compile time. For example, the statement `float32 two_pi = 3.14*2` would be rewritten as `float32 two_pi = 6.28`.
* **Strength reduction** - The compiler rewrites some expressions in a way that is computationally cheaper. For example, `float32 y = x/2.0` is rewritten as `float32 y = x*0.5`, and `int32 y = x*4` is rewritten as `int32 y = x << 2`.
* **Dead code stripping** - The compiler will omit logic that is obviously unused. For example, `int32 x = true ? 1 : 2` is rewritten as `int32 x = 1`. Similarly if a loop is declared with `while true { /* logic */ }` it will be rewritten as an infinite loop resembling `label "start"; /* logic */; goto "start";`. Note the previous example is more to illustrate an idea, there is no `label` or `goto` in MaPL.

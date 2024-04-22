# MaPL Compiler

### High-level goal
The goal of the MaPL compiler is to absorb as much of the complexity and computational cost of interpreting scripts as possible, and to surface errors at compile time rather than script execution time.

### How to Build the Compiler

The easiest way to build `MaPLCompiler` is with cmake:

```
cd MaPL/Compiler
cmake .
make

```
This will generate an executable in the `MaPL/Compiler/Executable` directory.

### Optimizations

The MaPL compiler attempts to make optimizations where it can. More complex optimizations that require broad analysis of multiple code paths etc is not implemented. However, the following low-hanging fruit is currently implemented:

**Constant folding** - Expressions which are entirely constant will be resolved at compile time. For example, the statement `float32 two_pi = 3.14*2` would be rewritten as `float32 two_pi = 6.28`.

**Strength reduction** - The compiler rewrites some expressions in a way that is computationally cheaper. For example, `float32 y = x/2.0` is rewritten as `float32 y = x*0.5`, and `int32 y = x*4` is rewritten as `int32 y = x << 2`.

**Dead code stripping** - The compiler will omit logic that is obviously unused. For example, `int32 x = true ? 1 : 2` is rewritten as `int32 x = 1`. Similarly if a loop is declared with...
```
while true {
    if /* conditional logic */ {
        break;
    }
}
```
...it will be rewritten as an infinite loop resembling...
```
// Execution starts here.
if /* conditional logic */ {
    // Execution jumps to end.
}
// Execution moves back to top.
// Execution ends here.
```

### ANTLR4 Dependency

The translation from human-readable MaPL scripts to machine-runnable bytecode happens in 3 steps:

1. Lexing (aka Tokenizing)
2. Parsing
3. Compiling

MaPL's compiler relies on [ANTLR](https://www.antlr.org/)-generated code for the first 2 steps of the translation. All code from ANTLR is already generated, and included in the `generated_c++` directory. To regenerate the the C++ code for these steps, run the following:
```
MaPL/Compiler/generate_parser.sh C++
```
Make sure you include the `C++` argument, or the script will build the Java version of the parser that is used for testing. The script is dependent on `antlr4-tools` and will attempt to install it if needed. The script should output the C++ code for the lexer and parser:

* `./generated_c++/MaPLLexer.h`
* `./generated_c++/MaPLLexer.cpp`
* `./generated_c++/MaPLParser.h`
* `./generated_c++/MaPLParser.cpp`

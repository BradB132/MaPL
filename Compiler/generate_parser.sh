#!/bin/bash

# Move into the directory of this script.
cd "$(dirname "$0")"

# Make sure ANTLR4 is installed.
export PATH=$PATH:~/Library/Python/3.9/bin
python3 -m pip install antlr4-tools

ARG1=$(echo "${1}" | awk '{print tolower($0)}')
if [ "${ARG1}" == "c++" ]; then
    mkdir -p "./generated_c++"

    echo "Generating C++ code for the parser..."
    antlr4 ./MaPLLexer.g4 ./MaPLParser.g4 -o ./generated_c++ -Dlanguage=Cpp -no-listener
    
    # Clean up extraneous files.
    rm ./generated_c++/*.interp
    rm ./generated_c++/*.tokens
    
else
    echo "Opening parser output in GUI..."
    antlr4-parse ./MaPLLexer.g4 ./MaPLParser.g4 program -gui ../Tests/Scripts/TestAPI.mapl
fi

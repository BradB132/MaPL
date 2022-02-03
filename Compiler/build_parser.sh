#!/bin/bash

# Move into the directory of this script.
cd "$(dirname "$0")"

# Make sure we know where the ANTLR jar is.
ANTLR_PATH="/Applications/antlr-4.9.3-complete.jar"
if [ ! -f "${ANTLR_PATH}" ]; then
    echo "Couldn't find ANTLR. This script assumes your antlr.jar is located at '${ANTLR_PATH}'. If not, edit this script to fix the path."
    return 1
fi

# Make sure Java is installed.
if [ $(which java) == "" ]; then
    echo "Couldn't find Java. Make sure 'java' is installed."
    return 1
fi

# Make sure JAVA_HOME environment var is set.
if [ "${JAVA_HOME}" == "" ]; then
    export JAVA_HOME=$(/usr/libexec/java_home)
fi

# Make sure ANTLR_PATH is in CLASSPATH.
if [[ "${CLASSPATH}" != *"${ANTLR_PATH}"* ]]; then
    export CLASSPATH=".:${ANTLR_PATH}:${CLASSPATH}"
fi

ARG1=$(echo "${1}" | awk '{print tolower($0)}')
if [ "${ARG1}" == "c++" ]; then
    mkdir -p "./c++"

    echo "Generating C++ code for the parser..."
    java -jar "${ANTLR_PATH}" MaPL.g4 -o ./c++ -Dlanguage=Cpp -no-listener
    
    # Clean up extraneous files.
    rm ./c++/*.interp
    rm ./c++/*.tokens
    
else
    mkdir -p "./java"
    
    echo "Generating Java code for the parser..."
    java -jar "${ANTLR_PATH}" MaPL.g4 -o ./java
    
    echo "Compiling parser code..."
    cd ./java
    javac *.java
    
    # Clean up extraneous files.
    rm *.interp
    rm *.tokens
    
    echo "Opening parser output in GUI..."
    java org.antlr.v4.gui.TestRig MaPL program -gui ../tests/testInput.mapl
fi

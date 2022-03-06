//
//  MaPLVariableStack.cpp
//  MaPLCompiler
//
//  Created by Brad Bambara on 3/3/22.
//

#include "MaPLVariableStack.h"

MaPLVariableStack::MaPLVariableStack() :
    _maximumMemoryUsed(0)
{
    // This stack should always have at least one frame.
    push();
}

void MaPLVariableStack::push() {
    std::unordered_map<std::string, MaPLVariable> pushedFrame;
    _stack.push_back(pushedFrame);
}

void MaPLVariableStack::pop() {
    _stack.pop_back();
}

MaPL_Index MaPLVariableStack::maximumMemoryUsed() {
    return _maximumMemoryUsed;
}

bool MaPLVariableStack::declareVariable(std::string variableName, MaPLVariable variable) {
    MaPL_Index variableSize = byteSizeOfType(variable.type.type);
    if (!variableSize) {
        return false;
    }
    
    // Calculate byte offset for this variable by summing the size of the existing variables.
    MaPL_Index currentOffset = 0;
    for (std::unordered_map<std::string, MaPLVariable> frame : _stack) {
        for (std::pair<std::string, MaPLVariable> pair : frame) {
            currentOffset += byteSizeOfType(pair.second.type.type);
        }
    }
    
    // Adjust offsets.
    variable.byteOffset = currentOffset;
    MaPL_Index insertedOffset = currentOffset+variableSize;
    if (insertedOffset > _maximumMemoryUsed) {
        _maximumMemoryUsed = insertedOffset;
    }
    
    // Insert the variable into the stack.
    _stack[_stack.size()-1][variableName] = variable;
    return true;
}

MaPLVariable MaPLVariableStack::getVariable(std::string variableName) {
    for (std::unordered_map<std::string, MaPLVariable> frame : _stack) {
        for (std::pair<std::string, MaPLVariable> pair : frame) {
            if (pair.first == variableName) {
                return pair.second;
            }
        }
    }
    return { MaPLPrimitiveType_InvalidType, 0 };
}

std::unordered_map<std::string, MaPLVariable> MaPLVariableStack::getGlobalVariables() {
    return _stack[0];
}

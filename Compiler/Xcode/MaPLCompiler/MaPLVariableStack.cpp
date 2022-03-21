//
//  MaPLVariableStack.cpp
//  MaPLCompiler
//
//  Created by Brad Bambara on 3/3/22.
//

#include "MaPLVariableStack.h"
#include "MaPLFile.h"

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

MaPL_MemoryAddress MaPLVariableStack::getMaximumMemoryUsed() {
    return _maximumMemoryUsed;
}

bool MaPLVariableStack::declareVariable(const std::string &variableName, MaPLVariable variable) {
    MaPL_MemoryAddress variableSize = byteSizeOfType(variable.type.primitiveType);
    if (!variableSize) {
        variable.file->logError(variable.token, "Failure declaring variable '"+variableName+"' with ambiguous type.");
        return false;
    }
    
    MaPLVariable existingVariable = getVariable(variableName);
    if (existingVariable.type.primitiveType != MaPLPrimitiveType_Uninitialized) {
        variable.file->logError(variable.token, "Variable '"+variableName+"' conflicts with a previously-declared variable of the same name.");
        existingVariable.file->logError(existingVariable.token, "Variable '"+variableName+"' later comes into conflict with a variable of the same name.");
        return false;
    }
    
    // Calculate byte offset for this variable by summing the size of the existing variables.
    MaPL_MemoryAddress currentOffset = 0;
    for (std::unordered_map<std::string, MaPLVariable> frame : _stack) {
        for (std::pair<std::string, MaPLVariable> pair : frame) {
            currentOffset += byteSizeOfType(pair.second.type.primitiveType);
        }
    }
    
    // Adjust offsets.
    variable.byteOffset = currentOffset;
    MaPL_MemoryAddress insertedOffset = currentOffset+variableSize;
    if (insertedOffset > _maximumMemoryUsed) {
        _maximumMemoryUsed = insertedOffset;
    }
    
    // Insert the variable into the stack.
    _stack[_stack.size()-1][variableName] = variable;
    return true;
}

MaPLVariable MaPLVariableStack::getVariable(const std::string &variableName) {
    for (std::unordered_map<std::string, MaPLVariable> frame : _stack) {
        for (std::pair<std::string, MaPLVariable> pair : frame) {
            if (pair.first == variableName) {
                return pair.second;
            }
        }
    }
    return { { MaPLPrimitiveType_Uninitialized } };
}

std::unordered_map<std::string, MaPLVariable> MaPLVariableStack::getGlobalVariables() {
    return _stack[0];
}

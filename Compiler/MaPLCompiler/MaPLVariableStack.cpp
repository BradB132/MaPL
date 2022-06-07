//
//  MaPLVariableStack.cpp
//  MaPLCompiler
//
//  Created by Brad Bambara on 3/3/22.
//

#include "MaPLVariableStack.h"
#include "MaPLFile.h"

MaPLVariableStack::MaPLVariableStack() :
    _maximumPrimitiveMemoryUsed(0),
    _maximumAllocatedMemoryUsed(0)
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

MaPLMemoryAddress MaPLVariableStack::getMaximumPrimitiveMemoryUsed() {
    return _maximumPrimitiveMemoryUsed;
}

MaPLMemoryAddress MaPLVariableStack::getMaximumAllocatedMemoryUsed() {
    return _maximumAllocatedMemoryUsed;
}

bool MaPLVariableStack::declareVariable(const std::string &variableName, MaPLVariable variable) {
    // Log an error if this variable was already declared elsewhere.
    MaPLVariable existingVariable = getVariable(variableName);
    if (existingVariable.type.primitiveType != MaPLPrimitiveType_Uninitialized) {
        variable.file->logError(variable.token, "Variable '"+variableName+"' conflicts with a previously-declared variable of the same name.");
        existingVariable.file->logError(existingVariable.token, "Variable '"+variableName+"' later comes into conflict with a variable of the same name.");
        return false;
    }
    
    // Strings are a special case because they might need to allocate additional memory. This is unlike all other
    // MaPL types which are "POD" primitives that can be held in memory without needing to be freed later.
    if (variable.type.primitiveType == MaPLPrimitiveType_String) {
        // String memory addresses don't take into account the size of a string pointer. They're an index into a char* array.
        // Calculate byte offset for this variable by summing the size of the existing allocated variables.
        MaPLMemoryAddress currentOffset = 0;
        for (const std::unordered_map<std::string, MaPLVariable> &frame : _stack) {
            for (const auto&[name, variable] : frame) {
                if (variable.type.primitiveType == MaPLPrimitiveType_String) {
                    currentOffset++;
                }
            }
        }
        
        // Calculate the highest memory usage for allocated variables.
        variable.memoryAddress = currentOffset;
        MaPLMemoryAddress currentMemoryUsed = currentOffset+1;
        if (currentMemoryUsed > _maximumAllocatedMemoryUsed) {
            _maximumAllocatedMemoryUsed = currentMemoryUsed;
        }
    } else {
        // Primitive memory addresses must take into account the byte size of the primitive. They're in index into a byte array.
        MaPLMemoryAddress variableSize = byteSizeOfType(variable.type.primitiveType);
        if (!variableSize) {
            variable.file->logError(variable.token, "Failure declaring variable '"+variableName+"' with ambiguous type.");
            return false;
        }
        
        // Calculate byte offset for this variable by summing the size of the existing primitive variables.
        MaPLMemoryAddress currentOffset = 0;
        for (const std::unordered_map<std::string, MaPLVariable> &frame : _stack) {
            for (const auto&[name, variable] : frame) {
                if (variable.type.primitiveType != MaPLPrimitiveType_String) {
                    currentOffset += byteSizeOfType(variable.type.primitiveType);
                }
            }
        }
        
        // Calculate the highest memory usage for primitives.
        variable.memoryAddress = currentOffset;
        MaPLMemoryAddress currentMemoryUsed = currentOffset+variableSize;
        if (currentMemoryUsed > _maximumPrimitiveMemoryUsed) {
            _maximumPrimitiveMemoryUsed = currentMemoryUsed;
        }
    }
    
    // Insert the variable into the stack.
    _stack[_stack.size()-1][variableName] = variable;
    return true;
}

bool MaPLVariableStack::appendVariableStack(MaPLVariableStack *otherStack) {
    MaPLMemoryAddress allocatedStackSize = _maximumAllocatedMemoryUsed;
    MaPLMemoryAddress primitiveStackSize = _maximumPrimitiveMemoryUsed;
    for (const auto&[name, variable] : otherStack->getGlobalVariables()) {
        MaPLMemoryAddress adjustedAddress = variable.memoryAddress;
        if (!declareVariable(name, variable)) {
            return false;
        }
        
        // Redeclaring the variable will scramble its memory address. Calculate what
        // the appended memory address should be and fix them after redeclaration.
        if (variable.type.primitiveType == MaPLPrimitiveType_String) {
            adjustedAddress += allocatedStackSize;
        } else {
            adjustedAddress += primitiveStackSize;
        }
        _stack[_stack.size()-1][name].memoryAddress = adjustedAddress;
    }
    return true;
}

MaPLVariable MaPLVariableStack::getVariable(const std::string &variableName) {
    for (const std::unordered_map<std::string, MaPLVariable> &frame : _stack) {
        if (frame.count(variableName)) {
            return frame.at(variableName);
        }
    }
    return { { MaPLPrimitiveType_Uninitialized } };
}

std::unordered_map<std::string, MaPLVariable> MaPLVariableStack::getGlobalVariables() {
    return _stack[0];
}

std::unordered_map<std::string, MaPLVariable> MaPLVariableStack::getTopStackFrame() {
    return _stack[_stack.size()-1];
}

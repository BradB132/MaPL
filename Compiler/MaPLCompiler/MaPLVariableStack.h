//
//  MaPLVariableStack.h
//  MaPLCompiler
//
//  Created by Brad Bambara on 3/3/22.
//

#ifndef MaPLVariableStack_h
#define MaPLVariableStack_h

#include <stdio.h>
#include <vector>
#include <unordered_map>

#include "MaPLCompilerHelpers.h"
#include "MaPLBytecodeConstants.h"

/**
 * Describes the declaration of a MaPL variable.
 */
struct MaPLVariable {
    // Specifies the type of this variable declaration.
    MaPLType type;
    // The file in which this variable was declared.
    MaPLFile *file;
    // The token which represents this variable declaration.
    antlr4::Token *token;
    // Specifies the location in the memory where this variable is stored.
    MaPLMemoryAddress memoryAddress;
    // Specifies if this variable was declared in a dependent file.
    bool declaredInDependency = false;
};

/**
 * An object that tracks the variables that are declared in the script.
 */
class  MaPLVariableStack {
public:
    
    MaPLVariableStack();
    
    /**
     * Pushes another frame onto the variable stack. This happens when the MaPL program enters a nested scope.
     */
    void push();
    
    /**
     * Pops a frame from the variable stack. This happens when the MaPL program exits a nested scope.
     */
    void pop();
    
    /**
     * @return The maximum number of bytes that this stack required to store all primitive variables, at any point during the parse tree traversal.
     */
    MaPLMemoryAddress getMaximumPrimitiveMemoryUsed();
    
    /**
     * @return The maximum number of bytes that this stack required to store all allocated variables (strings), at any point during the parse tree traversal.
     */
    MaPLMemoryAddress getMaximumAllocatedMemoryUsed();

    /**
     * Inserts a new variable into the top stack frame. The variable will be assigned a new @c memoryAddress based on current contents of the stack.
     *
     * @return @c true if declaration is successful.
     */
    bool declareVariable(const std::string &variableName, MaPLVariable variable);
    
    /**
     * Inserts all global variables from another stack into the top stack frame.
     */
    bool appendVariableStack(MaPLVariableStack *otherStack,
                             MaPLMemoryAddress primitiveMemoryAddressOffset,
                             MaPLMemoryAddress allocatedMemoryIndexOffset);
    
    /**
     * @return The @c MaPLVariable with the given name. If the variable does not exist, its @c primitiveType will be @c MaPLPrimitiveType_TypeError.
     */
    MaPLVariable getVariable(const std::string &variableName);

    /**
     * @return A mapping of all variables that are not within a nested scope.
     */
    std::unordered_map<std::string, MaPLVariable> getGlobalVariables();
    
    /**
     * @return A mapping of all variables that are in the top stack frame.
     */
    std::unordered_map<std::string, MaPLVariable> getTopStackFrame();
    
    /**
     * Flags all variables as being declared in a dependency. This is useful when the compiler is traversing the dependency graph.
     */
    void flagAllVariablesAsDependency();
    
private:
    
    std::vector<std::unordered_map<std::string, MaPLVariable>> _stack;
    MaPLMemoryAddress _maximumPrimitiveMemoryUsed;
    MaPLMemoryAddress _maximumAllocatedMemoryUsed;
};

#endif /* MaPLVariableStack_h */

//
//  MaPLCompilerHelpers.cpp
//  MaPLCompiler
//
//  Created by Brad Bambara on 2/14/22.
//

#include <set>
#include <vector>

#include "MaPLCompiler.h"
#include "MaPLCompilerHelpers.h"
#include "MaPLFile.h"
#include "MaPLFileCache.h"
#include "MaPLBuffer.h"

MaPLCompileResult compileMaPL(const std::vector<std::filesystem::path> &scriptPaths, const MaPLCompileOptions &options) {
    MaPLCompileResult compileResult;
    MaPLFileCache fileCache;
    std::vector<MaPLFile *> files;
    
    // Create a file for each path.
    for (const std::filesystem::path &path : scriptPaths) {
        if (!path.is_absolute()) {
            compileResult.errorMessages.push_back("Path '"+path.string()+"' must be specified as an absolute path.");
            continue;
        }
        MaPLFile *file = fileCache.fileForNormalizedPath(path.lexically_normal());
        file->setOptions(options);
        file->compileIfNeeded();
        files.push_back(file);
    }
    
    // Check for errors, not just in the listed files, but in all included dependent files.
    for (const auto&[path, file] : fileCache.getFiles()) {
        std::vector<std::string> errors = file->getErrors();
        compileResult.errorMessages.insert(compileResult.errorMessages.end(), errors.begin(), errors.end());
    }
    
    size_t byteLimitation = sizeof(MaPLMemoryAddress) < sizeof(MaPLBytecodeLength) ? sizeof(MaPLMemoryAddress) : sizeof(MaPLBytecodeLength);
    size_t maxByteCount = (size_t)pow(2, byteLimitation*8);
    for (MaPLFile *file : files) {
        size_t fileByteCount = file->getBytecode()->getByteCount();
        if (fileByteCount > maxByteCount) {
            compileResult.errorMessages.push_back("Compiled bytecode for file '"+file->getNormalizedFilePath().string()+"' is "+std::to_string(fileByteCount)+" bytes, which exceeds the length that can be described by MaPL's "+std::to_string(byteLimitation)+"-byte addressing system (max of "+std::to_string(maxByteCount)+" bytes). The compiler and runtime must be updated if scripts of this length are required.");
        }
    }
    
    if (compileResult.errorMessages.size()) {
        return compileResult;
    }
    
    // Generate the symbol table.
    std::map<std::string, MaPLSymbol> symbolTable = symbolTableForFiles(files);
    compileResult.symbolTable = "#ifndef "+options.symbolsPrefix+"_h\n#define "+options.symbolsPrefix+"_h\nenum "+options.symbolsPrefix+" {\n";
    for (const auto&[descriptor, symbol] : symbolTable) {
        compileResult.symbolTable += "    "+options.symbolsPrefix+"_"+descriptor+" = "+std::to_string(symbol)+",\n";
    }
    compileResult.symbolTable += "};\n#endif /* "+options.symbolsPrefix+"_h */\n";
    
    // Put the finishing touches on the bytecode and add each one to the result.
    for (MaPLFile *file : files) {
        MaPLBuffer *buffer = file->getBytecode();
        buffer->resolveSymbolsWithTable(symbolTable);
        
        // Prepend the amount of memory that the script requires.
        MaPLMemoryAddress prependedAddresses[] = {
            file->getVariableStack()->getMaximumPrimitiveMemoryUsed(),
            file->getVariableStack()->getMaximumAllocatedMemoryUsed()
        };
        buffer->prependBytes(prependedAddresses, sizeof(prependedAddresses));
        
        compileResult.compiledFiles[file->getNormalizedFilePath()] = buffer->getBytes();
    }
    
    return compileResult;
}

bool isAmbiguousNumericType(MaPLPrimitiveType type) {
    switch (type) {
        case MaPLPrimitiveType_Float_AmbiguousSize: // Intentional fallthrough.
        case MaPLPrimitiveType_SignedInt_AmbiguousSize: // Intentional fallthrough.
        case MaPLPrimitiveType_Int_AmbiguousSizeAndSign:
            return true;
        default:
            return false;
    }
}

bool isConcreteFloat(MaPLPrimitiveType type) {
    switch (type) {
        case MaPLPrimitiveType_Float32: // Intentional fallthrough.
        case MaPLPrimitiveType_Float64:
            return true;
        default:
            return false;
    }
}

bool isConcreteSignedInt(MaPLPrimitiveType type) {
    switch (type) {
        case MaPLPrimitiveType_Int32: // Intentional fallthrough.
        case MaPLPrimitiveType_Int64:
            return true;
        default:
            return false;
    }
}

bool isConcreteUnsignedInt(MaPLPrimitiveType type) {
    switch (type) {
        case MaPLPrimitiveType_Char: // Intentional fallthrough.
        case MaPLPrimitiveType_UInt32: // Intentional fallthrough.
        case MaPLPrimitiveType_UInt64:
            return true;
        default:
            return false;
    }
}

bool isIntegral(MaPLPrimitiveType type) {
    return isConcreteSignedInt(type) ||
           isConcreteUnsignedInt(type) ||
           type == MaPLPrimitiveType_Int_AmbiguousSizeAndSign ||
           type == MaPLPrimitiveType_SignedInt_AmbiguousSize;
}

bool isNumeric(MaPLPrimitiveType type) {
    return isConcreteFloat(type) ||
           isConcreteSignedInt(type) ||
           isConcreteUnsignedInt(type) ||
           isAmbiguousNumericType(type);
}

bool isConcreteType(MaPLPrimitiveType type) {
    return isConcreteFloat(type) ||
           isConcreteSignedInt(type) ||
           isConcreteUnsignedInt(type) ||
           type == MaPLPrimitiveType_Boolean ||
           type == MaPLPrimitiveType_String ||
           type == MaPLPrimitiveType_Pointer;
}

MaPLMemoryAddress byteSizeOfType(MaPLPrimitiveType type) {
    switch (type) {
        case MaPLPrimitiveType_Char: // Intentional fallthrough.
        case MaPLPrimitiveType_Boolean:
            return 1;
        case MaPLPrimitiveType_Int32: // Intentional fallthrough.
        case MaPLPrimitiveType_UInt32: // Intentional fallthrough.
        case MaPLPrimitiveType_Float32:
            return 4;
        case MaPLPrimitiveType_Int64: // Intentional fallthrough.
        case MaPLPrimitiveType_UInt64: // Intentional fallthrough.
        case MaPLPrimitiveType_Float64: // Intentional fallthrough.
        case MaPLPrimitiveType_Pointer:
            return 8;
        default: return 0;
    }
}

std::string descriptorForPrimitive(MaPLPrimitiveType type) {
    switch (type) {
        case MaPLPrimitiveType_Char: return "char";
        case MaPLPrimitiveType_Int32: return "int32";
        case MaPLPrimitiveType_Int64: return "int64";
        case MaPLPrimitiveType_UInt32: return "uint32";
        case MaPLPrimitiveType_UInt64: return "uint64";
        case MaPLPrimitiveType_Float32: return "float32";
        case MaPLPrimitiveType_Float64: return "float64";
        case MaPLPrimitiveType_Boolean: return "bool";
        case MaPLPrimitiveType_String: return "string";
        case MaPLPrimitiveType_Pointer: return "pointer";
        case MaPLPrimitiveType_SignedInt_AmbiguousSize: return "signed integer";
        case MaPLPrimitiveType_Int_AmbiguousSizeAndSign: return "integer";
        case MaPLPrimitiveType_Float_AmbiguousSize: return "floating point";
        case MaPLPrimitiveType_Void: return "void";
        case MaPLPrimitiveType_Uninitialized: return "uninitialized";
        case MaPLPrimitiveType_TypeError: return "invalid type";
    }
}

std::string descriptorForType(const MaPLType &type) {
    if (type.primitiveType == MaPLPrimitiveType_Pointer) {
        if (type.pointerType.empty()) {
            return "NULL";
        }
        std::string descriptor = type.pointerType;
        if (type.generics.size() > 0) {
            descriptor += "<";
            for (size_t i = 0; i < type.generics.size(); i++) {
                descriptor += descriptorForType(type.generics[i]);
                if (i < type.generics.size()-1) {
                    descriptor += ", ";
                }
            }
            descriptor += ">";
        }
        return descriptor;
    }
    return descriptorForPrimitive(type.primitiveType);
}

std::string descriptorForFunction(const std::string &typeName,
                                  const std::string &name,
                                  const std::vector<MaPLType> &parameterTypes,
                                  bool hasVariadicParams) {
    std::string functionDescriptor;
    if (!typeName.empty()) {
        functionDescriptor += typeName;
        functionDescriptor += "::";
    }
    functionDescriptor += name;
    functionDescriptor += "(";
    for (size_t i = 0; i < parameterTypes.size(); i++) {
        functionDescriptor += descriptorForType(parameterTypes[i]);
        if (i < parameterTypes.size()-1) {
            functionDescriptor += ", ";
        }
    }
    if (hasVariadicParams) {
        functionDescriptor += ", ...";
    }
    functionDescriptor += ")";
    return functionDescriptor;
}

MaPLInstruction typecastToInstructionForPrimitive(MaPLPrimitiveType type) {
    switch (type) {
        case MaPLPrimitiveType_Char: return MaPLInstruction_char_typecast;
        case MaPLPrimitiveType_Int32: return MaPLInstruction_int32_typecast;
        case MaPLPrimitiveType_Int64: return MaPLInstruction_int64_typecast;
        case MaPLPrimitiveType_UInt32: return MaPLInstruction_uint32_typecast;
        case MaPLPrimitiveType_UInt64: return MaPLInstruction_uint64_typecast;
        case MaPLPrimitiveType_Float32: return MaPLInstruction_float32_typecast;
        case MaPLPrimitiveType_Float64: return MaPLInstruction_float64_typecast;
        case MaPLPrimitiveType_Boolean: return MaPLInstruction_boolean_typecast;
        case MaPLPrimitiveType_String: return MaPLInstruction_string_typecast;
        default: return MaPLInstruction_error;
    }
}

MaPLInstruction equalityInstructionForPrimitive(MaPLPrimitiveType type) {
    switch (type) {
        case MaPLPrimitiveType_Char: return MaPLInstruction_logical_equality_char;
        case MaPLPrimitiveType_Int32: return MaPLInstruction_logical_equality_int32;
        case MaPLPrimitiveType_Int64: return MaPLInstruction_logical_equality_int64;
        case MaPLPrimitiveType_UInt32: return MaPLInstruction_logical_equality_uint32;
        case MaPLPrimitiveType_UInt64: return MaPLInstruction_logical_equality_uint64;
        case MaPLPrimitiveType_Float32: return MaPLInstruction_logical_equality_float32;
        case MaPLPrimitiveType_Float64: return MaPLInstruction_logical_equality_float64;
        case MaPLPrimitiveType_Boolean: return MaPLInstruction_logical_equality_boolean;
        case MaPLPrimitiveType_String: return MaPLInstruction_logical_equality_string;
        case MaPLPrimitiveType_Pointer: return MaPLInstruction_logical_equality_pointer;
        default: return MaPLInstruction_error;
    }
}

MaPLInstruction inequalityInstructionForPrimitive(MaPLPrimitiveType type) {
    switch (type) {
        case MaPLPrimitiveType_Char: return MaPLInstruction_logical_inequality_char;
        case MaPLPrimitiveType_Int32: return MaPLInstruction_logical_inequality_int32;
        case MaPLPrimitiveType_Int64: return MaPLInstruction_logical_inequality_int64;
        case MaPLPrimitiveType_UInt32: return MaPLInstruction_logical_inequality_uint32;
        case MaPLPrimitiveType_UInt64: return MaPLInstruction_logical_inequality_uint64;
        case MaPLPrimitiveType_Float32: return MaPLInstruction_logical_inequality_float32;
        case MaPLPrimitiveType_Float64: return MaPLInstruction_logical_inequality_float64;
        case MaPLPrimitiveType_Boolean: return MaPLInstruction_logical_inequality_boolean;
        case MaPLPrimitiveType_String: return MaPLInstruction_logical_inequality_string;
        case MaPLPrimitiveType_Pointer: return MaPLInstruction_logical_inequality_pointer;
        default: return MaPLInstruction_error;
    }
}

MaPLInstruction lessThanInstructionForPrimitive(MaPLPrimitiveType type) {
    switch (type) {
        case MaPLPrimitiveType_Char: return MaPLInstruction_logical_less_than_char;
        case MaPLPrimitiveType_Int32: return MaPLInstruction_logical_less_than_int32;
        case MaPLPrimitiveType_Int64: return MaPLInstruction_logical_less_than_int64;
        case MaPLPrimitiveType_UInt32: return MaPLInstruction_logical_less_than_uint32;
        case MaPLPrimitiveType_UInt64: return MaPLInstruction_logical_less_than_uint64;
        case MaPLPrimitiveType_Float32: return MaPLInstruction_logical_less_than_float32;
        case MaPLPrimitiveType_Float64: return MaPLInstruction_logical_less_than_float64;
        default: return MaPLInstruction_error;
    }
}

MaPLInstruction lessThanOrEqualInstructionForPrimitive(MaPLPrimitiveType type) {
    switch (type) {
        case MaPLPrimitiveType_Char: return MaPLInstruction_logical_less_than_equal_char;
        case MaPLPrimitiveType_Int32: return MaPLInstruction_logical_less_than_equal_int32;
        case MaPLPrimitiveType_Int64: return MaPLInstruction_logical_less_than_equal_int64;
        case MaPLPrimitiveType_UInt32: return MaPLInstruction_logical_less_than_equal_uint32;
        case MaPLPrimitiveType_UInt64: return MaPLInstruction_logical_less_than_equal_uint64;
        case MaPLPrimitiveType_Float32: return MaPLInstruction_logical_less_than_equal_float32;
        case MaPLPrimitiveType_Float64: return MaPLInstruction_logical_less_than_equal_float64;
        default: return MaPLInstruction_error;
    }
}

MaPLInstruction greaterThanInstructionForPrimitive(MaPLPrimitiveType type) {
    switch (type) {
        case MaPLPrimitiveType_Char: return MaPLInstruction_logical_greater_than_char;
        case MaPLPrimitiveType_Int32: return MaPLInstruction_logical_greater_than_int32;
        case MaPLPrimitiveType_Int64: return MaPLInstruction_logical_greater_than_int64;
        case MaPLPrimitiveType_UInt32: return MaPLInstruction_logical_greater_than_uint32;
        case MaPLPrimitiveType_UInt64: return MaPLInstruction_logical_greater_than_uint64;
        case MaPLPrimitiveType_Float32: return MaPLInstruction_logical_greater_than_float32;
        case MaPLPrimitiveType_Float64: return MaPLInstruction_logical_greater_than_float64;
        default: return MaPLInstruction_error;
    }
}

MaPLInstruction greaterThanOrEqualInstructionForPrimitive(MaPLPrimitiveType type) {
    switch (type) {
        case MaPLPrimitiveType_Char: return MaPLInstruction_logical_greater_than_equal_char;
        case MaPLPrimitiveType_Int32: return MaPLInstruction_logical_greater_than_equal_int32;
        case MaPLPrimitiveType_Int64: return MaPLInstruction_logical_greater_than_equal_int64;
        case MaPLPrimitiveType_UInt32: return MaPLInstruction_logical_greater_than_equal_uint32;
        case MaPLPrimitiveType_UInt64: return MaPLInstruction_logical_greater_than_equal_uint64;
        case MaPLPrimitiveType_Float32: return MaPLInstruction_logical_greater_than_equal_float32;
        case MaPLPrimitiveType_Float64: return MaPLInstruction_logical_greater_than_equal_float64;
        default: return MaPLInstruction_error;
    }
}

MaPLInstruction assignmentInstructionForPrimitive(MaPLPrimitiveType type) {
    switch (type) {
        case MaPLPrimitiveType_Char: return MaPLInstruction_char_assign;
        case MaPLPrimitiveType_Int32: return MaPLInstruction_int32_assign;
        case MaPLPrimitiveType_Int64: return MaPLInstruction_int64_assign;
        case MaPLPrimitiveType_UInt32: return MaPLInstruction_uint32_assign;
        case MaPLPrimitiveType_UInt64: return MaPLInstruction_uint64_assign;
        case MaPLPrimitiveType_Float32: return MaPLInstruction_float32_assign;
        case MaPLPrimitiveType_Float64: return MaPLInstruction_float64_assign;
        case MaPLPrimitiveType_Boolean: return MaPLInstruction_boolean_assign;
        case MaPLPrimitiveType_String: return MaPLInstruction_string_assign;
        case MaPLPrimitiveType_Pointer: return MaPLInstruction_pointer_assign;
        default: return MaPLInstruction_error;
    }
}

MaPLInstruction functionInvocationInstructionForPrimitive(MaPLPrimitiveType type) {
    switch (type) {
        case MaPLPrimitiveType_Char: return MaPLInstruction_char_function_invocation;
        case MaPLPrimitiveType_Int32: return MaPLInstruction_int32_function_invocation;
        case MaPLPrimitiveType_Int64: return MaPLInstruction_int64_function_invocation;
        case MaPLPrimitiveType_UInt32: return MaPLInstruction_uint32_function_invocation;
        case MaPLPrimitiveType_UInt64: return MaPLInstruction_uint64_function_invocation;
        case MaPLPrimitiveType_Float32: return MaPLInstruction_float32_function_invocation;
        case MaPLPrimitiveType_Float64: return MaPLInstruction_float64_function_invocation;
        case MaPLPrimitiveType_String: return MaPLInstruction_string_function_invocation;
        case MaPLPrimitiveType_Boolean: return MaPLInstruction_boolean_function_invocation;
        case MaPLPrimitiveType_Pointer: return MaPLInstruction_pointer_function_invocation;
        default: return MaPLInstruction_error;
    }
}

MaPLInstruction subscriptInvocationInstructionForPrimitive(MaPLPrimitiveType type) {
    switch (type) {
        case MaPLPrimitiveType_Char: return MaPLInstruction_char_subscript_invocation;
        case MaPLPrimitiveType_Int32: return MaPLInstruction_int32_subscript_invocation;
        case MaPLPrimitiveType_Int64: return MaPLInstruction_int64_subscript_invocation;
        case MaPLPrimitiveType_UInt32: return MaPLInstruction_uint32_subscript_invocation;
        case MaPLPrimitiveType_UInt64: return MaPLInstruction_uint64_subscript_invocation;
        case MaPLPrimitiveType_Float32: return MaPLInstruction_float32_subscript_invocation;
        case MaPLPrimitiveType_Float64: return MaPLInstruction_float64_subscript_invocation;
        case MaPLPrimitiveType_String: return MaPLInstruction_string_subscript_invocation;
        case MaPLPrimitiveType_Boolean: return MaPLInstruction_boolean_subscript_invocation;
        case MaPLPrimitiveType_Pointer: return MaPLInstruction_pointer_subscript_invocation;
        default: return MaPLInstruction_error;
    }
}

MaPLInstruction operatorAssignInstructionForTokenType(size_t tokenType, MaPLPrimitiveType primitiveType) {
    switch (tokenType) {
        case MaPLParser::ADD_ASSIGN:
            if (primitiveType == MaPLPrimitiveType_String) {
                return MaPLInstruction_string_concat;
            }
            return additionInstructionForPrimitive(primitiveType);
        case MaPLParser::INCREMENT:
            return additionInstructionForPrimitive(primitiveType);
        case MaPLParser::SUBTRACT_ASSIGN: // Intentional fallthrough.
        case MaPLParser::DECREMENT:
            return subtractionInstructionForPrimitive(primitiveType);
        case MaPLParser::MULTIPLY_ASSIGN:
            return multiplicationInstructionForPrimitive(primitiveType);
        case MaPLParser::DIVIDE_ASSIGN:
            return divisionInstructionForPrimitive(primitiveType);
        case MaPLParser::MOD_ASSIGN:
            return moduloInstructionForPrimitive(primitiveType);
        case MaPLParser::BITWISE_AND_ASSIGN:
            return bitwiseAndInstructionForPrimitive(primitiveType);
        case MaPLParser::BITWISE_OR_ASSIGN:
            return bitwiseOrInstructionForPrimitive(primitiveType);
        case MaPLParser::BITWISE_XOR_ASSIGN:
            return bitwiseXorInstructionForPrimitive(primitiveType);
        case MaPLParser::BITWISE_SHIFT_LEFT_ASSIGN:
            return bitwiseShiftLeftInstructionForPrimitive(primitiveType);
        case MaPLParser::BITWISE_SHIFT_RIGHT_ASSIGN:
            return bitwiseShiftRightInstructionForPrimitive(primitiveType);
        default: return MaPLInstruction_no_op;
    }
}

MaPLInstruction variableInstructionForPrimitive(MaPLPrimitiveType type) {
    switch (type) {
        case MaPLPrimitiveType_Char: return MaPLInstruction_char_variable;
        case MaPLPrimitiveType_Int32: return MaPLInstruction_int32_variable;
        case MaPLPrimitiveType_Int64: return MaPLInstruction_int64_variable;
        case MaPLPrimitiveType_UInt32: return MaPLInstruction_uint32_variable;
        case MaPLPrimitiveType_UInt64: return MaPLInstruction_uint64_variable;
        case MaPLPrimitiveType_Float32: return MaPLInstruction_float32_variable;
        case MaPLPrimitiveType_Float64: return MaPLInstruction_float64_variable;
        case MaPLPrimitiveType_String: return MaPLInstruction_string_variable;
        case MaPLPrimitiveType_Boolean: return MaPLInstruction_boolean_variable;
        case MaPLPrimitiveType_Pointer: return MaPLInstruction_pointer_variable;
        default: return MaPLInstruction_error;
    }
}

MaPLInstruction numericLiteralInstructionForPrimitive(MaPLPrimitiveType type) {
    switch (type) {
        case MaPLPrimitiveType_Char: return MaPLInstruction_char_literal;
        case MaPLPrimitiveType_Int32: return MaPLInstruction_int32_literal;
        case MaPLPrimitiveType_Int64: return MaPLInstruction_int64_literal;
        case MaPLPrimitiveType_UInt32: return MaPLInstruction_uint32_literal;
        case MaPLPrimitiveType_UInt64: return MaPLInstruction_uint64_literal;
        case MaPLPrimitiveType_Float32: return MaPLInstruction_float32_literal;
        case MaPLPrimitiveType_Float64: return MaPLInstruction_float64_literal;
        default: return MaPLInstruction_error;
    }
}

MaPLInstruction additionInstructionForPrimitive(MaPLPrimitiveType type) {
    switch (type) {
        case MaPLPrimitiveType_Char: return MaPLInstruction_char_add;
        case MaPLPrimitiveType_Int32: return MaPLInstruction_int32_add;
        case MaPLPrimitiveType_Int64: return MaPLInstruction_int64_add;
        case MaPLPrimitiveType_UInt32: return MaPLInstruction_uint32_add;
        case MaPLPrimitiveType_UInt64: return MaPLInstruction_uint64_add;
        case MaPLPrimitiveType_Float32: return MaPLInstruction_float32_add;
        case MaPLPrimitiveType_Float64: return MaPLInstruction_float64_add;
        default: return MaPLInstruction_error;
    }
}

MaPLInstruction subtractionInstructionForPrimitive(MaPLPrimitiveType type) {
    switch (type) {
        case MaPLPrimitiveType_Char: return MaPLInstruction_char_subtract;
        case MaPLPrimitiveType_Int32: return MaPLInstruction_int32_subtract;
        case MaPLPrimitiveType_Int64: return MaPLInstruction_int64_subtract;
        case MaPLPrimitiveType_UInt32: return MaPLInstruction_uint32_subtract;
        case MaPLPrimitiveType_UInt64: return MaPLInstruction_uint64_subtract;
        case MaPLPrimitiveType_Float32: return MaPLInstruction_float32_subtract;
        case MaPLPrimitiveType_Float64: return MaPLInstruction_float64_subtract;
        default: return MaPLInstruction_error;
    }
}

MaPLInstruction multiplicationInstructionForPrimitive(MaPLPrimitiveType type) {
    switch (type) {
        case MaPLPrimitiveType_Char: return MaPLInstruction_char_multiply;
        case MaPLPrimitiveType_Int32: return MaPLInstruction_int32_multiply;
        case MaPLPrimitiveType_Int64: return MaPLInstruction_int64_multiply;
        case MaPLPrimitiveType_UInt32: return MaPLInstruction_uint32_multiply;
        case MaPLPrimitiveType_UInt64: return MaPLInstruction_uint64_multiply;
        case MaPLPrimitiveType_Float32: return MaPLInstruction_float32_multiply;
        case MaPLPrimitiveType_Float64: return MaPLInstruction_float64_multiply;
        default: return MaPLInstruction_error;
    }
}

MaPLInstruction divisionInstructionForPrimitive(MaPLPrimitiveType type) {
    switch (type) {
        case MaPLPrimitiveType_Char: return MaPLInstruction_char_divide;
        case MaPLPrimitiveType_Int32: return MaPLInstruction_int32_divide;
        case MaPLPrimitiveType_Int64: return MaPLInstruction_int64_divide;
        case MaPLPrimitiveType_UInt32: return MaPLInstruction_uint32_divide;
        case MaPLPrimitiveType_UInt64: return MaPLInstruction_uint64_divide;
        case MaPLPrimitiveType_Float32: return MaPLInstruction_float32_divide;
        case MaPLPrimitiveType_Float64: return MaPLInstruction_float64_divide;
        default: return MaPLInstruction_error;
    }
}

MaPLInstruction moduloInstructionForPrimitive(MaPLPrimitiveType type) {
    switch (type) {
        case MaPLPrimitiveType_Char: return MaPLInstruction_char_modulo;
        case MaPLPrimitiveType_Int32: return MaPLInstruction_int32_modulo;
        case MaPLPrimitiveType_Int64: return MaPLInstruction_int64_modulo;
        case MaPLPrimitiveType_UInt32: return MaPLInstruction_uint32_modulo;
        case MaPLPrimitiveType_UInt64: return MaPLInstruction_uint64_modulo;
        case MaPLPrimitiveType_Float32: return MaPLInstruction_float32_modulo;
        case MaPLPrimitiveType_Float64: return MaPLInstruction_float64_modulo;
        default: return MaPLInstruction_error;
    }
}

MaPLInstruction numericNegationInstructionForPrimitive(MaPLPrimitiveType type) {
    switch (type) {
        case MaPLPrimitiveType_Int32: return MaPLInstruction_int32_numeric_negation;
        case MaPLPrimitiveType_Int64: return MaPLInstruction_int64_numeric_negation;
        case MaPLPrimitiveType_Float32: return MaPLInstruction_float32_numeric_negation;
        case MaPLPrimitiveType_Float64: return MaPLInstruction_float64_numeric_negation;
        default: return MaPLInstruction_error;
    }
}

MaPLInstruction bitwiseNegationInstructionForPrimitive(MaPLPrimitiveType type) {
    switch (type) {
        case MaPLPrimitiveType_Char: return MaPLInstruction_char_bitwise_negation;
        case MaPLPrimitiveType_Int32: return MaPLInstruction_int32_bitwise_negation;
        case MaPLPrimitiveType_Int64: return MaPLInstruction_int64_bitwise_negation;
        case MaPLPrimitiveType_UInt32: return MaPLInstruction_uint32_bitwise_negation;
        case MaPLPrimitiveType_UInt64: return MaPLInstruction_uint64_bitwise_negation;
        default: return MaPLInstruction_error;
    }
}

MaPLInstruction bitwiseAndInstructionForPrimitive(MaPLPrimitiveType type) {
    switch (type) {
        case MaPLPrimitiveType_Char: return MaPLInstruction_char_bitwise_and;
        case MaPLPrimitiveType_Int32: return MaPLInstruction_int32_bitwise_and;
        case MaPLPrimitiveType_Int64: return MaPLInstruction_int64_bitwise_and;
        case MaPLPrimitiveType_UInt32: return MaPLInstruction_uint32_bitwise_and;
        case MaPLPrimitiveType_UInt64: return MaPLInstruction_uint64_bitwise_and;
        default: return MaPLInstruction_error;
    }
}

MaPLInstruction bitwiseOrInstructionForPrimitive(MaPLPrimitiveType type) {
    switch (type) {
        case MaPLPrimitiveType_Char: return MaPLInstruction_char_bitwise_or;
        case MaPLPrimitiveType_Int32: return MaPLInstruction_int32_bitwise_or;
        case MaPLPrimitiveType_Int64: return MaPLInstruction_int64_bitwise_or;
        case MaPLPrimitiveType_UInt32: return MaPLInstruction_uint32_bitwise_or;
        case MaPLPrimitiveType_UInt64: return MaPLInstruction_uint64_bitwise_or;
        default: return MaPLInstruction_error;
    }
}

MaPLInstruction bitwiseXorInstructionForPrimitive(MaPLPrimitiveType type) {
    switch (type) {
        case MaPLPrimitiveType_Char: return MaPLInstruction_char_bitwise_xor;
        case MaPLPrimitiveType_Int32: return MaPLInstruction_int32_bitwise_xor;
        case MaPLPrimitiveType_Int64: return MaPLInstruction_int64_bitwise_xor;
        case MaPLPrimitiveType_UInt32: return MaPLInstruction_uint32_bitwise_xor;
        case MaPLPrimitiveType_UInt64: return MaPLInstruction_uint64_bitwise_xor;
        default: return MaPLInstruction_error;
    }
}

MaPLInstruction bitwiseShiftLeftInstructionForPrimitive(MaPLPrimitiveType type) {
    switch (type) {
        case MaPLPrimitiveType_Char: return MaPLInstruction_char_bitwise_shift_left;
        case MaPLPrimitiveType_Int32: return MaPLInstruction_int32_bitwise_shift_left;
        case MaPLPrimitiveType_Int64: return MaPLInstruction_int64_bitwise_shift_left;
        case MaPLPrimitiveType_UInt32: return MaPLInstruction_uint32_bitwise_shift_left;
        case MaPLPrimitiveType_UInt64: return MaPLInstruction_uint64_bitwise_shift_left;
        default: return MaPLInstruction_error;
    }
}

MaPLInstruction bitwiseShiftRightInstructionForPrimitive(MaPLPrimitiveType type) {
    switch (type) {
        case MaPLPrimitiveType_Char: return MaPLInstruction_char_bitwise_shift_right;
        case MaPLPrimitiveType_Int32: return MaPLInstruction_int32_bitwise_shift_right;
        case MaPLPrimitiveType_Int64: return MaPLInstruction_int64_bitwise_shift_right;
        case MaPLPrimitiveType_UInt32: return MaPLInstruction_uint32_bitwise_shift_right;
        case MaPLPrimitiveType_UInt64: return MaPLInstruction_uint64_bitwise_shift_right;
        default: return MaPLInstruction_error;
    }
}

MaPLInstruction ternaryConditionalInstructionForPrimitive(MaPLPrimitiveType type) {
    switch (type) {
        case MaPLPrimitiveType_Char: return MaPLInstruction_char_ternary_conditional;
        case MaPLPrimitiveType_Int32: return MaPLInstruction_int32_ternary_conditional;
        case MaPLPrimitiveType_Int64: return MaPLInstruction_int64_ternary_conditional;
        case MaPLPrimitiveType_UInt32: return MaPLInstruction_uint32_ternary_conditional;
        case MaPLPrimitiveType_UInt64: return MaPLInstruction_uint64_ternary_conditional;
        case MaPLPrimitiveType_Float32: return MaPLInstruction_float32_ternary_conditional;
        case MaPLPrimitiveType_Float64: return MaPLInstruction_float64_ternary_conditional;
        case MaPLPrimitiveType_String: return MaPLInstruction_string_ternary_conditional;
        case MaPLPrimitiveType_Boolean: return MaPLInstruction_boolean_ternary_conditional;
        case MaPLPrimitiveType_Pointer: return MaPLInstruction_pointer_ternary_conditional;
        default: return MaPLInstruction_error;
    }
}

MaPLType typeForPointerType(MaPLParser::PointerTypeContext *pointerTypeContext) {
    MaPLType returnValue{ MaPLPrimitiveType_Pointer, pointerTypeContext->identifier()->getText() };
    std::vector<MaPLParser::TypeContext *> types = pointerTypeContext->type();
    returnValue.generics.reserve(types.size());
    for (MaPLParser::TypeContext *type : types) {
        returnValue.generics.push_back(typeForTypeContext(type));
    }
    return returnValue;
}

MaPLPrimitiveType primitiveTypeForTypeContext(MaPLParser::TypeContext *typeContext) {
    MaPLParser::PointerTypeContext *pointerType = typeContext->pointerType();
    if (pointerType) {
        return MaPLPrimitiveType_Pointer;
    }
    switch (typeContext->start->getType()) {
        case MaPLParser::DECL_CHAR: return MaPLPrimitiveType_Char;
        case MaPLParser::DECL_INT32: return MaPLPrimitiveType_Int32;
        case MaPLParser::DECL_INT64: return MaPLPrimitiveType_Int64;
        case MaPLParser::DECL_UINT32: return MaPLPrimitiveType_UInt32;
        case MaPLParser::DECL_UINT64: return MaPLPrimitiveType_UInt64;
        case MaPLParser::DECL_FLOAT32: return MaPLPrimitiveType_Float32;
        case MaPLParser::DECL_FLOAT64: return MaPLPrimitiveType_Float64;
        case MaPLParser::DECL_BOOL: return MaPLPrimitiveType_Boolean;
        case MaPLParser::DECL_STRING: return MaPLPrimitiveType_String;
        default: return MaPLPrimitiveType_TypeError;
    }
}

MaPLType typeForTypeContext(MaPLParser::TypeContext *typeContext) {
    MaPLPrimitiveType primitive = primitiveTypeForTypeContext(typeContext);
    if (primitive == MaPLPrimitiveType_Pointer) {
        return typeForPointerType(typeContext->pointerType());
    }
    return { primitive };
}

bool MaPLType::operator== (const MaPLType &otherType) const {
    if (primitiveType != otherType.primitiveType) {
        return false;
    }
    if (primitiveType == MaPLPrimitiveType_Pointer) {
        if (pointerType != otherType.pointerType || generics.size() != otherType.generics.size()) {
            return false;
        }
        for (size_t i = 0; i < generics.size(); i++) {
            if (generics[i] != otherType.generics[i]) {
                return false;
            }
        }
    }
    return true;
}

bool MaPLType::operator!= (const MaPLType &otherType) const {
    return !(*this == otherType);
}

bool typeNameMatchesPrimitiveType(const std::string &typeName) {
    return typeName == "char" ||
        typeName == "int32" ||
        typeName == "int64" ||
        typeName == "uint32" ||
        typeName == "uint64" ||
        typeName == "float32" ||
        typeName == "float64" ||
        typeName == "bool" ||
        typeName == "string" ||
        typeName == "void";
}

void confirmSignedValueFitsInSignedBits(int64_t value, int8_t bits, MaPLFile *file, antlr4::Token *token) {
    if (value >= 1L << (bits-1) ||
        value < -(1L << (bits-1))) {
        file->logError(token, "A value of "+std::to_string(value)+" is outside the range of numbers that can be represented by a "+std::to_string(bits)+"-bit signed integer.");
    }
}

void confirmUnsignedValueFitsInSignedBits(u_int64_t value, int8_t bits, MaPLFile *file, antlr4::Token *token) {
    if (value >= 1L << (bits-1)) {
        file->logError(token, "A value of "+std::to_string(value)+" is outside the range of numbers that can be represented by a "+std::to_string(bits)+"-bit signed integer.");
    }
}

void confirmSignedValueFitsInUnsignedBits(int64_t value, int8_t bits, MaPLFile *file, antlr4::Token *token) {
    if (value >= 1UL << bits ||
        value < 0) {
        file->logError(token, "A value of "+std::to_string(value)+" is outside the range of numbers that can be represented by a "+std::to_string(bits)+"-bit unsigned integer.");
    }
}

void confirmUnsignedValueFitsInUnsignedBits(u_int64_t value, int8_t bits, MaPLFile *file, antlr4::Token *token) {
    if (value >= 1UL << bits) {
        file->logError(token, "A value of "+std::to_string(value)+" is outside the range of numbers that can be represented by a "+std::to_string(bits)+"-bit unsigned integer.");
    }
}

MaPLLiteral castLiteralToType(const MaPLLiteral &literal, const MaPLType &castType, MaPLFile *file, antlr4::Token *token) {
    MaPLLiteral returnVal{ { castType.primitiveType } };
    switch (castType.primitiveType) {
        case MaPLPrimitiveType_Char:
            switch (literal.type.primitiveType) {
                case MaPLPrimitiveType_Char:
                    return literal;
                case MaPLPrimitiveType_Int32:
                    confirmSignedValueFitsInUnsignedBits(literal.int32Value, 8, file, token);
                    returnVal.charValue = (u_int8_t)literal.int32Value;
                    return returnVal;
                case MaPLPrimitiveType_Int64: // Intentional fallthrough.
                case MaPLPrimitiveType_SignedInt_AmbiguousSize:
                    confirmSignedValueFitsInUnsignedBits(literal.int64Value, 8, file, token);
                    returnVal.charValue = (u_int8_t)literal.int64Value;
                    return returnVal;
                case MaPLPrimitiveType_UInt32:
                    confirmUnsignedValueFitsInUnsignedBits(literal.uInt32Value, 8, file, token);
                    returnVal.charValue = (u_int8_t)literal.uInt32Value;
                    return returnVal;
                case MaPLPrimitiveType_UInt64: // Intentional fallthrough.
                case MaPLPrimitiveType_Int_AmbiguousSizeAndSign:
                    confirmUnsignedValueFitsInUnsignedBits(literal.uInt64Value, 8, file, token);
                    returnVal.charValue = (u_int8_t)literal.uInt64Value;
                    return returnVal;
                case MaPLPrimitiveType_Float32:
                    confirmSignedValueFitsInUnsignedBits((int64_t)literal.float32Value, 8, file, token);
                    returnVal.charValue = (u_int8_t)literal.float32Value;
                    return returnVal;
                case MaPLPrimitiveType_Float64: // Intentional fallthrough.
                case MaPLPrimitiveType_Float_AmbiguousSize:
                    confirmSignedValueFitsInUnsignedBits((int64_t)literal.float64Value, 8, file, token);
                    returnVal.charValue = (u_int8_t)literal.float64Value;
                    return returnVal;
                case MaPLPrimitiveType_String: {
                    u_int64_t stringAsUnsignedInt = (u_int64_t)std::stoull(literal.stringValue);
                    confirmUnsignedValueFitsInUnsignedBits(stringAsUnsignedInt, 8, file, token);
                    returnVal.charValue = (u_int8_t)stringAsUnsignedInt;
                    return returnVal;
                }
                case MaPLPrimitiveType_Boolean:
                    returnVal.charValue = literal.booleanValue ? 1 : 0;
                    return returnVal;
                default: break;
            }
            break;
        case MaPLPrimitiveType_Int32:
            switch (literal.type.primitiveType) {
                case MaPLPrimitiveType_Char:
                    returnVal.int32Value = (int32_t)literal.charValue;
                    return returnVal;
                case MaPLPrimitiveType_Int32:
                    return literal;
                case MaPLPrimitiveType_Int64: // Intentional fallthrough.
                case MaPLPrimitiveType_SignedInt_AmbiguousSize:
                    confirmSignedValueFitsInSignedBits(literal.int64Value, 32, file, token);
                    returnVal.int32Value = (int32_t)literal.int64Value;
                    return returnVal;
                case MaPLPrimitiveType_UInt32:
                    confirmUnsignedValueFitsInSignedBits(literal.uInt32Value, 32, file, token);
                    returnVal.int32Value = (int32_t)literal.uInt32Value;
                    return returnVal;
                case MaPLPrimitiveType_UInt64: // Intentional fallthrough.
                case MaPLPrimitiveType_Int_AmbiguousSizeAndSign:
                    confirmUnsignedValueFitsInSignedBits(literal.uInt64Value, 32, file, token);
                    returnVal.int32Value = (int32_t)literal.uInt64Value;
                    return returnVal;
                case MaPLPrimitiveType_Float32:
                    confirmSignedValueFitsInSignedBits((int64_t)literal.float32Value, 32, file, token);
                    returnVal.int32Value = (int32_t)literal.float32Value;
                    return returnVal;
                case MaPLPrimitiveType_Float64: // Intentional fallthrough.
                case MaPLPrimitiveType_Float_AmbiguousSize:
                    confirmSignedValueFitsInSignedBits((int64_t)literal.float64Value, 32, file, token);
                    returnVal.int32Value = (int32_t)literal.float64Value;
                    return returnVal;
                case MaPLPrimitiveType_String: {
                    int64_t stringAsSignedInt = (int64_t)std::stoll(literal.stringValue);
                    confirmSignedValueFitsInSignedBits(stringAsSignedInt, 32, file, token);
                    returnVal.int32Value = (int32_t)stringAsSignedInt;
                    return returnVal;
                }
                case MaPLPrimitiveType_Boolean:
                    returnVal.int32Value = literal.booleanValue ? 1 : 0;
                    return returnVal;
                default: break;
            }
            break;
        case MaPLPrimitiveType_Int64:
            switch (literal.type.primitiveType) {
                case MaPLPrimitiveType_Char:
                    returnVal.int64Value = (int64_t)literal.charValue;
                    return returnVal;
                case MaPLPrimitiveType_Int32:
                    returnVal.int64Value = (int64_t)literal.int32Value;
                    return returnVal;
                case MaPLPrimitiveType_Int64:
                    return literal;
                case MaPLPrimitiveType_SignedInt_AmbiguousSize:
                    returnVal.int64Value = literal.int64Value;
                    return returnVal;
                case MaPLPrimitiveType_UInt32:
                    returnVal.int64Value = (int64_t)literal.uInt32Value;
                    return returnVal;
                case MaPLPrimitiveType_UInt64: // Intentional fallthrough.
                case MaPLPrimitiveType_Int_AmbiguousSizeAndSign:
                    confirmUnsignedValueFitsInSignedBits(literal.uInt64Value, 64, file, token);
                    returnVal.int64Value = (int64_t)literal.uInt64Value;
                    return returnVal;
                case MaPLPrimitiveType_Float32:
                    returnVal.int64Value = (int64_t)literal.float32Value;
                    return returnVal;
                case MaPLPrimitiveType_Float64: // Intentional fallthrough.
                case MaPLPrimitiveType_Float_AmbiguousSize:
                    returnVal.int64Value = (int64_t)literal.float64Value;
                    return returnVal;
                case MaPLPrimitiveType_String:
                    returnVal.int64Value = (int64_t)std::stoll(literal.stringValue);
                    return returnVal;
                case MaPLPrimitiveType_Boolean:
                    returnVal.int64Value = literal.booleanValue ? 1 : 0;
                    return returnVal;
                default: break;
            }
            break;
        case MaPLPrimitiveType_UInt32:
            switch (literal.type.primitiveType) {
                case MaPLPrimitiveType_Char:
                    returnVal.uInt32Value = (u_int32_t)literal.charValue;
                    return returnVal;
                case MaPLPrimitiveType_Int32:
                    returnVal.uInt32Value = (u_int32_t)literal.int32Value;
                    return returnVal;
                case MaPLPrimitiveType_Int64: // Intentional fallthrough.
                case MaPLPrimitiveType_SignedInt_AmbiguousSize:
                    confirmSignedValueFitsInUnsignedBits(literal.int64Value, 32, file, token);
                    returnVal.uInt32Value = (u_int32_t)literal.int64Value;
                    return returnVal;
                case MaPLPrimitiveType_UInt32:
                    return literal;
                case MaPLPrimitiveType_UInt64: // Intentional fallthrough.
                case MaPLPrimitiveType_Int_AmbiguousSizeAndSign:
                    confirmUnsignedValueFitsInUnsignedBits(literal.uInt64Value, 32, file, token);
                    returnVal.uInt32Value = (u_int32_t)literal.uInt64Value;
                    return returnVal;
                case MaPLPrimitiveType_Float32:
                    confirmSignedValueFitsInUnsignedBits((int64_t)literal.float32Value, 32, file, token);
                    returnVal.uInt32Value = (u_int32_t)literal.float32Value;
                    return returnVal;
                case MaPLPrimitiveType_Float64: // Intentional fallthrough.
                case MaPLPrimitiveType_Float_AmbiguousSize:
                    confirmSignedValueFitsInUnsignedBits((int64_t)literal.float64Value, 32, file, token);
                    returnVal.uInt32Value = (u_int32_t)literal.float64Value;
                    return returnVal;
                case MaPLPrimitiveType_String: {
                    u_int64_t stringAsUnsignedInt = (u_int64_t)std::stoull(literal.stringValue);
                    confirmUnsignedValueFitsInUnsignedBits(stringAsUnsignedInt, 32, file, token);
                    returnVal.uInt32Value = (u_int32_t)stringAsUnsignedInt;
                    return returnVal;
                }
                case MaPLPrimitiveType_Boolean:
                    returnVal.uInt32Value = literal.booleanValue ? 1 : 0;
                    return returnVal;
                default: break;
            }
            break;
        case MaPLPrimitiveType_UInt64:
            switch (literal.type.primitiveType) {
                case MaPLPrimitiveType_Char:
                    returnVal.uInt64Value = (u_int64_t)literal.charValue;
                    return returnVal;
                case MaPLPrimitiveType_Int32:
                    returnVal.uInt64Value = (u_int64_t)literal.int32Value;
                    return returnVal;
                case MaPLPrimitiveType_Int64: // Intentional fallthrough.
                case MaPLPrimitiveType_SignedInt_AmbiguousSize:
                    returnVal.uInt64Value = (u_int64_t)literal.int64Value;
                    return returnVal;
                case MaPLPrimitiveType_UInt32:
                    returnVal.uInt64Value = (u_int64_t)literal.uInt32Value;
                    return returnVal;
                case MaPLPrimitiveType_UInt64:
                    return literal;
                case MaPLPrimitiveType_Int_AmbiguousSizeAndSign:
                    returnVal.uInt64Value = literal.uInt64Value;
                    return returnVal;
                case MaPLPrimitiveType_Float32:
                    returnVal.uInt64Value = (u_int64_t)literal.float32Value;
                    return returnVal;
                case MaPLPrimitiveType_Float64: // Intentional fallthrough.
                case MaPLPrimitiveType_Float_AmbiguousSize:
                    returnVal.uInt64Value = (u_int64_t)literal.float64Value;
                    return returnVal;
                case MaPLPrimitiveType_String:
                    returnVal.uInt64Value = (u_int64_t)std::stoull(literal.stringValue);
                    return returnVal;
                case MaPLPrimitiveType_Boolean:
                    returnVal.uInt64Value = literal.booleanValue ? 1 : 0;
                    return returnVal;
                default: break;
            }
            break;
        case MaPLPrimitiveType_Float32:
            switch (literal.type.primitiveType) {
                case MaPLPrimitiveType_Char:
                    returnVal.float32Value = (float)literal.charValue;
                    return returnVal;
                case MaPLPrimitiveType_Int32:
                    returnVal.float32Value = (float)literal.int32Value;
                    return returnVal;
                case MaPLPrimitiveType_Int64: // Intentional fallthrough.
                case MaPLPrimitiveType_SignedInt_AmbiguousSize:
                    returnVal.float32Value = (float)literal.int64Value;
                    return returnVal;
                case MaPLPrimitiveType_UInt32:
                    returnVal.float32Value = (float)literal.uInt32Value;
                    return returnVal;
                case MaPLPrimitiveType_UInt64: // Intentional fallthrough.
                case MaPLPrimitiveType_Int_AmbiguousSizeAndSign:
                    returnVal.float32Value = (float)literal.uInt64Value;
                    return returnVal;
                case MaPLPrimitiveType_Float32:
                    return literal;
                case MaPLPrimitiveType_Float64: // Intentional fallthrough.
                case MaPLPrimitiveType_Float_AmbiguousSize:
                    returnVal.float32Value = (float)literal.float64Value;
                    return returnVal;
                case MaPLPrimitiveType_String:
                    returnVal.float32Value = (float)std::stof(literal.stringValue);
                    return returnVal;
                case MaPLPrimitiveType_Boolean:
                    returnVal.float32Value = literal.booleanValue ? 1 : 0;
                    return returnVal;
                default: break;
            }
            break;
        case MaPLPrimitiveType_Float64:
            switch (literal.type.primitiveType) {
                case MaPLPrimitiveType_Char:
                    returnVal.float64Value = (double)literal.charValue;
                    return returnVal;
                case MaPLPrimitiveType_Int32:
                    returnVal.float64Value = (double)literal.int32Value;
                    return returnVal;
                case MaPLPrimitiveType_Int64: // Intentional fallthrough.
                case MaPLPrimitiveType_SignedInt_AmbiguousSize:
                    returnVal.float64Value = (double)literal.int64Value;
                    return returnVal;
                case MaPLPrimitiveType_UInt32:
                    returnVal.float64Value = (double)literal.uInt32Value;
                    return returnVal;
                case MaPLPrimitiveType_UInt64: // Intentional fallthrough.
                case MaPLPrimitiveType_Int_AmbiguousSizeAndSign:
                    returnVal.float64Value = (double)literal.uInt64Value;
                    return returnVal;
                case MaPLPrimitiveType_Float32:
                    returnVal.float64Value = (double)literal.float32Value;
                    return returnVal;
                case MaPLPrimitiveType_Float64:
                    return literal;
                case MaPLPrimitiveType_Float_AmbiguousSize:
                    returnVal.float64Value = literal.float64Value;
                    return returnVal;
                case MaPLPrimitiveType_String:
                    returnVal.float64Value = (double)std::stod(literal.stringValue);
                    return returnVal;
                case MaPLPrimitiveType_Boolean:
                    returnVal.float64Value = literal.booleanValue ? 1 : 0;
                    return returnVal;
                default: break;
            }
            break;
        case MaPLPrimitiveType_String:
            switch (literal.type.primitiveType) {
                case MaPLPrimitiveType_Char:
                    returnVal.stringValue = std::to_string(literal.charValue);
                    return returnVal;
                case MaPLPrimitiveType_Int32:
                    returnVal.stringValue = std::to_string(literal.int32Value);
                    return returnVal;
                case MaPLPrimitiveType_Int64: // Intentional fallthrough.
                case MaPLPrimitiveType_SignedInt_AmbiguousSize:
                    returnVal.stringValue = std::to_string(literal.int64Value);
                    return returnVal;
                case MaPLPrimitiveType_UInt32:
                    returnVal.stringValue = std::to_string(literal.uInt32Value);
                    return returnVal;
                case MaPLPrimitiveType_UInt64: // Intentional fallthrough.
                case MaPLPrimitiveType_Int_AmbiguousSizeAndSign:
                    returnVal.stringValue = std::to_string(literal.uInt64Value);
                    return returnVal;
                case MaPLPrimitiveType_Float32:
                    returnVal.stringValue = std::to_string(literal.float32Value);
                    return returnVal;
                case MaPLPrimitiveType_Float64: // Intentional fallthrough.
                case MaPLPrimitiveType_Float_AmbiguousSize:
                    returnVal.stringValue = std::to_string(literal.float64Value);
                    return returnVal;
                case MaPLPrimitiveType_String:
                    return literal;
                case MaPLPrimitiveType_Boolean:
                    returnVal.stringValue = literal.booleanValue ? "true" : "false";
                    return returnVal;
                case MaPLPrimitiveType_Pointer:
                    // NULL is the only possible literal pointer.
                    returnVal.stringValue = "0x0000000000000000";
                    return returnVal;
                default: break;
            }
            break;
        case MaPLPrimitiveType_Boolean:
            switch (literal.type.primitiveType) {
                case MaPLPrimitiveType_Char:
                    returnVal.booleanValue = (bool)literal.charValue;
                    return returnVal;
                case MaPLPrimitiveType_Int32:
                    returnVal.booleanValue = (bool)literal.int32Value;
                    return returnVal;
                case MaPLPrimitiveType_Int64: // Intentional fallthrough.
                case MaPLPrimitiveType_SignedInt_AmbiguousSize:
                    returnVal.booleanValue = (bool)literal.int64Value;
                    return returnVal;
                case MaPLPrimitiveType_UInt32:
                    returnVal.booleanValue = (bool)literal.uInt32Value;
                    return returnVal;
                case MaPLPrimitiveType_UInt64: // Intentional fallthrough.
                case MaPLPrimitiveType_Int_AmbiguousSizeAndSign:
                    returnVal.booleanValue = (bool)literal.uInt64Value;
                    return returnVal;
                case MaPLPrimitiveType_Float32:
                    returnVal.booleanValue = (bool)literal.float32Value;
                    return returnVal;
                case MaPLPrimitiveType_Float64: // Intentional fallthrough.
                case MaPLPrimitiveType_Float_AmbiguousSize:
                    returnVal.booleanValue = (bool)literal.float64Value;
                    return returnVal;
                case MaPLPrimitiveType_String:
                    returnVal.booleanValue = literal.stringValue == "true";
                    return returnVal;
                case MaPLPrimitiveType_Boolean:
                    return literal;
                default: break;
            }
            break;
        case MaPLPrimitiveType_Float_AmbiguousSize:
            switch (literal.type.primitiveType) {
                case MaPLPrimitiveType_Float_AmbiguousSize:
                    return literal;
                case MaPLPrimitiveType_SignedInt_AmbiguousSize:
                    returnVal.int64Value = (int64_t)literal.float64Value;
                    return returnVal;
                case MaPLPrimitiveType_Int_AmbiguousSizeAndSign:
                    returnVal.uInt64Value = (u_int64_t)literal.float64Value;
                    return returnVal;
                default: break;
            }
            break;
        case MaPLPrimitiveType_SignedInt_AmbiguousSize:
            switch (literal.type.primitiveType) {
                case MaPLPrimitiveType_SignedInt_AmbiguousSize:
                    return literal;
                case MaPLPrimitiveType_Int_AmbiguousSizeAndSign:
                    returnVal.uInt64Value = (u_int64_t)literal.int64Value;
                    return returnVal;
                default: break;
            }
            break;
        case MaPLPrimitiveType_Int_AmbiguousSizeAndSign:
            if (literal.type.primitiveType == MaPLPrimitiveType_Int_AmbiguousSizeAndSign) {
                return literal;
            }
            break;
        default: break;
    }
    return { { MaPLPrimitiveType_TypeError } };
}

/// https://stackoverflow.com/a/23000588
u_int8_t logBase2(uint64_t n)
{
    static const u_int8_t table[64] = {
        0, 58, 1, 59, 47, 53, 2, 60, 39, 48, 27, 54, 33, 42, 3, 61,
        51, 37, 40, 49, 18, 28, 20, 55, 30, 34, 11, 43, 14, 22, 4, 62,
        57, 46, 52, 38, 26, 32, 41, 50, 36, 17, 19, 29, 10, 13, 21, 56,
        45, 25, 31, 35, 16, 9, 12, 44, 24, 15, 8, 23, 7, 6, 5, 63
    };
    n |= n >> 1;
    n |= n >> 2;
    n |= n >> 4;
    n |= n >> 8;
    n |= n >> 16;
    n |= n >> 32;
    return table[(n * 0x03f6eaf2cd271461) >> 58];
}

u_int8_t bitShiftForLiteral(const MaPLLiteral &literal) {
    // Convert this literal to an unsigned int.
    u_int64_t unsignedValue;
    switch (literal.type.primitiveType) {
        case MaPLPrimitiveType_Char:
            unsignedValue = (u_int64_t)literal.charValue;
            break;
        case MaPLPrimitiveType_Int32:
            if (literal.int32Value <= 0) { return 0; }
            unsignedValue = (u_int64_t)literal.int32Value;
            break;
        case MaPLPrimitiveType_SignedInt_AmbiguousSize: // Intentional fallthrough.
        case MaPLPrimitiveType_Int64:
            if (literal.int64Value <= 0) { return 0; }
            unsignedValue = (u_int64_t)literal.int64Value;
            break;
        case MaPLPrimitiveType_UInt32:
            unsignedValue = (u_int64_t)literal.uInt32Value;
            break;
        case MaPLPrimitiveType_Int_AmbiguousSizeAndSign: // Intentional fallthrough.
        case MaPLPrimitiveType_UInt64:
            unsignedValue = literal.uInt64Value;
            break;
        default: return 0;
    }
    // Return the log2 only if it's an exact match.
    u_int8_t log2 = logBase2(unsignedValue);
    return 1 << log2 == unsignedValue ? log2 : 0;
}

MaPLParser::ObjectExpressionContext *terminalObjectExpression(MaPLParser::ObjectExpressionContext *rootExpression) {
    if (rootExpression->keyToken && rootExpression->keyToken->getType() == MaPLParser::OBJECT_TO_MEMBER) {
        // This expression is a compound expression. To find the last item in the chain of expressions, get the second child.
        return rootExpression->objectExpression(1);
    }
    return rootExpression;
}

MaPLParser::ObjectExpressionContext *prefixObjectExpression(MaPLParser::ObjectExpressionContext *rootExpression) {
    if (rootExpression->keyToken) {
        size_t tokenType = rootExpression->keyToken->getType();
        if (tokenType == MaPLParser::OBJECT_TO_MEMBER || tokenType == MaPLParser::SUBSCRIPT_OPEN) {
            return rootExpression->objectExpression(0);
        }
    }
    return NULL;
}

bool assignOperatorIsCompatibleWithType(MaPLFile *file, size_t operatorType, MaPLPrimitiveType type, antlr4::Token *token) {
    switch (operatorType) {
        case MaPLParser::ADD_ASSIGN:
            if (!isNumeric(type) && type != MaPLPrimitiveType_String) {
                file->logError(token, "This operator can only be used on numeric or string expressions.");
                return false;
            }
            break;
        case MaPLParser::SUBTRACT_ASSIGN:
        case MaPLParser::MULTIPLY_ASSIGN:
        case MaPLParser::DIVIDE_ASSIGN:
        case MaPLParser::MOD_ASSIGN:
        case MaPLParser::INCREMENT:
        case MaPLParser::DECREMENT:
            if (!isNumeric(type)) {
                file->logError(token, "This operator can only be used on numeric expressions.");
                return false;
            }
            break;
        case MaPLParser::BITWISE_AND_ASSIGN:
        case MaPLParser::BITWISE_OR_ASSIGN:
        case MaPLParser::BITWISE_XOR_ASSIGN:
        case MaPLParser::BITWISE_SHIFT_LEFT_ASSIGN:
        case MaPLParser::BITWISE_SHIFT_RIGHT_ASSIGN:
            if (!isIntegral(type)) {
                file->logError(token, "This operator can only be used on integral expressions.");
                return false;
            }
            break;
        default: break;
    }
    return true;
}

void flattenDependencies(MaPLFile *file, std::vector<MaPLFile *> &outputList, std::set<std::filesystem::path> &seenPaths) {
    for (MaPLFile *dependentFile : file->getDependencies()) {
        if (seenPaths.count(dependentFile->getNormalizedFilePath())) {
            continue;
        }
        seenPaths.insert(dependentFile->getNormalizedFilePath());
        flattenDependencies(dependentFile, outputList, seenPaths);
        outputList.push_back(dependentFile);
    }
}

std::vector<MaPLFile *> flattenedDependencies(MaPLFile *file) {
    std::vector<MaPLFile *> outputList;
    std::set<std::filesystem::path> seenPaths;
    flattenDependencies(file, outputList, seenPaths);
    return outputList;
}

std::map<std::string, MaPLSymbol> symbolTableForFiles(const std::vector<MaPLFile *> files) {
    // Populate the symbol table.
    std::map<std::string, MaPLSymbol> symbolTable;
    for (MaPLFile *file : files) {
        file->getAPI()->collateSymbolsInAPI(symbolTable);
    }
    
    // The table is now full of a sorted list of descriptors. Assign a unique ID to each.
    MaPLSymbol UUID = 1;
    for (const auto&[descriptor, symbol] : symbolTable) {
        symbolTable[descriptor] = UUID;
        UUID++;
    }
    return symbolTable;
}

bool isInsideLoopScope(antlr4::tree::ParseTree *node) {
    while (node) {
        if (dynamic_cast<MaPLParser::ScopeContext *>(node)) {
            antlr4::tree::ParseTree *scopeParent = node->parent;
            if (dynamic_cast<MaPLParser::ForLoopContext *>(scopeParent) ||
                dynamic_cast<MaPLParser::WhileLoopContext *>(scopeParent) ||
                dynamic_cast<MaPLParser::DoWhileLoopContext *>(scopeParent)) {
                return true;
            }
        }
        node = node->parent;
    }
    return false;
}

bool isTerminalImperativeObjectExpression(MaPLParser::ObjectExpressionContext *objectExpression) {
    MaPLParser::ObjectExpressionContext *parentObjectExpression = dynamic_cast<MaPLParser::ObjectExpressionContext *>(objectExpression->parent);
    if (parentObjectExpression && parentObjectExpression->keyToken->getType() == MaPLParser::OBJECT_TO_MEMBER) {
        if (objectExpression != parentObjectExpression->objectExpression(1)) {
            return false;
        }
        objectExpression = parentObjectExpression;
    }
    return dynamic_cast<MaPLParser::ImperativeStatementContext *>(objectExpression->parent) != NULL;
}

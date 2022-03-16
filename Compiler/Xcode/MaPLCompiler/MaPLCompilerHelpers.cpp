//
//  MaPLCompilerHelpers.cpp
//  MaPLCompiler
//
//  Created by Brad Bambara on 2/14/22.
//

#include <set>
#include <vector>

#include "MaPLCompilerHelpers.h"
#include "MaPLFile.h"

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
        case MaPLPrimitiveType_Int8: // Intentional fallthrough.
        case MaPLPrimitiveType_Int16: // Intentional fallthrough.
        case MaPLPrimitiveType_Int32: // Intentional fallthrough.
        case MaPLPrimitiveType_Int64:
            return true;
        default:
            return false;
    }
}

bool isConcreteUnsignedInt(MaPLPrimitiveType type) {
    switch (type) {
        case MaPLPrimitiveType_UInt8: // Intentional fallthrough.
        case MaPLPrimitiveType_UInt16: // Intentional fallthrough.
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

MaPL_Index byteSizeOfType(MaPLPrimitiveType type) {
    switch (type) {
        case MaPLPrimitiveType_Int8: // Intentional fallthrough.
        case MaPLPrimitiveType_UInt8: // Intentional fallthrough.
        case MaPLPrimitiveType_Boolean:
            return 1;
        case MaPLPrimitiveType_Int16: // Intentional fallthrough.
        case MaPLPrimitiveType_UInt16:
            return 2;
        case MaPLPrimitiveType_Int32: // Intentional fallthrough.
        case MaPLPrimitiveType_UInt32: // Intentional fallthrough.
        case MaPLPrimitiveType_Float32:
            return 4;
        case MaPLPrimitiveType_Int64: // Intentional fallthrough.
        case MaPLPrimitiveType_UInt64: // Intentional fallthrough.
        case MaPLPrimitiveType_Float64:
            return 8;
        case MaPLPrimitiveType_String:
            return sizeof(MaPL_String);
        case MaPLPrimitiveType_Pointer:
            return sizeof(void *);
        default: return 0;
    }
}

std::string descriptorForPrimitive(MaPLPrimitiveType type) {
    switch (type) {
        case MaPLPrimitiveType_Int8: return "int8";
        case MaPLPrimitiveType_Int16: return "int16";
        case MaPLPrimitiveType_Int32: return "int32";
        case MaPLPrimitiveType_Int64: return "int64";
        case MaPLPrimitiveType_UInt8: return "uint8";
        case MaPLPrimitiveType_UInt16: return "uint16";
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
        return type.pointerType.empty() ? "NULL" : type.pointerType;
    }
    return descriptorForPrimitive(type.primitiveType);
}

std::string descriptorForFunction(const std::string &name, const std::vector<MaPLType> &parameterTypes, bool hasVariadicArgs) {
    std::string functionDescriptor = name+"(";
    for (int i = 0; i < parameterTypes.size(); i++) {
        functionDescriptor += descriptorForType(parameterTypes[i]);
        if (i < parameterTypes.size()-1) {
            functionDescriptor += ", ";
        }
    }
    if (hasVariadicArgs) {
        functionDescriptor += ", ...";
    }
    functionDescriptor += ")";
    return functionDescriptor;
}

std::string descriptorForFunction(MaPLParser::ApiFunctionContext *function) {
    std::vector<MaPLType> parameterTypes;
    bool hasVariadicArgs = false;
    MaPLParser::ApiFunctionArgsContext *functionArgs = function->apiFunctionArgs();
    if (functionArgs) {
        hasVariadicArgs = functionArgs->API_VARIADIC_ARGUMENTS() != NULL;
        for (MaPLParser::TypeContext *typeContext : functionArgs->type()) {
            parameterTypes.push_back(typeForTypeContext(typeContext));
        }
    }
    return descriptorForFunction(function->identifier()->getText(),
                                 parameterTypes,
                                 hasVariadicArgs);
}

MaPL_Instruction assignmentInstructionForPrimitive(MaPLPrimitiveType type) {
    switch (type) {
        case MaPLPrimitiveType_Int8: return MAPL_BYTE_INT8_ASSIGN;
        case MaPLPrimitiveType_Int16: return MAPL_BYTE_INT16_ASSIGN;
        case MaPLPrimitiveType_Int32: return MAPL_BYTE_INT32_ASSIGN;
        case MaPLPrimitiveType_Int64: return MAPL_BYTE_INT64_ASSIGN;
        case MaPLPrimitiveType_UInt8: return MAPL_BYTE_UINT8_ASSIGN;
        case MaPLPrimitiveType_UInt16: return MAPL_BYTE_UINT16_ASSIGN;
        case MaPLPrimitiveType_UInt32: return MAPL_BYTE_UINT32_ASSIGN;
        case MaPLPrimitiveType_UInt64: return MAPL_BYTE_UINT64_ASSIGN;
        case MaPLPrimitiveType_Float32: return MAPL_BYTE_FLOAT32_ASSIGN;
        case MaPLPrimitiveType_Float64: return MAPL_BYTE_FLOAT64_ASSIGN;
        case MaPLPrimitiveType_Boolean: return MAPL_BYTE_BOOLEAN_ASSIGN;
        case MaPLPrimitiveType_String: return MAPL_BYTE_STRING_ASSIGN;
        case MaPLPrimitiveType_Pointer: return MAPL_BYTE_POINTER_ASSIGN;
        default: return 0;
    }
}

MaPL_Instruction typecastFromInstructionForPrimitive(MaPLPrimitiveType type) {
    switch (type) {
        case MaPLPrimitiveType_Int8: return MAPL_BYTE_TYPECAST_TO_INT8;
        case MaPLPrimitiveType_Int16: return MAPL_BYTE_TYPECAST_TO_INT16;
        case MaPLPrimitiveType_Int32: return MAPL_BYTE_TYPECAST_TO_INT32;
        case MaPLPrimitiveType_Int64: return MAPL_BYTE_TYPECAST_TO_INT64;
        case MaPLPrimitiveType_UInt8: return MAPL_BYTE_TYPECAST_TO_UINT8;
        case MaPLPrimitiveType_UInt16: return MAPL_BYTE_TYPECAST_TO_UINT16;
        case MaPLPrimitiveType_UInt32: return MAPL_BYTE_TYPECAST_TO_UINT32;
        case MaPLPrimitiveType_UInt64: return MAPL_BYTE_TYPECAST_TO_UINT64;
        case MaPLPrimitiveType_Float32: return MAPL_BYTE_TYPECAST_TO_FLOAT32;
        case MaPLPrimitiveType_Float64: return MAPL_BYTE_TYPECAST_TO_FLOAT64;
        case MaPLPrimitiveType_Boolean: return MAPL_BYTE_TYPECAST_TO_BOOLEAN;
        case MaPLPrimitiveType_String: return MAPL_BYTE_TYPECAST_TO_STRING;
        default: return 0;
    }
}

MaPL_Instruction typecastToInstructionForPrimitive(MaPLPrimitiveType type) {
    switch (type) {
        case MaPLPrimitiveType_Int8: return MAPL_BYTE_TYPECAST_FROM_INT8;
        case MaPLPrimitiveType_Int16: return MAPL_BYTE_TYPECAST_FROM_INT16;
        case MaPLPrimitiveType_Int32: return MAPL_BYTE_TYPECAST_FROM_INT32;
        case MaPLPrimitiveType_Int64: return MAPL_BYTE_TYPECAST_FROM_INT64;
        case MaPLPrimitiveType_UInt8: return MAPL_BYTE_TYPECAST_FROM_UINT8;
        case MaPLPrimitiveType_UInt16: return MAPL_BYTE_TYPECAST_FROM_UINT16;
        case MaPLPrimitiveType_UInt32: return MAPL_BYTE_TYPECAST_FROM_UINT32;
        case MaPLPrimitiveType_UInt64: return MAPL_BYTE_TYPECAST_FROM_UINT64;
        case MaPLPrimitiveType_Float32: return MAPL_BYTE_TYPECAST_FROM_FLOAT32;
        case MaPLPrimitiveType_Float64: return MAPL_BYTE_TYPECAST_FROM_FLOAT64;
        case MaPLPrimitiveType_Boolean: return MAPL_BYTE_TYPECAST_FROM_BOOLEAN;
        case MaPLPrimitiveType_String: return MAPL_BYTE_TYPECAST_FROM_STRING;
        default: return 0;
    }
}

MaPL_Instruction equalityInstructionForPrimitive(MaPLPrimitiveType type) {
    switch (type) {
        case MaPLPrimitiveType_Int8: return MAPL_BYTE_INT8_LOGICAL_EQUALITY;
        case MaPLPrimitiveType_Int16: return MAPL_BYTE_INT16_LOGICAL_EQUALITY;
        case MaPLPrimitiveType_Int32: return MAPL_BYTE_INT32_LOGICAL_EQUALITY;
        case MaPLPrimitiveType_Int64: return MAPL_BYTE_INT64_LOGICAL_EQUALITY;
        case MaPLPrimitiveType_UInt8: return MAPL_BYTE_UINT8_LOGICAL_EQUALITY;
        case MaPLPrimitiveType_UInt16: return MAPL_BYTE_UINT16_LOGICAL_EQUALITY;
        case MaPLPrimitiveType_UInt32: return MAPL_BYTE_UINT32_LOGICAL_EQUALITY;
        case MaPLPrimitiveType_UInt64: return MAPL_BYTE_UINT64_LOGICAL_EQUALITY;
        case MaPLPrimitiveType_Float32: return MAPL_BYTE_FLOAT32_LOGICAL_EQUALITY;
        case MaPLPrimitiveType_Float64: return MAPL_BYTE_FLOAT64_LOGICAL_EQUALITY;
        case MaPLPrimitiveType_Boolean: return MAPL_BYTE_BOOLEAN_LOGICAL_EQUALITY;
        case MaPLPrimitiveType_String: return MAPL_BYTE_STRING_LOGICAL_EQUALITY;
        case MaPLPrimitiveType_Pointer: return MAPL_BYTE_POINTER_LOGICAL_EQUALITY;
        default: return 0;
    }
}

MaPL_Instruction inequalityInstructionForPrimitive(MaPLPrimitiveType type) {
    switch (type) {
        case MaPLPrimitiveType_Int8: return MAPL_BYTE_INT8_LOGICAL_INEQUALITY;
        case MaPLPrimitiveType_Int16: return MAPL_BYTE_INT16_LOGICAL_INEQUALITY;
        case MaPLPrimitiveType_Int32: return MAPL_BYTE_INT32_LOGICAL_INEQUALITY;
        case MaPLPrimitiveType_Int64: return MAPL_BYTE_INT64_LOGICAL_INEQUALITY;
        case MaPLPrimitiveType_UInt8: return MAPL_BYTE_UINT8_LOGICAL_INEQUALITY;
        case MaPLPrimitiveType_UInt16: return MAPL_BYTE_UINT16_LOGICAL_INEQUALITY;
        case MaPLPrimitiveType_UInt32: return MAPL_BYTE_UINT32_LOGICAL_INEQUALITY;
        case MaPLPrimitiveType_UInt64: return MAPL_BYTE_UINT64_LOGICAL_INEQUALITY;
        case MaPLPrimitiveType_Float32: return MAPL_BYTE_FLOAT32_LOGICAL_INEQUALITY;
        case MaPLPrimitiveType_Float64: return MAPL_BYTE_FLOAT64_LOGICAL_INEQUALITY;
        case MaPLPrimitiveType_Boolean: return MAPL_BYTE_BOOLEAN_LOGICAL_INEQUALITY;
        case MaPLPrimitiveType_String: return MAPL_BYTE_STRING_LOGICAL_INEQUALITY;
        case MaPLPrimitiveType_Pointer: return MAPL_BYTE_POINTER_LOGICAL_INEQUALITY;
        default: return 0;
    }
}

MaPL_Instruction lessThanInstructionForPrimitive(MaPLPrimitiveType type) {
    switch (type) {
        case MaPLPrimitiveType_Int8: return MAPL_BYTE_INT8_LOGICAL_LESS_THAN;
        case MaPLPrimitiveType_Int16: return MAPL_BYTE_INT16_LOGICAL_LESS_THAN;
        case MaPLPrimitiveType_Int32: return MAPL_BYTE_INT32_LOGICAL_LESS_THAN;
        case MaPLPrimitiveType_Int64: return MAPL_BYTE_INT64_LOGICAL_LESS_THAN;
        case MaPLPrimitiveType_UInt8: return MAPL_BYTE_UINT8_LOGICAL_LESS_THAN;
        case MaPLPrimitiveType_UInt16: return MAPL_BYTE_UINT16_LOGICAL_LESS_THAN;
        case MaPLPrimitiveType_UInt32: return MAPL_BYTE_UINT32_LOGICAL_LESS_THAN;
        case MaPLPrimitiveType_UInt64: return MAPL_BYTE_UINT64_LOGICAL_LESS_THAN;
        case MaPLPrimitiveType_Float32: return MAPL_BYTE_FLOAT32_LOGICAL_LESS_THAN;
        case MaPLPrimitiveType_Float64: return MAPL_BYTE_FLOAT64_LOGICAL_LESS_THAN;
        default: return 0;
    }
}

MaPL_Instruction lessThanOrEqualInstructionForPrimitive(MaPLPrimitiveType type) {
    switch (type) {
        case MaPLPrimitiveType_Int8: return MAPL_BYTE_INT8_LOGICAL_LESS_THAN_EQUAL;
        case MaPLPrimitiveType_Int16: return MAPL_BYTE_INT16_LOGICAL_LESS_THAN_EQUAL;
        case MaPLPrimitiveType_Int32: return MAPL_BYTE_INT32_LOGICAL_LESS_THAN_EQUAL;
        case MaPLPrimitiveType_Int64: return MAPL_BYTE_INT64_LOGICAL_LESS_THAN_EQUAL;
        case MaPLPrimitiveType_UInt8: return MAPL_BYTE_UINT8_LOGICAL_LESS_THAN_EQUAL;
        case MaPLPrimitiveType_UInt16: return MAPL_BYTE_UINT16_LOGICAL_LESS_THAN_EQUAL;
        case MaPLPrimitiveType_UInt32: return MAPL_BYTE_UINT32_LOGICAL_LESS_THAN_EQUAL;
        case MaPLPrimitiveType_UInt64: return MAPL_BYTE_UINT64_LOGICAL_LESS_THAN_EQUAL;
        case MaPLPrimitiveType_Float32: return MAPL_BYTE_FLOAT32_LOGICAL_LESS_THAN_EQUAL;
        case MaPLPrimitiveType_Float64: return MAPL_BYTE_FLOAT64_LOGICAL_LESS_THAN_EQUAL;
        default: return 0;
    }
}

MaPL_Instruction greaterThanInstructionForPrimitive(MaPLPrimitiveType type) {
    switch (type) {
        case MaPLPrimitiveType_Int8: return MAPL_BYTE_INT8_LOGICAL_GREATER_THAN;
        case MaPLPrimitiveType_Int16: return MAPL_BYTE_INT16_LOGICAL_GREATER_THAN;
        case MaPLPrimitiveType_Int32: return MAPL_BYTE_INT32_LOGICAL_GREATER_THAN;
        case MaPLPrimitiveType_Int64: return MAPL_BYTE_INT64_LOGICAL_GREATER_THAN;
        case MaPLPrimitiveType_UInt8: return MAPL_BYTE_UINT8_LOGICAL_GREATER_THAN;
        case MaPLPrimitiveType_UInt16: return MAPL_BYTE_UINT16_LOGICAL_GREATER_THAN;
        case MaPLPrimitiveType_UInt32: return MAPL_BYTE_UINT32_LOGICAL_GREATER_THAN;
        case MaPLPrimitiveType_UInt64: return MAPL_BYTE_UINT64_LOGICAL_GREATER_THAN;
        case MaPLPrimitiveType_Float32: return MAPL_BYTE_FLOAT32_LOGICAL_GREATER_THAN;
        case MaPLPrimitiveType_Float64: return MAPL_BYTE_FLOAT64_LOGICAL_GREATER_THAN;
        default: return 0;
    }
}

MaPL_Instruction greaterThanOrEqualInstructionForPrimitive(MaPLPrimitiveType type) {
    switch (type) {
        case MaPLPrimitiveType_Int8: return MAPL_BYTE_INT8_LOGICAL_GREATER_THAN_EQUAL;
        case MaPLPrimitiveType_Int16: return MAPL_BYTE_INT16_LOGICAL_GREATER_THAN_EQUAL;
        case MaPLPrimitiveType_Int32: return MAPL_BYTE_INT32_LOGICAL_GREATER_THAN_EQUAL;
        case MaPLPrimitiveType_Int64: return MAPL_BYTE_INT64_LOGICAL_GREATER_THAN_EQUAL;
        case MaPLPrimitiveType_UInt8: return MAPL_BYTE_UINT8_LOGICAL_GREATER_THAN_EQUAL;
        case MaPLPrimitiveType_UInt16: return MAPL_BYTE_UINT16_LOGICAL_GREATER_THAN_EQUAL;
        case MaPLPrimitiveType_UInt32: return MAPL_BYTE_UINT32_LOGICAL_GREATER_THAN_EQUAL;
        case MaPLPrimitiveType_UInt64: return MAPL_BYTE_UINT64_LOGICAL_GREATER_THAN_EQUAL;
        case MaPLPrimitiveType_Float32: return MAPL_BYTE_FLOAT32_LOGICAL_GREATER_THAN_EQUAL;
        case MaPLPrimitiveType_Float64: return MAPL_BYTE_FLOAT64_LOGICAL_GREATER_THAN_EQUAL;
        default: return 0;
    }
}

MaPLType typeForTypeContext(MaPLParser::TypeContext *typeContext) {
    if (typeContext->identifier()) {
        return { MaPLPrimitiveType_Pointer, typeContext->identifier()->getText() };
    }
    switch (typeContext->start->getType()) {
        case MaPLParser::DECL_INT8: return { MaPLPrimitiveType_Int8 };
        case MaPLParser::DECL_INT16: return { MaPLPrimitiveType_Int16 };
        case MaPLParser::DECL_INT32: return { MaPLPrimitiveType_Int32 };
        case MaPLParser::DECL_INT64: return { MaPLPrimitiveType_Int64 };
        case MaPLParser::DECL_UINT8: return { MaPLPrimitiveType_UInt8 };
        case MaPLParser::DECL_UINT16: return { MaPLPrimitiveType_UInt16 };
        case MaPLParser::DECL_UINT32: return { MaPLPrimitiveType_UInt32 };
        case MaPLParser::DECL_UINT64: return { MaPLPrimitiveType_UInt64 };
        case MaPLParser::DECL_FLOAT32: return { MaPLPrimitiveType_Float32 };
        case MaPLParser::DECL_FLOAT64: return { MaPLPrimitiveType_Float64 };
        case MaPLParser::DECL_BOOL: return { MaPLPrimitiveType_Boolean };
        case MaPLParser::DECL_STRING: return { MaPLPrimitiveType_String };
        default: return { MaPLPrimitiveType_TypeError };
    }
}

MaPLLiteral castLiteralToType(const MaPLLiteral &literal, const MaPLType &castType) {
    MaPLLiteral returnVal = { { castType.primitiveType } };
    switch (castType.primitiveType) {
        case MaPLPrimitiveType_Int8:
            switch (literal.type.primitiveType) {
                case MaPLPrimitiveType_Int8:
                    return literal;
                case MaPLPrimitiveType_Int16:
                    returnVal.int8Value = (int8_t)literal.int16Value;
                    return returnVal;
                case MaPLPrimitiveType_Int32:
                    returnVal.int8Value = (int8_t)literal.int32Value;
                    return returnVal;
                case MaPLPrimitiveType_Int64: // Intentional fallthrough.
                case MaPLPrimitiveType_SignedInt_AmbiguousSize:
                    returnVal.int8Value = (int8_t)literal.int64Value;
                    return returnVal;
                case MaPLPrimitiveType_UInt8:
                    returnVal.int8Value = (int8_t)literal.uInt8Value;
                    return returnVal;
                case MaPLPrimitiveType_UInt16:
                    returnVal.int8Value = (int8_t)literal.uInt16Value;
                    return returnVal;
                case MaPLPrimitiveType_UInt32:
                    returnVal.int8Value = (int8_t)literal.uInt32Value;
                    return returnVal;
                case MaPLPrimitiveType_UInt64: // Intentional fallthrough.
                case MaPLPrimitiveType_Int_AmbiguousSizeAndSign:
                    returnVal.int8Value = (int8_t)literal.uInt64Value;
                    return returnVal;
                case MaPLPrimitiveType_Float32:
                    returnVal.int8Value = (int8_t)literal.float32Value;
                    return returnVal;
                case MaPLPrimitiveType_Float64: // Intentional fallthrough.
                case MaPLPrimitiveType_Float_AmbiguousSize:
                    returnVal.int8Value = (int8_t)literal.float64Value;
                    return returnVal;
                case MaPLPrimitiveType_String:
                    returnVal.int8Value = (int8_t)std::stoi(literal.stringValue);
                    return returnVal;
                case MaPLPrimitiveType_Boolean:
                    returnVal.int8Value = literal.booleanValue ? 1 : 0;
                    return returnVal;
                default: break;
            }
            break;
        case MaPLPrimitiveType_Int16:
            switch (literal.type.primitiveType) {
                case MaPLPrimitiveType_Int8:
                    returnVal.int16Value = (int16_t)literal.int8Value;
                    return returnVal;
                case MaPLPrimitiveType_Int16:
                    return literal;
                case MaPLPrimitiveType_Int32:
                    returnVal.int16Value = (int16_t)literal.int32Value;
                    return returnVal;
                case MaPLPrimitiveType_Int64: // Intentional fallthrough.
                case MaPLPrimitiveType_SignedInt_AmbiguousSize:
                    returnVal.int16Value = (int16_t)literal.int64Value;
                    return returnVal;
                case MaPLPrimitiveType_UInt8:
                    returnVal.int16Value = (int16_t)literal.uInt8Value;
                    return returnVal;
                case MaPLPrimitiveType_UInt16:
                    returnVal.int16Value = (int16_t)literal.uInt16Value;
                    return returnVal;
                case MaPLPrimitiveType_UInt32:
                    returnVal.int16Value = (int16_t)literal.uInt32Value;
                    return returnVal;
                case MaPLPrimitiveType_UInt64: // Intentional fallthrough.
                case MaPLPrimitiveType_Int_AmbiguousSizeAndSign:
                    returnVal.int16Value = (int16_t)literal.uInt64Value;
                    return returnVal;
                case MaPLPrimitiveType_Float32:
                    returnVal.int16Value = (int16_t)literal.float32Value;
                    return returnVal;
                case MaPLPrimitiveType_Float64: // Intentional fallthrough.
                case MaPLPrimitiveType_Float_AmbiguousSize:
                    returnVal.int16Value = (int16_t)literal.float64Value;
                    return returnVal;
                case MaPLPrimitiveType_String:
                    returnVal.int16Value = (int16_t)std::stoi(literal.stringValue);
                    return returnVal;
                case MaPLPrimitiveType_Boolean:
                    returnVal.int16Value = literal.booleanValue ? 1 : 0;
                    return returnVal;
                default: break;
            }
            break;
        case MaPLPrimitiveType_Int32:
            switch (literal.type.primitiveType) {
                case MaPLPrimitiveType_Int8:
                    returnVal.int32Value = (int32_t)literal.int8Value;
                    return returnVal;
                case MaPLPrimitiveType_Int16:
                    returnVal.int32Value = (int32_t)literal.int16Value;
                    return returnVal;
                case MaPLPrimitiveType_Int32:
                    return literal;
                case MaPLPrimitiveType_Int64: // Intentional fallthrough.
                case MaPLPrimitiveType_SignedInt_AmbiguousSize:
                    returnVal.int32Value = (int32_t)literal.int64Value;
                    return returnVal;
                case MaPLPrimitiveType_UInt8:
                    returnVal.int32Value = (int32_t)literal.uInt8Value;
                    return returnVal;
                case MaPLPrimitiveType_UInt16:
                    returnVal.int32Value = (int32_t)literal.uInt16Value;
                    return returnVal;
                case MaPLPrimitiveType_UInt32:
                    returnVal.int32Value = (int32_t)literal.uInt32Value;
                    return returnVal;
                case MaPLPrimitiveType_UInt64: // Intentional fallthrough.
                case MaPLPrimitiveType_Int_AmbiguousSizeAndSign:
                    returnVal.int32Value = (int32_t)literal.uInt64Value;
                    return returnVal;
                case MaPLPrimitiveType_Float32:
                    returnVal.int32Value = (int32_t)literal.float32Value;
                    return returnVal;
                case MaPLPrimitiveType_Float64: // Intentional fallthrough.
                case MaPLPrimitiveType_Float_AmbiguousSize:
                    returnVal.int32Value = (int32_t)literal.float64Value;
                    return returnVal;
                case MaPLPrimitiveType_String:
                    returnVal.int32Value = (int32_t)std::stoi(literal.stringValue);
                    return returnVal;
                case MaPLPrimitiveType_Boolean:
                    returnVal.int32Value = literal.booleanValue ? 1 : 0;
                    return returnVal;
                default: break;
            }
            break;
        case MaPLPrimitiveType_Int64:
            switch (literal.type.primitiveType) {
                case MaPLPrimitiveType_Int8:
                    returnVal.int64Value = (int64_t)literal.int8Value;
                    return returnVal;
                case MaPLPrimitiveType_Int16:
                    returnVal.int64Value = (int64_t)literal.int16Value;
                    return returnVal;
                case MaPLPrimitiveType_Int32:
                    returnVal.int64Value = (int64_t)literal.int32Value;
                    return returnVal;
                case MaPLPrimitiveType_Int64:
                    return literal;
                case MaPLPrimitiveType_SignedInt_AmbiguousSize:
                    returnVal.int64Value = literal.int64Value;
                    return returnVal;
                case MaPLPrimitiveType_UInt8:
                    returnVal.int64Value = (int64_t)literal.uInt8Value;
                    return returnVal;
                case MaPLPrimitiveType_UInt16:
                    returnVal.int64Value = (int64_t)literal.uInt16Value;
                    return returnVal;
                case MaPLPrimitiveType_UInt32:
                    returnVal.int64Value = (int64_t)literal.uInt32Value;
                    return returnVal;
                case MaPLPrimitiveType_UInt64: // Intentional fallthrough.
                case MaPLPrimitiveType_Int_AmbiguousSizeAndSign:
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
        case MaPLPrimitiveType_UInt8:
            switch (literal.type.primitiveType) {
                case MaPLPrimitiveType_Int8:
                    returnVal.uInt8Value = (u_int8_t)literal.int8Value;
                    return returnVal;
                case MaPLPrimitiveType_Int16:
                    returnVal.uInt8Value = (u_int8_t)literal.int16Value;
                    return returnVal;
                case MaPLPrimitiveType_Int32:
                    returnVal.uInt8Value = (u_int8_t)literal.int32Value;
                    return returnVal;
                case MaPLPrimitiveType_Int64: // Intentional fallthrough.
                case MaPLPrimitiveType_SignedInt_AmbiguousSize:
                    returnVal.uInt8Value = (u_int8_t)literal.int64Value;
                    return returnVal;
                case MaPLPrimitiveType_UInt8:
                    return literal;
                case MaPLPrimitiveType_UInt16:
                    returnVal.uInt8Value = (u_int8_t)literal.uInt16Value;
                    return returnVal;
                case MaPLPrimitiveType_UInt32:
                    returnVal.uInt8Value = (u_int8_t)literal.uInt32Value;
                    return returnVal;
                case MaPLPrimitiveType_UInt64: // Intentional fallthrough.
                case MaPLPrimitiveType_Int_AmbiguousSizeAndSign:
                    returnVal.uInt8Value = (u_int8_t)literal.uInt64Value;
                    return returnVal;
                case MaPLPrimitiveType_Float32:
                    returnVal.uInt8Value = (u_int8_t)literal.float32Value;
                    return returnVal;
                case MaPLPrimitiveType_Float64: // Intentional fallthrough.
                case MaPLPrimitiveType_Float_AmbiguousSize:
                    returnVal.uInt8Value = (u_int8_t)literal.float64Value;
                    return returnVal;
                case MaPLPrimitiveType_String:
                    returnVal.uInt8Value = (u_int8_t)std::stoul(literal.stringValue);
                    return returnVal;
                case MaPLPrimitiveType_Boolean:
                    returnVal.uInt8Value = literal.booleanValue ? 1 : 0;
                    return returnVal;
                default: break;
            }
            break;
        case MaPLPrimitiveType_UInt16:
            switch (literal.type.primitiveType) {
                case MaPLPrimitiveType_Int8:
                    returnVal.uInt16Value = (u_int16_t)literal.int8Value;
                    return returnVal;
                case MaPLPrimitiveType_Int16:
                    returnVal.uInt16Value = (u_int16_t)literal.int16Value;
                    return returnVal;
                case MaPLPrimitiveType_Int32:
                    returnVal.uInt16Value = (u_int16_t)literal.int32Value;
                    return returnVal;
                case MaPLPrimitiveType_Int64: // Intentional fallthrough.
                case MaPLPrimitiveType_SignedInt_AmbiguousSize:
                    returnVal.uInt16Value = (u_int16_t)literal.int64Value;
                    return returnVal;
                case MaPLPrimitiveType_UInt8:
                    returnVal.uInt16Value = (u_int16_t)literal.uInt8Value;
                    return returnVal;
                case MaPLPrimitiveType_UInt16:
                    return literal;
                case MaPLPrimitiveType_UInt32:
                    returnVal.uInt16Value = (u_int16_t)literal.uInt32Value;
                    return returnVal;
                case MaPLPrimitiveType_UInt64: // Intentional fallthrough.
                case MaPLPrimitiveType_Int_AmbiguousSizeAndSign:
                    returnVal.uInt16Value = (u_int16_t)literal.uInt64Value;
                    return returnVal;
                case MaPLPrimitiveType_Float32:
                    returnVal.uInt16Value = (u_int16_t)literal.float32Value;
                    return returnVal;
                case MaPLPrimitiveType_Float64: // Intentional fallthrough.
                case MaPLPrimitiveType_Float_AmbiguousSize:
                    returnVal.uInt16Value = (u_int16_t)literal.float64Value;
                    return returnVal;
                case MaPLPrimitiveType_String:
                    returnVal.uInt16Value = (u_int16_t)std::stoul(literal.stringValue);
                    return returnVal;
                case MaPLPrimitiveType_Boolean:
                    returnVal.uInt16Value = literal.booleanValue ? 1 : 0;
                    return returnVal;
                default: break;
            }
            break;
        case MaPLPrimitiveType_UInt32:
            switch (literal.type.primitiveType) {
                case MaPLPrimitiveType_Int8:
                    returnVal.uInt32Value = (u_int32_t)literal.int8Value;
                    return returnVal;
                case MaPLPrimitiveType_Int16:
                    returnVal.uInt32Value = (u_int32_t)literal.int16Value;
                    return returnVal;
                case MaPLPrimitiveType_Int32:
                    returnVal.uInt32Value = (u_int32_t)literal.int32Value;
                    return returnVal;
                case MaPLPrimitiveType_Int64: // Intentional fallthrough.
                case MaPLPrimitiveType_SignedInt_AmbiguousSize:
                    returnVal.uInt32Value = (u_int32_t)literal.int64Value;
                    return returnVal;
                case MaPLPrimitiveType_UInt8:
                    returnVal.uInt32Value = (u_int32_t)literal.uInt8Value;
                    return returnVal;
                case MaPLPrimitiveType_UInt16:
                    returnVal.uInt32Value = (u_int32_t)literal.uInt16Value;
                    return returnVal;
                case MaPLPrimitiveType_UInt32:
                    return literal;
                case MaPLPrimitiveType_UInt64: // Intentional fallthrough.
                case MaPLPrimitiveType_Int_AmbiguousSizeAndSign:
                    returnVal.uInt32Value = (u_int32_t)literal.uInt64Value;
                    return returnVal;
                case MaPLPrimitiveType_Float32:
                    returnVal.uInt32Value = (u_int32_t)literal.float32Value;
                    return returnVal;
                case MaPLPrimitiveType_Float64: // Intentional fallthrough.
                case MaPLPrimitiveType_Float_AmbiguousSize:
                    returnVal.uInt32Value = (u_int32_t)literal.float64Value;
                    return returnVal;
                case MaPLPrimitiveType_String:
                    returnVal.uInt32Value = (u_int32_t)std::stoul(literal.stringValue);
                    return returnVal;
                case MaPLPrimitiveType_Boolean:
                    returnVal.uInt32Value = literal.booleanValue ? 1 : 0;
                    return returnVal;
                default: break;
            }
            break;
        case MaPLPrimitiveType_UInt64:
            switch (literal.type.primitiveType) {
                case MaPLPrimitiveType_Int8:
                    returnVal.uInt64Value = (u_int64_t)literal.int8Value;
                    return returnVal;
                case MaPLPrimitiveType_Int16:
                    returnVal.uInt64Value = (u_int64_t)literal.int16Value;
                    return returnVal;
                case MaPLPrimitiveType_Int32:
                    returnVal.uInt64Value = (u_int64_t)literal.int32Value;
                    return returnVal;
                case MaPLPrimitiveType_Int64: // Intentional fallthrough.
                case MaPLPrimitiveType_SignedInt_AmbiguousSize:
                    returnVal.uInt64Value = (u_int64_t)literal.int64Value;
                    return returnVal;
                case MaPLPrimitiveType_UInt8:
                    returnVal.uInt64Value = (u_int64_t)literal.uInt8Value;
                    return returnVal;
                case MaPLPrimitiveType_UInt16:
                    returnVal.uInt64Value = (u_int64_t)literal.uInt16Value;
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
                case MaPLPrimitiveType_Int8:
                    returnVal.float32Value = (float_t)literal.int8Value;
                    return returnVal;
                case MaPLPrimitiveType_Int16:
                    returnVal.float32Value = (float_t)literal.int16Value;
                    return returnVal;
                case MaPLPrimitiveType_Int32:
                    returnVal.float32Value = (float_t)literal.int32Value;
                    return returnVal;
                case MaPLPrimitiveType_Int64: // Intentional fallthrough.
                case MaPLPrimitiveType_SignedInt_AmbiguousSize:
                    returnVal.float32Value = (float_t)literal.int64Value;
                    return returnVal;
                case MaPLPrimitiveType_UInt8:
                    returnVal.float32Value = (float_t)literal.uInt8Value;
                    return returnVal;
                case MaPLPrimitiveType_UInt16:
                    returnVal.float32Value = (float_t)literal.uInt16Value;
                    return returnVal;
                case MaPLPrimitiveType_UInt32:
                    returnVal.float32Value = (float_t)literal.uInt32Value;
                    return returnVal;
                case MaPLPrimitiveType_UInt64: // Intentional fallthrough.
                case MaPLPrimitiveType_Int_AmbiguousSizeAndSign:
                    returnVal.float32Value = (float_t)literal.uInt64Value;
                    return returnVal;
                case MaPLPrimitiveType_Float32:
                    return literal;
                case MaPLPrimitiveType_Float64: // Intentional fallthrough.
                case MaPLPrimitiveType_Float_AmbiguousSize:
                    returnVal.float32Value = (float_t)literal.float64Value;
                    return returnVal;
                case MaPLPrimitiveType_String:
                    returnVal.float32Value = (float_t)std::stof(literal.stringValue);
                    return returnVal;
                case MaPLPrimitiveType_Boolean:
                    returnVal.float32Value = literal.booleanValue ? 1 : 0;
                    return returnVal;
                default: break;
            }
            break;
        case MaPLPrimitiveType_Float64:
            switch (literal.type.primitiveType) {
                case MaPLPrimitiveType_Int8:
                    returnVal.float64Value = (double_t)literal.int8Value;
                    return returnVal;
                case MaPLPrimitiveType_Int16:
                    returnVal.float64Value = (double_t)literal.int16Value;
                    return returnVal;
                case MaPLPrimitiveType_Int32:
                    returnVal.float64Value = (double_t)literal.int32Value;
                    return returnVal;
                case MaPLPrimitiveType_Int64: // Intentional fallthrough.
                case MaPLPrimitiveType_SignedInt_AmbiguousSize:
                    returnVal.float64Value = (double_t)literal.int64Value;
                    return returnVal;
                case MaPLPrimitiveType_UInt8:
                    returnVal.float64Value = (double_t)literal.uInt8Value;
                    return returnVal;
                case MaPLPrimitiveType_UInt16:
                    returnVal.float64Value = (double_t)literal.uInt16Value;
                    return returnVal;
                case MaPLPrimitiveType_UInt32:
                    returnVal.float64Value = (double_t)literal.uInt32Value;
                    return returnVal;
                case MaPLPrimitiveType_UInt64: // Intentional fallthrough.
                case MaPLPrimitiveType_Int_AmbiguousSizeAndSign:
                    returnVal.float64Value = (double_t)literal.uInt64Value;
                    return returnVal;
                case MaPLPrimitiveType_Float32:
                    returnVal.float64Value = (double_t)literal.float32Value;
                    return returnVal;
                case MaPLPrimitiveType_Float64:
                    return literal;
                case MaPLPrimitiveType_Float_AmbiguousSize:
                    returnVal.float64Value = literal.float64Value;
                    return returnVal;
                case MaPLPrimitiveType_String:
                    returnVal.float64Value = (double_t)std::stod(literal.stringValue);
                    return returnVal;
                case MaPLPrimitiveType_Boolean:
                    returnVal.float64Value = literal.booleanValue ? 1 : 0;
                    return returnVal;
                default: break;
            }
            break;
        case MaPLPrimitiveType_String:
            switch (literal.type.primitiveType) {
                case MaPLPrimitiveType_Int8:
                    returnVal.stringValue = std::to_string(literal.int8Value);
                    return returnVal;
                case MaPLPrimitiveType_Int16:
                    returnVal.stringValue = std::to_string(literal.int16Value);
                    return returnVal;
                case MaPLPrimitiveType_Int32:
                    returnVal.stringValue = std::to_string(literal.int32Value);
                    return returnVal;
                case MaPLPrimitiveType_Int64: // Intentional fallthrough.
                case MaPLPrimitiveType_SignedInt_AmbiguousSize:
                    returnVal.stringValue = std::to_string(literal.int64Value);
                    return returnVal;
                case MaPLPrimitiveType_UInt8:
                    returnVal.stringValue = std::to_string(literal.uInt8Value);
                    return returnVal;
                case MaPLPrimitiveType_UInt16:
                    returnVal.stringValue = std::to_string(literal.uInt16Value);
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
                case MaPLPrimitiveType_Int8:
                    returnVal.booleanValue = (bool)literal.int8Value;
                    return returnVal;
                case MaPLPrimitiveType_Int16:
                    returnVal.booleanValue = (bool)literal.int16Value;
                    return returnVal;
                case MaPLPrimitiveType_Int32:
                    returnVal.booleanValue = (bool)literal.int32Value;
                    return returnVal;
                case MaPLPrimitiveType_Int64: // Intentional fallthrough.
                case MaPLPrimitiveType_SignedInt_AmbiguousSize:
                    returnVal.booleanValue = (bool)literal.int64Value;
                    return returnVal;
                case MaPLPrimitiveType_UInt8:
                    returnVal.booleanValue = (bool)literal.uInt8Value;
                    return returnVal;
                case MaPLPrimitiveType_UInt16:
                    returnVal.booleanValue = (bool)literal.uInt16Value;
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

bool isAssignable(MaPLFile *file, const MaPLType &expressionType, const MaPLType &assignToType) {
    // Handle direct matches first.
    if (assignToType.primitiveType == expressionType.primitiveType) {
        if (assignToType.primitiveType == MaPLPrimitiveType_Pointer) {
            // Empty "pointerType" indicates a NULL literal. Nulls are assignable to any type of pointer.
            return expressionType.pointerType.empty() ||
                   assignToType.pointerType == expressionType.pointerType ||
                   inheritsFromType(file, expressionType.pointerType, assignToType.pointerType);
        }
        return true;
    }
    // Handle all the ways that a concrete numeric type could accept ambiguity from the expression.
    switch (assignToType.primitiveType) {
        case MaPLPrimitiveType_UInt8: // Intentional fallthrough.
        case MaPLPrimitiveType_UInt16: // Intentional fallthrough.
        case MaPLPrimitiveType_UInt32: // Intentional fallthrough.
        case MaPLPrimitiveType_UInt64:
            return expressionType.primitiveType == MaPLPrimitiveType_Int_AmbiguousSizeAndSign;
        case MaPLPrimitiveType_Int8: // Intentional fallthrough.
        case MaPLPrimitiveType_Int16: // Intentional fallthrough.
        case MaPLPrimitiveType_Int32: // Intentional fallthrough.
        case MaPLPrimitiveType_Int64:
            return expressionType.primitiveType == MaPLPrimitiveType_SignedInt_AmbiguousSize ||
                   expressionType.primitiveType == MaPLPrimitiveType_Int_AmbiguousSizeAndSign;
        case MaPLPrimitiveType_Float32: // Intentional fallthrough.
        case MaPLPrimitiveType_Float64:
            return expressionType.primitiveType == MaPLPrimitiveType_Float_AmbiguousSize ||
                   expressionType.primitiveType == MaPLPrimitiveType_SignedInt_AmbiguousSize ||
                   expressionType.primitiveType == MaPLPrimitiveType_Int_AmbiguousSizeAndSign;
        default: return false;
    }
}

std::set<std::string> findAncestorTypes(MaPLFile *file, std::string type) {
    std::set<std::string> allAncestorTypes;
    MaPLParser::ApiDeclarationContext *typeContext = findType(file, type, NULL);
    if (!typeContext) {
        return allAncestorTypes;
    }
    MaPLParser::ApiInheritanceContext *inheritance = typeContext->apiInheritance();
    if (!inheritance) {
        return allAncestorTypes;
    }
    for (MaPLParser::IdentifierContext *identifier : inheritance->identifier()) {
        std::string parentType = identifier->getText();
        allAncestorTypes.insert(parentType);
        std::set<std::string> ancestorTypes = findAncestorTypes(file, parentType);
        allAncestorTypes.insert(ancestorTypes.begin(), ancestorTypes.end());
    }
    return allAncestorTypes;
}

std::vector<std::string> mutualAncestorTypes(MaPLFile *file, const std::string &type1, const std::string &type2) {
    std::set<std::string> ancestors1 = findAncestorTypes(file, type1);
    std::set<std::string> ancestors2 = findAncestorTypes(file, type2);
    std::vector<std::string> mutuals;
    for (std::string ancestorType : ancestors1) {
        if (ancestors2.count(ancestorType)) {
            mutuals.push_back(ancestorType);
        }
    }
    return mutuals;
}

bool inheritsFromType(MaPLFile *file, const std::string &type, const std::string &possibleAncestorType) {
    return findAncestorTypes(file, type).count(possibleAncestorType) > 0;
}

std::vector<std::string> findInheritanceCycle(MaPLFile *file, MaPLParser::ApiDeclarationContext *apiDeclaration, std::set<std::string> &seenTypes) {
    std::vector<std::string> cycleVector;
    std::string typeName = apiDeclaration->identifier()->getText();
    if (seenTypes.count(typeName) > 0) {
        // This graph contains a cycle.
        cycleVector.push_back(typeName);
        return cycleVector;
    }
    MaPLParser::ApiInheritanceContext *inheritance = apiDeclaration->apiInheritance();
    if (!inheritance) {
        return cycleVector;
    }
    seenTypes.insert(typeName);
    for (MaPLParser::IdentifierContext *identifier : inheritance->identifier()) {
        MaPLParser::ApiDeclarationContext *parentDeclaration = findType(file, identifier->getText(), NULL);
        if (!parentDeclaration) {
            continue;
        }
        std::vector<std::string> foundCycle = findInheritanceCycle(file, parentDeclaration, seenTypes);
        if (foundCycle.size() > 0) {
            cycleVector.push_back(typeName);
            cycleVector.insert(cycleVector.end(), foundCycle.begin(), foundCycle.end());
            break;
        }
    }
    seenTypes.erase(typeName);
    return cycleVector;
}

bool findInheritanceCycle(MaPLFile *file) {
    MaPLParser::ProgramContext *program = file->getParseTree();
    if (!program) { return false; }
    std::set<std::string> seenTypes;
    for (MaPLParser::StatementContext *statement : program->statement()) {
        MaPLParser::ApiDeclarationContext *apiDeclaration = statement->apiDeclaration();
        if (!apiDeclaration ||
            apiDeclaration->keyToken->getType() != MaPLParser::API_TYPE) {
            continue;
        }
        std::vector<std::string> cycle = findInheritanceCycle(file, apiDeclaration, seenTypes);
        if (cycle.size() > 0) {
            std::string cycleDescriptor;
            for (int i = 0; i < cycle.size(); i++) {
                cycleDescriptor += cycle[i];
                if (i < cycle.size()-1) {
                    cycleDescriptor += " -> ";
                }
            }
            cycleDescriptor += ".";
            logError(file, apiDeclaration->identifier()->start, "Type inheritance forms a cycle: "+cycleDescriptor);
            return true;
        }
        seenTypes.clear();
    }
    return false;
}

std::set<std::filesystem::path> findDuplicateDependencies(MaPLFile *file, std::set<std::filesystem::path> &pathSet) {
    std::set<std::filesystem::path> returnPaths;
    if (pathSet.count(file->getNormalizedFilePath())) {
        returnPaths.insert(file->getNormalizedFilePath());
        return returnPaths;
    } else {
        pathSet.insert(file->getNormalizedFilePath());
    }
    for (MaPLFile *dependentFile : file->getDependencies()) {
        for (std::filesystem::path duplicatePath : findDuplicateDependencies(dependentFile, pathSet)) {
            returnPaths.insert(duplicatePath);
        }
    }
    return returnPaths;
}

std::set<std::filesystem::path> findDuplicateDependencies(MaPLFile *file) {
    std::set<std::filesystem::path> pathSet;
    return findDuplicateDependencies(file, pathSet);
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

MaPLParser::ApiDeclarationContext *findType(MaPLFile *file, const std::string &type, MaPLParser::ApiDeclarationContext *excludingType) {
    MaPLParser::ProgramContext *program = file->getParseTree();
    if (!program) { return NULL; }
    for (MaPLParser::StatementContext *statement : program->statement()) {
        MaPLParser::ApiDeclarationContext *apiDeclaration = statement->apiDeclaration();
        if (!apiDeclaration ||
            (excludingType && excludingType == apiDeclaration) ||
            apiDeclaration->keyToken->getType() != MaPLParser::API_TYPE) {
            continue;
        }
        MaPLParser::IdentifierContext *identifier = apiDeclaration->identifier();
        if (identifier && identifier->getText() == type) {
            return apiDeclaration;
        }
    }
    for (MaPLFile *dependency : file->getDependencies()) {
        MaPLParser::ApiDeclarationContext *apiDeclaration = findType(dependency, type, excludingType);
        if (apiDeclaration) {
            return apiDeclaration;
        }
    }
    return NULL;
}

bool functionIsCompatible(MaPLFile *file,
                          MaPLParser::ApiFunctionContext *function,
                          const std::string &name,
                          const std::vector<MaPLType> &parameterTypes,
                          MaPLParameterStrategy parameterStrategy) {
    // Confirm matching function name.
    if (name != function->identifier()->getText()) {
        return false;
    }
    
    // Confirm matching number of arguments.
    MaPLParser::ApiFunctionArgsContext *args = function->apiFunctionArgs();
    if (!args) {
        return parameterTypes.size() == 0;
    }
    bool hasVariadicArgs = args->API_VARIADIC_ARGUMENTS() != NULL;
    std::vector<MaPLParser::TypeContext *> typeContexts = args->type();
    if (parameterStrategy == MaPLParameterStrategy_Flexible) {
        // It's possible to match longer parameter lists, but only if this function has variadic args.
        if ((!hasVariadicArgs && typeContexts.size() != parameterTypes.size()) ||
            (hasVariadicArgs && typeContexts.size() > parameterTypes.size())) {
            return false;
        }
        
        // Confirm assignable types for all arguments.
        for (int32_t i = 0; i < typeContexts.size(); i++) {
            if (!isAssignable(file, parameterTypes[i], typeForTypeContext(typeContexts[i]))) {
                return false;
            }
        }
    } else {
        // This strategy requires an exact match in number of args.
        if (typeContexts.size() != parameterTypes.size()) {
            return false;
        }
        bool expectingVariadicArgs = parameterStrategy == MaPLParameterStrategy_Exact_IncludeVariadicArgs;
        if (expectingVariadicArgs != hasVariadicArgs) {
            return false;
        }
        
        // Confirm exact matching types for all arguments.
        for (int32_t i = 0; i < typeContexts.size(); i++) {
            MaPLType searchedParamType = parameterTypes[i];
            MaPLType declaredParamType = typeForTypeContext(typeContexts[i]);
            if (searchedParamType.primitiveType != declaredParamType.primitiveType ||
                (searchedParamType.primitiveType == MaPLPrimitiveType_Pointer && searchedParamType.pointerType != declaredParamType.pointerType)) {
                return false;
            }
        }
    }
    
    return true;
}

MaPLParser::ApiFunctionContext *findFunction(MaPLFile *file,
                                             const std::string &type,
                                             const std::string &name,
                                             const std::vector<MaPLType> &parameterTypes,
                                             MaPLParameterStrategy parameterStrategy,
                                             MaPLParser::ApiFunctionContext *excludingFunction) {
    MaPLParser::ProgramContext *program = file->getParseTree();
    if (!program) { return NULL; }
    
    // Search all API function declarations in this file.
    bool isGlobal = type.empty();
    for (MaPLParser::StatementContext *statement : program->statement()) {
        MaPLParser::ApiDeclarationContext *apiDeclaration = statement->apiDeclaration();
        if (!apiDeclaration) { continue; }
        switch (apiDeclaration->keyToken->getType()) {
            case MaPLParser::API_GLOBAL: {
                if (!isGlobal) { continue; }
                MaPLParser::ApiFunctionContext *function = apiDeclaration->apiFunction(0);
                if (!function || (excludingFunction && excludingFunction == function)) {
                    continue;
                }
                if (functionIsCompatible(file, function, name, parameterTypes, parameterStrategy)) {
                    return function;
                }
            }
                break;
            case MaPLParser::API_TYPE: {
                if (isGlobal) { continue; }
                MaPLParser::IdentifierContext *identifier = apiDeclaration->identifier();
                if (!identifier || identifier->getText() != type) {
                    continue;
                }
                for (MaPLParser::ApiFunctionContext *function : apiDeclaration->apiFunction()) {
                    if (excludingFunction && excludingFunction == function) {
                        continue;
                    }
                    if (functionIsCompatible(file, function, name, parameterTypes, parameterStrategy)) {
                        return function;
                    }
                }
                MaPLParser::ApiInheritanceContext *inheritance = apiDeclaration->apiInheritance();
                if (inheritance) {
                    for (MaPLParser::IdentifierContext *identifier : inheritance->identifier()) {
                        MaPLParser::ApiFunctionContext *foundFunction = findFunction(file,
                                                                                     identifier->getText(),
                                                                                     name,
                                                                                     parameterTypes,
                                                                                     parameterStrategy,
                                                                                     excludingFunction);
                        if (foundFunction) {
                            return foundFunction;
                        }
                    }
                }
            }
                break;
            default:
                break;
        }
    }
    
    // If no matching function was found in this file, recurse through all dependent files.
    for (MaPLFile *dependency : file->getDependencies()) {
        MaPLParser::ApiFunctionContext *foundFunction = findFunction(dependency,
                                                                     type,
                                                                     name,
                                                                     parameterTypes,
                                                                     parameterStrategy,
                                                                     excludingFunction);
        if (foundFunction) {
            return foundFunction;
        }
    }
    
    return NULL;
}

MaPLParser::ApiSubscriptContext *findSubscript(MaPLFile *file,
                                               const std::string &type,
                                               const MaPLType &indexType,
                                               MaPLParser::ApiSubscriptContext *excludingSubscript) {
    MaPLParser::ApiDeclarationContext *typeDeclaration = findType(file, type, NULL);
    if (!typeDeclaration) {
        return NULL;
    }
    for (MaPLParser::ApiSubscriptContext *subscript : typeDeclaration->apiSubscript()) {
        if (excludingSubscript && excludingSubscript == subscript) {
            continue;
        }
        if (isAssignable(file, indexType, typeForTypeContext(subscript->type(1)))) {
            return subscript;
        }
    }
    MaPLParser::ApiInheritanceContext *inheritance = typeDeclaration->apiInheritance();
    if (inheritance) {
        for (MaPLParser::IdentifierContext *identifier : inheritance->identifier()) {
            MaPLParser::ApiSubscriptContext *foundSubscript = findSubscript(file, identifier->getText(), indexType, excludingSubscript);
            if (foundSubscript) {
                return foundSubscript;
            }
        }
    }
    return NULL;
}

MaPLParser::ApiPropertyContext *findProperty(MaPLFile *file,
                                             const std::string &type,
                                             const std::string &name,
                                             MaPLParser::ApiPropertyContext *excludingProperty) {
    MaPLParser::ProgramContext *program = file->getParseTree();
    if (!program) { return NULL; }
    
    // Search all API property declarations in this file.
    bool isGlobal = type.empty();
    for (MaPLParser::StatementContext *statement : program->statement()) {
        MaPLParser::ApiDeclarationContext *apiDeclaration = statement->apiDeclaration();
        if (!apiDeclaration) { continue; }
        switch (apiDeclaration->keyToken->getType()) {
            case MaPLParser::API_GLOBAL: {
                if (!isGlobal) { continue; }
                MaPLParser::ApiPropertyContext *property = apiDeclaration->apiProperty(0);
                if (!property || (excludingProperty && excludingProperty == property)) { continue; }
                MaPLParser::IdentifierContext *identifier = property->identifier();
                if (identifier && identifier->getText() == name) {
                    return property;
                }
            }
                break;
            case MaPLParser::API_TYPE: {
                if (isGlobal) { continue; }
                MaPLParser::IdentifierContext *identifier = apiDeclaration->identifier();
                if (!identifier || identifier->getText() != type) {
                    continue;
                }
                for (MaPLParser::ApiPropertyContext *property : apiDeclaration->apiProperty()) {
                    if (excludingProperty && excludingProperty == property) {
                        continue;
                    }
                    MaPLParser::IdentifierContext *identifier = property->identifier();
                    if (identifier && identifier->getText() == name) {
                        return property;
                    }
                }
                MaPLParser::ApiInheritanceContext *inheritance = apiDeclaration->apiInheritance();
                if (inheritance) {
                    for (MaPLParser::IdentifierContext *identifier : inheritance->identifier()) {
                        MaPLParser::ApiPropertyContext *foundProperty = findProperty(file, identifier->getText(), name, excludingProperty);
                        if (foundProperty) {
                            return foundProperty;
                        }
                    }
                }
            }
                break;
            default:
                break;
        }
    }
    
    // If no matching property was found in this file, recurse through all dependent files.
    for (MaPLFile *dependency : file->getDependencies()) {
        MaPLParser::ApiPropertyContext *foundProperty = findProperty(dependency, type, name, excludingProperty);
        if (foundProperty) {
            return foundProperty;
        }
    }
    
    return NULL;
}

void logAmbiguousLiteralError(MaPLFile *file, MaPLPrimitiveType type, antlr4::Token *token) {
    if (!isAmbiguousNumericType(type)) {
        return;
    }
    std::string message = "This expression contains numeric literals whose type is ambiguous. An explicit cast must be added to distinguish between: ";
    std::vector<MaPLPrimitiveType> suggestedTypes;
    switch (type) {
        case MaPLPrimitiveType_Int_AmbiguousSizeAndSign:
            suggestedTypes.push_back(MaPLPrimitiveType_UInt8);
            suggestedTypes.push_back(MaPLPrimitiveType_UInt16);
            suggestedTypes.push_back(MaPLPrimitiveType_UInt32);
            suggestedTypes.push_back(MaPLPrimitiveType_UInt64);
            // Intentional fallthrough.
        case MaPLPrimitiveType_SignedInt_AmbiguousSize:
            suggestedTypes.push_back(MaPLPrimitiveType_Int8);
            suggestedTypes.push_back(MaPLPrimitiveType_Int16);
            suggestedTypes.push_back(MaPLPrimitiveType_Int32);
            suggestedTypes.push_back(MaPLPrimitiveType_Int64);
            // Intentional fallthrough.
        case MaPLPrimitiveType_Float_AmbiguousSize:
            suggestedTypes.push_back(MaPLPrimitiveType_Float32);
            suggestedTypes.push_back(MaPLPrimitiveType_Float64);
            break;
        default: return;
    }
    for (int i = 0; i < suggestedTypes.size(); i++) {
        message += descriptorForType({ suggestedTypes[i] });
        if (i == suggestedTypes.size()-1) {
            message += ".";
        } else {
            message += ", ";
        }
    }
    logError(file, token, message);
}

void logNonNumericOperandsError(MaPLFile *file, antlr4::Token *token) {
    logError(file, token, "Both operands must be numeric.");
}

void logError(MaPLFile *file, antlr4::Token *token, const std::string &msg) {
    if (token) {
        printf("%s %ld:%ld: %s\n", file->getNormalizedFilePath().c_str(), token->getLine(), token->getCharPositionInLine(), msg.c_str());
    } else {
        printf("%s: %s\n", file->getNormalizedFilePath().c_str(), msg.c_str());
    }
}

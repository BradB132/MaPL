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
        case MaPLPrimitiveType_Float_AmbiguousSize:
        case MaPLPrimitiveType_SignedInt_AmbiguousSize:
        case MaPLPrimitiveType_Int_AmbiguousSizeAndSign:
            return true;
        default:
            return false;
    }
}

bool isFloat(MaPLPrimitiveType type) {
    switch (type) {
        case MaPLPrimitiveType_Float32:
        case MaPLPrimitiveType_Float64:
            return true;
        default:
            return false;
    }
}

bool isSignedInt(MaPLPrimitiveType type) {
    switch (type) {
        case MaPLPrimitiveType_Int8:
        case MaPLPrimitiveType_Int16:
        case MaPLPrimitiveType_Int32:
        case MaPLPrimitiveType_Int64:
            return true;
        default:
            return false;
    }
}

bool isUnsignedInt(MaPLPrimitiveType type) {
    switch (type) {
        case MaPLPrimitiveType_UInt8:
        case MaPLPrimitiveType_UInt16:
        case MaPLPrimitiveType_UInt32:
        case MaPLPrimitiveType_UInt64:
            return true;
        default:
            return false;
    }
}

bool isIntegral(MaPLPrimitiveType type) {
    return isSignedInt(type) ||
           isUnsignedInt(type) ||
           type == MaPLPrimitiveType_Int_AmbiguousSizeAndSign ||
           type == MaPLPrimitiveType_SignedInt_AmbiguousSize;
}

bool isNumeric(MaPLPrimitiveType type) {
    return isFloat(type) ||
           isSignedInt(type) ||
           isUnsignedInt(type) ||
           isAmbiguousNumericType(type);
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

std::string descriptorForType(MaPLType type) {
    switch (type.primitiveType) {
        case MaPLPrimitiveType_Int8: return "'int8'";
        case MaPLPrimitiveType_Int16: return "'int16'";
        case MaPLPrimitiveType_Int32: return "'int32'";
        case MaPLPrimitiveType_Int64: return "'int64'";
        case MaPLPrimitiveType_UInt8: return "'uint8'";
        case MaPLPrimitiveType_UInt16: return "'uint16'";
        case MaPLPrimitiveType_UInt32: return "'uint32'";
        case MaPLPrimitiveType_UInt64: return "'uint64'";
        case MaPLPrimitiveType_Float32: return "'float32'";
        case MaPLPrimitiveType_Float64: return "'float64'";
        case MaPLPrimitiveType_Boolean: return "'bool'";
        case MaPLPrimitiveType_String: return "'string'";
        case MaPLPrimitiveType_Pointer: return "'"+type.pointerType+"' pointer";
        case MaPLPrimitiveType_SignedInt_AmbiguousSize: return "signed integer";
        case MaPLPrimitiveType_Int_AmbiguousSizeAndSign: return "integer";
        case MaPLPrimitiveType_Float_AmbiguousSize: return "floating point";
        case MaPLPrimitiveType_Void: return "'void'";
        case MaPLPrimitiveType_InvalidType: return "invalid type";
    }
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
        default: return { MaPLPrimitiveType_InvalidType };
    }
}

bool isAssignable(MaPLFile *file, MaPLType expressionType, MaPLType assignToType) {
    // Handle direct matches first.
    if (assignToType.primitiveType == expressionType.primitiveType) {
        if (assignToType.primitiveType == MaPLPrimitiveType_Pointer) {
            // Empty "pointerType" indicates a null literal. Nulls are assignable to any type of pointer.
            return expressionType.pointerType == "" ||
                   assignToType.pointerType == expressionType.pointerType ||
                   inheritsFromType(file, expressionType.pointerType, assignToType.pointerType);
        }
        return true;
    }
    // Handle all the ways that the concrete type could accept ambiguity from the expression.
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

std::vector<std::string> mutualAncestorTypes(MaPLFile *file, std::string type1, std::string type2) {
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

bool inheritsFromType(MaPLFile *file, std::string type, std::string possibleAncestorType) {
    return findAncestorTypes(file, type).count(possibleAncestorType) > 0;
}

MaPLParser::ApiDeclarationContext *findType(MaPLFile *file, std::string type, MaPLParser::ApiDeclarationContext *excludingType) {
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
                          std::string name,
                          std::vector<MaPLType> parameterTypes) {
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
    if ((!hasVariadicArgs && typeContexts.size() != parameterTypes.size()) ||
        (hasVariadicArgs && typeContexts.size() > parameterTypes.size())) {
        return false;
    }
    // Confirm compatible types for all arguments.
    for (int32_t i = 0; i < typeContexts.size(); i++) {
        if (!isAssignable(file, parameterTypes[i], typeForTypeContext(typeContexts[i]))) {
            return false;
        }
    }
    return true;
}

MaPLParser::ApiFunctionContext *findFunction(MaPLFile *file,
                                             std::string type,
                                             std::string name,
                                             std::vector<MaPLType> parameterTypes) {
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
                if (function && functionIsCompatible(file, function, name, parameterTypes)) {
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
                    if (functionIsCompatible(file, function, name, parameterTypes)) {
                        return function;
                    }
                }
                MaPLParser::ApiInheritanceContext *inheritance = apiDeclaration->apiInheritance();
                if (inheritance) {
                    for (MaPLParser::IdentifierContext *identifier : inheritance->identifier()) {
                        MaPLParser::ApiFunctionContext *foundFunction = findFunction(file, identifier->getText(), name, parameterTypes);
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
        MaPLParser::ApiFunctionContext *foundFunction = findFunction(dependency, type, name, parameterTypes);
        if (foundFunction) {
            return foundFunction;
        }
    }
    
    return NULL;
}

MaPLParser::ApiSubscriptContext *findSubscript(MaPLFile *file,
                                               std::string type,
                                               MaPLType indexType) {
    MaPLParser::ApiDeclarationContext *typeDeclaration = findType(file, type, NULL);
    if (!typeDeclaration) {
        return NULL;
    }
    for (MaPLParser::ApiSubscriptContext *subscript : typeDeclaration->apiSubscript()) {
        if (isAssignable(file, indexType, typeForTypeContext(subscript->type(1)))) {
            return subscript;
        }
    }
    MaPLParser::ApiInheritanceContext *inheritance = typeDeclaration->apiInheritance();
    if (inheritance) {
        for (MaPLParser::IdentifierContext *identifier : inheritance->identifier()) {
            MaPLParser::ApiSubscriptContext *foundSubscript = findSubscript(file, identifier->getText(), indexType);
            if (foundSubscript) {
                return foundSubscript;
            }
        }
    }
    return NULL;
}

MaPLParser::ApiPropertyContext *findProperty(MaPLFile *file,
                                             std::string type,
                                             std::string name,
                                             MaPLParser::ApiPropertyContext *excludingType) {
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
                if (!property || (excludingType && excludingType == property)) { continue; }
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
                    if (excludingType && excludingType == property) {
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
                        MaPLParser::ApiPropertyContext *foundProperty = findProperty(file, identifier->getText(), name, excludingType);
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
        MaPLParser::ApiPropertyContext *foundProperty = findProperty(dependency, type, name, excludingType);
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

// TODO: Convert struct params throughout this program, where applicable, to use 'const' and '&' like is done for this function.
void logError(MaPLFile *file, antlr4::Token *token, const std::string &msg) {
    if (token) {
        printf("%s %ld:%ld: %s\n", file->getNormalizedFilePath().c_str(), token->getLine(), token->getCharPositionInLine(), msg.c_str());
    } else {
        printf("%s: %s\n", file->getNormalizedFilePath().c_str(), msg.c_str());
    }
}

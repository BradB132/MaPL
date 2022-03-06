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

bool isCompatibleType(MaPLParser::TypeContext *typeContext, MaPLType type) {
    if (typeContext->identifier()) {
        return type.type == MaPLPrimitiveType_Pointer && typeContext->identifier()->getText() == type.pointerType;
    }
    switch (typeContext->start->getType()) {
        case MaPLParser::DECL_INT8:
            return type.type == MaPLPrimitiveType_Int8 ||
                   type.type == MaPLPrimitiveType_SignedInt_AmbiguousSize ||
                   type.type == MaPLPrimitiveType_Int_AmbiguousSizeAndSign;
        case MaPLParser::DECL_INT16:
            return type.type == MaPLPrimitiveType_Int16 ||
                   type.type == MaPLPrimitiveType_SignedInt_AmbiguousSize ||
                   type.type == MaPLPrimitiveType_Int_AmbiguousSizeAndSign;
        case MaPLParser::DECL_INT32:
            return type.type == MaPLPrimitiveType_Int32 ||
                   type.type == MaPLPrimitiveType_SignedInt_AmbiguousSize ||
                   type.type == MaPLPrimitiveType_Int_AmbiguousSizeAndSign;
        case MaPLParser::DECL_INT64:
            return type.type == MaPLPrimitiveType_Int64 ||
                   type.type == MaPLPrimitiveType_SignedInt_AmbiguousSize ||
                   type.type == MaPLPrimitiveType_Int_AmbiguousSizeAndSign;
        case MaPLParser::DECL_UINT8:
            return type.type == MaPLPrimitiveType_UInt8 ||
                   type.type == MaPLPrimitiveType_Int_AmbiguousSizeAndSign;
        case MaPLParser::DECL_UINT16:
            return type.type == MaPLPrimitiveType_UInt16 ||
                   type.type == MaPLPrimitiveType_Int_AmbiguousSizeAndSign;
        case MaPLParser::DECL_UINT32:
            return type.type == MaPLPrimitiveType_UInt32 ||
                   type.type == MaPLPrimitiveType_Int_AmbiguousSizeAndSign;
        case MaPLParser::DECL_UINT64:
            return type.type == MaPLPrimitiveType_UInt64 ||
                   type.type == MaPLPrimitiveType_Int_AmbiguousSizeAndSign;
        case MaPLParser::DECL_FLOAT32:
            return type.type == MaPLPrimitiveType_Float32 ||
                   type.type == MaPLPrimitiveType_Float_AmbiguousSize;
        case MaPLParser::DECL_FLOAT64:
            return type.type == MaPLPrimitiveType_Float64 ||
                   type.type == MaPLPrimitiveType_Float_AmbiguousSize;
        case MaPLParser::DECL_BOOL: return type.type == MaPLPrimitiveType_Boolean;
        case MaPLParser::DECL_STRING: return type.type == MaPLPrimitiveType_String;
        default: return false;
    }
}

std::set<std::string> superclasses(MaPLFile *file, std::string type) {
    std::set<std::string> allClasses;
    MaPLParser::ApiDeclarationContext *typeContext = findType(file, type);
    if (!typeContext) {
        return allClasses;
    }
    MaPLParser::ApiInheritanceContext *inheritance = typeContext->apiInheritance();
    if (!inheritance) {
        return allClasses;
    }
    for (MaPLParser::IdentifierContext *identifier : inheritance->identifier()) {
        std::string parentType = identifier->getText();
        allClasses.insert(parentType);
        std::set<std::string> parentClasses = superclasses(file, parentType);
        allClasses.insert(parentClasses.begin(), parentClasses.end());
    }
    return allClasses;
}

std::vector<std::string> mutualSuperclasses(MaPLFile *file, std::string type1, std::string type2) {
    std::set<std::string> superclasses1 = superclasses(file, type1);
    std::set<std::string> superclasses2 = superclasses(file, type2);
    std::vector<std::string> mutuals;
    for (std::string superclass : superclasses1) {
        if (superclasses2.count(superclass)) {
            mutuals.push_back(superclass);
        }
    }
    return mutuals;
}

MaPLParser::ApiDeclarationContext *findType(MaPLFile *file, std::string type) {
    MaPLParser::ProgramContext *program = file->getParseTree();
    if (!program) { return NULL; }
    for (MaPLParser::StatementContext *statement : program->statement()) {
        MaPLParser::ApiDeclarationContext *apiDeclaration = statement->apiDeclaration();
        if (apiDeclaration && apiDeclaration->keyToken->getType() == MaPLParser::API_TYPE) {
            MaPLParser::IdentifierContext *identifier = apiDeclaration->identifier();
            if (identifier && identifier->getText() == type) {
                return apiDeclaration;
            }
        }
    }
    for (MaPLFile *dependency : file->getDependencies()) {
        MaPLParser::ApiDeclarationContext *apiDeclaration = findType(dependency, type);
        if (apiDeclaration) {
            return apiDeclaration;
        }
    }
    return NULL;
}

bool functionIsCompatible(MaPLParser::ApiFunctionContext *function,
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
        if (!isCompatibleType(typeContexts[i], parameterTypes[i])) {
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
                if (function && functionIsCompatible(function, name, parameterTypes)) {
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
                    if (functionIsCompatible(function, name, parameterTypes)) {
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
    MaPLParser::ApiDeclarationContext *typeDeclaration = findType(file, type);
    if (!typeDeclaration) {
        return NULL;
    }
    for (MaPLParser::ApiSubscriptContext *subscript : typeDeclaration->apiSubscript()) {
        if (isCompatibleType(subscript->type(1), indexType)) {
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
                                             std::string name) {
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
                if (!property) { continue; }
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
                    MaPLParser::IdentifierContext *identifier = property->identifier();
                    if (identifier && identifier->getText() == name) {
                        return property;
                    }
                }
                MaPLParser::ApiInheritanceContext *inheritance = apiDeclaration->apiInheritance();
                if (inheritance) {
                    for (MaPLParser::IdentifierContext *identifier : inheritance->identifier()) {
                        MaPLParser::ApiPropertyContext *foundProperty = findProperty(file, identifier->getText(), name);
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
        MaPLParser::ApiPropertyContext *foundProperty = findProperty(dependency, type, name);
        if (foundProperty) {
            return foundProperty;
        }
    }
    
    return NULL;
}

void logError(MaPLFile *file, antlr4::Token *token, const std::string &msg) {
    if (token) {
        printf("%s %ld:%ld: %s\n", file->getNormalizedFilePath().c_str(), token->getLine(), token->getCharPositionInLine(), msg.c_str());
    } else {
        printf("%s: %s\n", file->getNormalizedFilePath().c_str(), msg.c_str());
    }
}

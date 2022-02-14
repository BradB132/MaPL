//
//  MaPLCompilerHelpers.cpp
//  MaPLCompiler
//
//  Created by Brad Bambara on 2/14/22.
//

#include "MaPLCompilerHelpers.h"
#include "MaPLFile.h"

bool isAmbiguousNumericType(MaPLPrimitiveType returnType) {
    switch (returnType) {
        case MaPLPrimitiveType_Float_AmbiguousSize:
        case MaPLPrimitiveType_SignedInt_AmbiguousSize:
        case MaPLPrimitiveType_Int_AmbiguousSizeAndSign:
            return true;
        default:
            return false;
    }
}

bool isFloat(MaPLPrimitiveType returnType) {
    switch (returnType) {
        case MaPLPrimitiveType_Float32:
        case MaPLPrimitiveType_Float64:
            return true;
        default:
            return false;
    }
}

bool isSignedInt(MaPLPrimitiveType returnType) {
    switch (returnType) {
        case MaPLPrimitiveType_Int8:
        case MaPLPrimitiveType_Int16:
        case MaPLPrimitiveType_Int32:
        case MaPLPrimitiveType_Int64:
            return true;
        default:
            return false;
    }
}

bool isUnsignedInt(MaPLPrimitiveType returnType) {
    switch (returnType) {
        case MaPLPrimitiveType_UInt8:
        case MaPLPrimitiveType_UInt16:
        case MaPLPrimitiveType_UInt32:
        case MaPLPrimitiveType_UInt64:
            return true;
        default:
            return false;
    }
}

bool isNumeric(MaPLPrimitiveType returnType) {
    return isFloat(returnType) ||
           isSignedInt(returnType) ||
           isUnsignedInt(returnType) ||
           isAmbiguousNumericType(returnType);
}

bool typeExists(MaPLFile *file, std::string type) {
    MaPLParser::ProgramContext *program = file->getParseTree();
    if (!program) { return false; }
    for (MaPLParser::StatementContext *statement : program->statement()) {
        MaPLParser::ApiDeclarationContext *apiDeclaration = statement->apiDeclaration();
        if (!apiDeclaration && apiDeclaration->keyToken->getType() == MaPLParser::API_TYPE) {
            MaPLParser::IdentifierContext *identifier = apiDeclaration->identifier();
            if (identifier && identifier->getText() == type) {
                return true;
            }
        }
    }
    for (MaPLFile *dependency : file->getDependencies()) {
        if (typeExists(dependency, type)) {
            return true;
        }
    }
    return false;
}

MaPLParser::ApiFunctionContext *findFunction(MaPLFile *file,
                                             std::string type,
                                             std::string name,
                                             std::vector<MaPLType> parameterTypes) {
    return NULL;// TODO: Implement this.
}

MaPLParser::ApiSubscriptContext *findSubscript(MaPLFile *file,
                                               std::string type,
                                               MaPLType indexType) {
    return NULL;// TODO: Implement this.
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

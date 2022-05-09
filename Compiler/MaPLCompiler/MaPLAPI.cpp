//
//  MaPLAPI.cpp
//  MaPLCompiler
//
//  Created by Brad Bambara on 5/3/22.
//

#include "MaPLAPI.h"
#include "MaPLFile.h"

MaPLGenericType genericTypeForTypeContext(MaPLParser::TypeContext *typeContext, const std::vector<std::string> &generics, MaPLFile *file);

MaPLType MaPLGenericType::typeWithSubstitutedGenerics(const std::vector<MaPLType> &genericsSubstitutions) const {
    if (primitiveType == MaPLPrimitiveType_Uninitialized) {
        return genericsSubstitutions[genericIndex];
    }
    MaPLType concreteType{ primitiveType, pointerType };
    for (const MaPLGenericType &genericType : generics) {
        concreteType.generics.push_back(genericType.typeWithSubstitutedGenerics(genericsSubstitutions));
    }
    return concreteType;
}

MaPLGenericType MaPLGenericType::genericWithSubstitutedGenerics(const std::vector<MaPLGenericType> &genericsSubstitutions) const {
    if (primitiveType == MaPLPrimitiveType_Uninitialized) {
        return genericsSubstitutions[genericIndex];
    }
    MaPLGenericType returnType{ file, token, primitiveType, pointerType };
    for (const MaPLGenericType &genericType : generics) {
        returnType.generics.push_back(genericType.genericWithSubstitutedGenerics(genericsSubstitutions));
    }
    return returnType;
}

MaPLType MaPLGenericType::typeWithoutSubstitutedGenerics() const {
    MaPLType concreteType{
        (primitiveType == MaPLPrimitiveType_Uninitialized) ? MaPLPrimitiveType_Pointer : primitiveType,
        pointerType,
    };
    for (const MaPLGenericType &genericType : generics) {
        concreteType.generics.push_back(genericType.typeWithoutSubstitutedGenerics());
    }
    return concreteType;
}

bool MaPLGenericType::operator== (const MaPLGenericType &otherType) const {
    if (primitiveType != otherType.primitiveType) {
        return false;
    }
    if (primitiveType == MaPLPrimitiveType_Uninitialized) {
        return genericIndex == otherType.genericIndex;
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

bool MaPLGenericType::operator!= (const MaPLGenericType &otherType) const {
    return !(*this == otherType);
}

void performErrorCheckingForGeneric(const MaPLGenericType &genericType, const MaPLAPI *api) {
    if (genericType.primitiveType == MaPLPrimitiveType_Pointer) {
        const MaPLTypeAPI *typeAPI = api->findType(genericType.pointerType);
        if (typeAPI) {
            // Check that the supertype's generics count matches the number of generics on the referenced type.
            if (genericType.generics.size() != typeAPI->generics.size()) {
                genericType.file->logError(genericType.token, "The number of generics specified for '"+genericType.pointerType+"' doesn't match the number of generics in the type declaration.");
            }
        } else {
            genericType.file->logMissingTypeError(genericType.token, genericType.pointerType);
        }
        for (const MaPLGenericType &childGenericType : genericType.generics) {
            performErrorCheckingForGeneric(childGenericType, api);
        }
    }
}

MaPLGenericType genericTypeForPointerType(MaPLParser::PointerTypeContext *pointerTypeContext, const std::vector<std::string> &generics, MaPLFile *file) {
    MaPLGenericType genericType{ file, pointerTypeContext->start, MaPLPrimitiveType_Pointer, pointerTypeContext->identifier()->getText() };
    for (size_t i = 0; i < generics.size(); i++) {
        if (genericType.pointerType == generics[i]) {
            genericType.genericIndex = i;
            genericType.primitiveType = MaPLPrimitiveType_Uninitialized;
            if (pointerTypeContext->type().size() != 0) {
                file->logError(genericType.token, "Generic specifiers like '"+genericType.pointerType+"' cannot have their own generics.");
            }
            return genericType;
        }
    }
    std::vector<MaPLParser::TypeContext *> types = pointerTypeContext->type();
    for (MaPLParser::TypeContext *typeContext : types) {
        genericType.generics.push_back(genericTypeForTypeContext(typeContext, generics, file));
    }
    return genericType;
}

MaPLGenericType genericTypeForTypeContext(MaPLParser::TypeContext *typeContext, const std::vector<std::string> &generics, MaPLFile *file) {
    MaPLPrimitiveType primitive = primitiveTypeForTypeContext(typeContext);
    if (primitive == MaPLPrimitiveType_Pointer) {
        return genericTypeForPointerType(typeContext->pointerType(), generics, file);
    }
     return { file, typeContext->start, primitive };
}

MaPLFunctionAPI functionAPIForNode(MaPLParser::ApiFunctionContext *node, const std::vector<std::string> &generics, MaPLFile *file) {
    MaPLParser::ApiTypeContext *apiTypeContext = dynamic_cast<MaPLParser::ApiTypeContext *>(node->parent);
    MaPLFunctionAPI functionAPI{
        file,
        node,
        node->identifier()->getText(),
        apiTypeContext ? apiTypeContext->typeName->getText() : "",
    };
    if (node->API_VOID()) {
        functionAPI.returnType = { file, node->API_VOID()->getSymbol(), MaPLPrimitiveType_Void };
    } else {
        functionAPI.returnType = genericTypeForTypeContext(node->type(), generics, file);
    }
    MaPLParser::ApiFunctionParamsContext *params = node->apiFunctionParams();
    if (params) {
        functionAPI.isVariadic = params->API_VARIADIC_PARAMETERS() != NULL;
        for (MaPLParser::TypeContext *type : params->type()) {
            functionAPI.parameterTypes.push_back(genericTypeForTypeContext(type, generics, file));
        }
    }
    return functionAPI;
}

MaPLPropertyAPI propertyAPIForNode(MaPLParser::ApiPropertyContext *node, const std::vector<std::string> &generics, MaPLFile *file) {
    MaPLParser::ApiTypeContext *apiTypeContext = dynamic_cast<MaPLParser::ApiTypeContext *>(node->parent);
    return {
        file,
        node,
        node->identifier()->getText(),
        apiTypeContext ? apiTypeContext->typeName->getText() : "",
        genericTypeForTypeContext(node->type(), generics, file),
        node->API_READONLY() != NULL,
    };
}

MaPLSubscriptAPI subscriptAPIForNode(MaPLParser::ApiSubscriptContext *node, const std::vector<std::string> &generics, MaPLFile *file) {
    MaPLParser::ApiTypeContext *apiTypeContext = dynamic_cast<MaPLParser::ApiTypeContext *>(node->parent);
    return {
        file,
        node,
        apiTypeContext ? apiTypeContext->typeName->getText() : "",
        genericTypeForTypeContext(node->type(0), generics, file),
        genericTypeForTypeContext(node->type(1), generics, file),
        node->API_READONLY() != NULL,
    };
}

MaPLTypeAPI typeAPIForNode(MaPLParser::ApiTypeContext *node, MaPLFile *file) {
    MaPLTypeAPI typeAPI{ file, node, node->typeName->getText() };
    if (typeNameMatchesPrimitiveType(typeAPI.name)) {
        file->logError(node->typeName->start, "Type name '"+typeAPI.name+"' conflicts with a primitive type.");
    }
    for (MaPLParser::IdentifierContext *identifier : node->generics) {
        typeAPI.generics.push_back(identifier->getText());
    }
    for (size_t i = 0; i < typeAPI.generics.size(); i++) {
        if (typeNameMatchesPrimitiveType(typeAPI.generics[i])) {
            file->logError(node->generics[i]->start, "Generics descriptor '"+typeAPI.generics[i]+"' conflicts with a primitive type.");
        }
        // Enforce uniqueness for generics descriptors.
        for (size_t j = i+1; j < typeAPI.generics.size(); j++) {
            if (typeAPI.generics[i] == typeAPI.generics[j]) {
                file->logError(node->generics[i]->start, "Duplicate generics descriptor '"+typeAPI.generics[i]+"'. Descriptors must be unique.");
            }
        }
    }
    MaPLParser::ApiInheritanceContext *inheritance = node->apiInheritance();
    if (inheritance) {
        for (MaPLParser::PointerTypeContext *pointerType : inheritance->pointerType()) {
            typeAPI.supertypes.push_back(genericTypeForPointerType(pointerType, typeAPI.generics, file));
        }
    }
    for (MaPLParser::ApiFunctionContext *function : node->apiFunction()) {
        typeAPI.assimilate(functionAPIForNode(function, typeAPI.generics, file));
    }
    for (MaPLParser::ApiPropertyContext *property : node->apiProperty()) {
        typeAPI.assimilate(propertyAPIForNode(property, typeAPI.generics, file));
    }
    for (MaPLParser::ApiSubscriptContext *subscript : node->apiSubscript()) {
        typeAPI.assimilate(subscriptAPIForNode(subscript, typeAPI.generics, file));
    }
    return typeAPI;
}

std::string MaPLFunctionAPI::symbolDescriptor() const {
    std::string descriptor = typeAPIName.empty() ? "GLOBAL" : typeAPIName;
    descriptor += "_";
    descriptor += name;
    for (const MaPLGenericType &parameterType : parameterTypes) {
        descriptor += "_";
        if (parameterType.primitiveType == MaPLPrimitiveType_Pointer ||
            parameterType.primitiveType == MaPLPrimitiveType_Uninitialized) {
            descriptor += parameterType.pointerType;
        } else {
            descriptor += descriptorForPrimitive(parameterType.primitiveType);
        }
    }
    if (isVariadic) {
        descriptor += "_VARIADIC";
    }
    return descriptor;
}

std::string MaPLFunctionAPI::signatureDescriptor() const {
    std::vector<MaPLType> params;
    for (const MaPLGenericType &genericParam : parameterTypes) {
        params.push_back(genericParam.typeWithoutSubstitutedGenerics());
    }
    return descriptorForFunction(typeAPIName,
                                 name,
                                 params,
                                 isVariadic);
}

std::string MaPLPropertyAPI::symbolDescriptor() const {
    std::string descriptor = typeAPIName.empty() ? "GLOBAL" : typeAPIName;
    descriptor += "_";
    descriptor += name;
    return descriptor;
}

void MaPLTypeAPI::assimilate(const MaPLFunctionAPI &function) {
    functions.push_back(function);
}

void logPropertyCollision(const MaPLPropertyAPI &newProperty, const MaPLPropertyAPI &existingProperty) {
    newProperty.file->logError(newProperty.node->start, "Property '"+newProperty.name+"' in type '"+newProperty.typeAPIName+"' is in conflict with a previously declared property of the same name in type '"+existingProperty.typeAPIName+"'.");
    existingProperty.file->logError(existingProperty.node->start, "Property '"+existingProperty.name+"' in type '"+existingProperty.typeAPIName+"' later comes into conflict with a property of the same name in type '"+newProperty.typeAPIName+"'.");
}

void MaPLTypeAPI::assimilate(const MaPLPropertyAPI &property) {
    if (properties.count(property.name)) {
        logPropertyCollision(property, properties[property.name]);
    }
    properties[property.name] = property;
}

void MaPLTypeAPI::assimilate(const MaPLSubscriptAPI &subscript) {
    subscripts.push_back(subscript);
}

void MaPLAPI::assimilate(const MaPLFunctionAPI &globalFunction) {
    globalFunctions.push_back(globalFunction);
}

void MaPLAPI::assimilate(const MaPLPropertyAPI &globalProperty) {
    if (globalProperties.count(globalProperty.name)) {
        globalProperty.file->logError(globalProperty.node->start, "Global property '"+globalProperty.name+"' is in conflict with a previously declared global property of the same name.");
        MaPLPropertyAPI &conflictingProperty = globalProperties[globalProperty.name];
        conflictingProperty.file->logError(conflictingProperty.node->start, "Global property '"+conflictingProperty.name+"' later comes into conflict with a global property of the same name.");
    }
    globalProperties[globalProperty.name] = globalProperty;
}

void MaPLAPI::assimilate(const MaPLTypeAPI &type) {
    if (types.count(type.name)) {
        type.file->logError(type.node->typeName->start, "Type '"+type.name+"' is in conflict with a previously declared type of the same name.");
        MaPLTypeAPI &conflictingType = types[type.name];
        conflictingType.file->logError(conflictingType.node->typeName->start, "Type '"+type.name+"' later comes into conflict with a type of the same name.");
    }
    types[type.name] = type;
}

void MaPLAPI::assimilate(const MaPLAPI *otherAPI) {
    for (const MaPLFunctionAPI &functionAPI : otherAPI->globalFunctions) {
        assimilate(functionAPI);
    }
    for (const auto&[key, property] : otherAPI->globalProperties) {
        assimilate(property);
    }
    for (const auto&[key, type] : otherAPI->types) {
        assimilate(type);
    }
}

void MaPLAPI::assimilate(MaPLParser::ProgramContext *program, MaPLFile *file) {
    for(MaPLParser::StatementContext *statement : program->statement()) {
        MaPLParser::ApiTypeContext *typeContext = statement->apiType();
        if (typeContext) {
            assimilate(typeAPIForNode(typeContext, file));
        }
        MaPLParser::ApiGlobalContext *globalContext = statement->apiGlobal();
        if (globalContext) {
            MaPLParser::ApiPropertyContext *propertyContext = globalContext->apiProperty();
            if (propertyContext) {
                assimilate(propertyAPIForNode(propertyContext, {}, file));
            }
            MaPLParser::ApiFunctionContext *functionContext = globalContext->apiFunction();
            if (functionContext) {
                assimilate(functionAPIForNode(functionContext, {}, file));
            }
        }
    }
}

void checkForFunctionCollisionsInList(const MaPLFunctionAPI *originalFunction, const std::vector<MaPLFunctionAPI> &functionList, const std::vector<MaPLGenericType> &substitutedGenerics) {
    for (const MaPLFunctionAPI &functionAPI : functionList) {
        if (&functionAPI == originalFunction ||
            functionAPI.isVariadic != originalFunction->isVariadic ||
            functionAPI.name != originalFunction->name ||
            functionAPI.parameterTypes.size() != originalFunction->parameterTypes.size()) {
            continue;
        }
        bool parametersAreEquivalent = true;
        for (size_t i = 0; i < functionAPI.parameterTypes.size(); i++) {
            if (functionAPI.parameterTypes[i].genericWithSubstitutedGenerics(substitutedGenerics) != originalFunction->parameterTypes[i]) {
                parametersAreEquivalent = false;
                break;
            }
        }
        if (parametersAreEquivalent) {
            originalFunction->file->logError(originalFunction->node->start, "Function '"+originalFunction->signatureDescriptor()+"' is in conflict with previously declared function '"+functionAPI.signatureDescriptor()+"'.");
            functionAPI.file->logError(functionAPI.node->start, "Function '"+functionAPI.signatureDescriptor()+"' later comes into conflict with function '"+originalFunction->signatureDescriptor()+"'.");
        }
    }
}

void checkForFunctionCollisions(const MaPLAPI *api, const MaPLFunctionAPI *originalFunction, const std::string &typeName, const std::vector<MaPLGenericType> &substitutedGenerics) {
    const MaPLTypeAPI *foundType = api->findType(typeName);
    if (!foundType) {
        return;
    }
    checkForFunctionCollisionsInList(originalFunction, foundType->functions, substitutedGenerics);
    for (const MaPLGenericType &supertype : foundType->supertypes) {
        std::vector<MaPLGenericType> remappedGenerics;
        for (const MaPLGenericType &supertypeGeneric : supertype.generics) {
            remappedGenerics.push_back(supertypeGeneric.genericWithSubstitutedGenerics(substitutedGenerics));
        }
        checkForFunctionCollisions(api, originalFunction, supertype.pointerType, remappedGenerics);
    }
}

void checkForSubscriptCollisions(const MaPLAPI *api, const MaPLSubscriptAPI *originalSubscript, const std::string &typeName, const std::vector<MaPLGenericType> &substitutedGenerics) {
    const MaPLTypeAPI *foundType = api->findType(typeName);
    if (!foundType) {
        return;
    }
    for (const MaPLSubscriptAPI &subscriptAPI : foundType->subscripts) {
        if (&subscriptAPI == originalSubscript) {
            continue;
        }
        if (subscriptAPI.indexType.genericWithSubstitutedGenerics(substitutedGenerics) == originalSubscript->indexType) {
            std::string originalDescriptor = descriptorForType(originalSubscript->indexType.typeWithoutSubstitutedGenerics());
            std::string subscriptAPIDescriptor = descriptorForType(subscriptAPI.indexType.typeWithoutSubstitutedGenerics());
            originalSubscript->file->logError(originalSubscript->node->start, "Subscript with index type '"+originalDescriptor+"' on type '"+originalSubscript->typeAPIName+"' is in conflict with subscript with index type '"+subscriptAPIDescriptor+"' on type '"+subscriptAPI.typeAPIName+"'.");
        }
    }
    for (const MaPLGenericType &supertype : foundType->supertypes) {
        std::vector<MaPLGenericType> remappedGenerics;
        for (const MaPLGenericType &supertypeGeneric : supertype.generics) {
            remappedGenerics.push_back(supertypeGeneric.genericWithSubstitutedGenerics(substitutedGenerics));
        }
        checkForSubscriptCollisions(api, originalSubscript, supertype.pointerType, remappedGenerics);
    }
}

void MaPLAPI::performErrorChecking() {
    for (const auto&[key, type] : types) {
        for (const std::string &genericDescriptor : type.generics) {
            // Log an error if a generic descriptor conflicts with a type name.
            if (types.count(genericDescriptor)) {
                type.file->logError(type.node->start, "Generic descriptor '"+genericDescriptor+"' conflicts with a type of the same name.");
                MaPLTypeAPI &conflictingType = types[genericDescriptor];
                conflictingType.file->logError(conflictingType.node->typeName->start, "Type '"+genericDescriptor+"' later comes into cconflict with a generic descriptor of the same name.");
            }
        }
        for (const MaPLGenericType& supertype : type.supertypes) {
            performErrorCheckingForGeneric(supertype, this);
        }
        std::vector<MaPLGenericType> mappedGenerics;
        for (size_t i = 0; i < type.generics.size(); i++) {
            mappedGenerics.push_back({ type.file, type.node->start, MaPLPrimitiveType_Uninitialized, type.generics[i], i });
        }
        for (const MaPLFunctionAPI &functionAPI : type.functions) {
            performErrorCheckingForGeneric(functionAPI.returnType, this);
            for (const MaPLGenericType &genericType : functionAPI.parameterTypes) {
                performErrorCheckingForGeneric(genericType, this);
            }
            checkForFunctionCollisions(this, &functionAPI, functionAPI.typeAPIName, mappedGenerics);
        }
        for (const auto&[key, propertyAPI] : type.properties) {
            performErrorCheckingForGeneric(propertyAPI.returnType, this);
            const MaPLPropertyAPI *conflictingProperty = findTypeProperty(propertyAPI.typeAPIName, propertyAPI.name, &propertyAPI);
            if (conflictingProperty) {
                logPropertyCollision(propertyAPI, *conflictingProperty);
            }
        }
        for (const MaPLSubscriptAPI &subscriptAPI : type.subscripts) {
            performErrorCheckingForGeneric(subscriptAPI.returnType, this);
            performErrorCheckingForGeneric(subscriptAPI.indexType, this);
            checkForSubscriptCollisions(this, &subscriptAPI, subscriptAPI.typeAPIName, mappedGenerics);
        }
    }
    std::vector<MaPLGenericType> emptyGenericsList;
    for (const MaPLFunctionAPI &functionAPI : globalFunctions) {
        performErrorCheckingForGeneric(functionAPI.returnType, this);
        for (const MaPLGenericType &genericType : functionAPI.parameterTypes) {
            performErrorCheckingForGeneric(genericType, this);
        }
        checkForFunctionCollisionsInList(&functionAPI, globalFunctions, emptyGenericsList);
    }
    for (const auto&[key, propertyAPI] : globalProperties) {
        performErrorCheckingForGeneric(propertyAPI.returnType, this);
    }
}

const MaPLTypeAPI *MaPLAPI::findType(const std::string &typeName) const {
    if (!types.count(typeName)) {
        return NULL;
    }
    return &(types.at(typeName));
}

const MaPLFunctionAPI *findFunctionInList(const MaPLAPI *api,
                                          const std::vector<MaPLFunctionAPI> &functionList,
                                          const std::string &functionName,
                                          const std::vector<MaPLType> &expressionParameterTypes,
                                          const std::vector<MaPLType> &substitutedGenerics,
                                          const MaPLFunctionAPI *excludingFunction) {
    for (const MaPLFunctionAPI &functionAPI : functionList) {
        // Confirm matching function name.
        if (excludingFunction == &functionAPI || functionName != functionAPI.name) {
            continue;
        }
        
        // It's possible to match longer parameter lists, but only if this function has variadic params.
        if ((!functionAPI.isVariadic && functionAPI.parameterTypes.size() != expressionParameterTypes.size()) ||
            (functionAPI.isVariadic && functionAPI.parameterTypes.size() > expressionParameterTypes.size())) {
            continue;
        }
        
        // Confirm assignable types for all params.
        bool parametersAreCompatible = true;
        for (size_t i = 0; i < functionAPI.parameterTypes.size(); i++) {
            if (!api->isAssignable(expressionParameterTypes[i], functionAPI.parameterTypes[i].typeWithSubstitutedGenerics(substitutedGenerics))) {
                parametersAreCompatible = false;
                break;
            }
        }
        if (parametersAreCompatible) {
            return &functionAPI;
        }
    }
    return NULL;
}

const MaPLFunctionAPI *MaPLAPI::findTypeFunction(const std::string &typeName,
                                                 const std::string &functionName,
                                                 const std::vector<MaPLType> &parameterTypes,
                                                 const std::vector<MaPLType> &substitutedGenerics,
                                                 const MaPLFunctionAPI *excludingFunction) const {
    const MaPLTypeAPI *foundType = findType(typeName);
    if (foundType) {
        const MaPLFunctionAPI *foundFunction = findFunctionInList(this,
                                                                  foundType->functions,
                                                                  functionName,
                                                                  parameterTypes,
                                                                  substitutedGenerics,
                                                                  excludingFunction);
        if (foundFunction) {
            return foundFunction;
        }
        for (const MaPLGenericType &genericType : foundType->supertypes) {
            std::vector<MaPLType> remappedGenerics;
            for (const MaPLGenericType &supertypeGeneric : genericType.generics) {
                remappedGenerics.push_back(supertypeGeneric.typeWithSubstitutedGenerics(substitutedGenerics));
            }
            foundFunction = findTypeFunction(genericType.pointerType,
                                             functionName,
                                             parameterTypes,
                                             remappedGenerics,
                                             excludingFunction);
            if (foundFunction) {
                return foundFunction;
            }
        }
    }
    return NULL;
}

const MaPLFunctionAPI *MaPLAPI::findGlobalFunction(const std::string &functionName,
                                                   const std::vector<MaPLType> &parameterTypes,
                                                   const MaPLFunctionAPI *excludingFunction) const {
    return findFunctionInList(this, globalFunctions, functionName, parameterTypes, {}, excludingFunction);
}

const MaPLPropertyAPI *MaPLAPI::findTypeProperty(const std::string &typeName,
                                                 const std::string &propertyName,
                                                 const MaPLPropertyAPI *excludingProperty) const {
    const MaPLTypeAPI *foundType = findType(typeName);
    if (foundType) {
        if (foundType->properties.count(propertyName)) {
            const MaPLPropertyAPI *foundProperty = &(foundType->properties.at(propertyName));
            if (foundProperty != excludingProperty) {
                return foundProperty;
            }
        }
        for (const MaPLGenericType &genericType : foundType->supertypes) {
            const MaPLPropertyAPI *foundProperty = findTypeProperty(genericType.pointerType,
                                                                    propertyName,
                                                                    excludingProperty);
            if (foundProperty) {
                return foundProperty;
            }
        }
    }
    return NULL;
}

const MaPLPropertyAPI *MaPLAPI::findGlobalProperty(const std::string &propertyName) const {
    if (!globalProperties.count(propertyName)) {
        return NULL;
    }
    return &(globalProperties.at(propertyName));
}

const MaPLSubscriptAPI *MaPLAPI::findSubscript(const std::string &typeName,
                                               const MaPLType &indexType,
                                               const std::vector<MaPLType> &substitutedGenerics,
                                               const MaPLSubscriptAPI *excludingSubscript) const {
    const MaPLTypeAPI *foundType = findType(typeName);
    if (foundType) {
        for (const MaPLSubscriptAPI &subscript : foundType->subscripts) {
            if (excludingSubscript == &subscript) {
                continue;
            }
            MaPLType subscriptIndexType = subscript.indexType.typeWithSubstitutedGenerics(substitutedGenerics);
            if (isAssignable(indexType, subscriptIndexType)) {
                return &subscript;
            }
        }
        for (const MaPLGenericType &genericType : foundType->supertypes) {
            std::vector<MaPLType> remappedGenerics;
            for (const MaPLGenericType &supertypeGeneric : genericType.generics) {
                remappedGenerics.push_back(supertypeGeneric.typeWithSubstitutedGenerics(substitutedGenerics));
            }
            const MaPLSubscriptAPI *foundSubscript = findSubscript(genericType.pointerType,
                                                                   indexType,
                                                                   remappedGenerics,
                                                                   excludingSubscript);
            if (foundSubscript) {
                return foundSubscript;
            }
        }
    }
    return NULL;
}

bool MaPLAPI::isAssignable(const MaPLType &expressionType, const MaPLType &assignToType) const {
    // Handle direct matches first.
    if (assignToType.primitiveType == expressionType.primitiveType) {
        if (assignToType.primitiveType == MaPLPrimitiveType_Pointer) {
            if (expressionType.pointerType.empty()) {
                // Empty "pointerType" indicates a NULL literal. Nulls are assignable to any type of pointer.
                return true;
            }
            if (expressionType.pointerType == assignToType.pointerType) {
                // Pointer types match, the generics can do a direct comparison.
                if (expressionType.generics.size() != assignToType.generics.size()) {
                    return false;
                }
                for (size_t i = 0; i < expressionType.generics.size(); i++) {
                    if (!isAssignable(expressionType.generics[i], assignToType.generics[i])) {
                        return false;
                    }
                }
                return true;
            }
            // Attempt to interpret 'assignToType' as a parent type and calculate what the equivalent generics would be.
            MaPLType equivalentType = findEquivalentGenerics(expressionType, assignToType.pointerType);
            if (equivalentType.primitiveType == MaPLPrimitiveType_TypeError) {
                return false;
            }
            if (equivalentType.generics.size() != assignToType.generics.size()) {
                return false;
            }
            for (size_t i = 0; i < equivalentType.generics.size(); i++) {
                if (!isAssignable(equivalentType.generics[i], assignToType.generics[i])) {
                    return false;
                }
            }
            return true;
        }
        return true;
    }
    // Handle all the ways that a concrete numeric type could accept ambiguity from the expression.
    switch (assignToType.primitiveType) {
        case MaPLPrimitiveType_Char: // Intentional fallthrough.
        case MaPLPrimitiveType_UInt32: // Intentional fallthrough.
        case MaPLPrimitiveType_UInt64:
            return expressionType.primitiveType == MaPLPrimitiveType_Int_AmbiguousSizeAndSign;
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

bool MaPLAPI::inheritsFromType(const std::string &typeName, const std::string &ancestorTypeName) const {
    if (typeName == ancestorTypeName) {
        return true;
    }
    const MaPLTypeAPI *foundType = findType(typeName);
    if (foundType) {
        for (const MaPLGenericType &genericType : foundType->supertypes) {
            if (inheritsFromType(genericType.pointerType, ancestorTypeName)) {
                return true;
            }
        }
    }
    return false;
}

std::vector<std::string> findInheritanceCyclesAndDiamonds(const MaPLAPI *api,
                                                          const MaPLTypeAPI *type,
                                                          std::set<std::string> &cycleTypes,
                                                          std::set<std::string> &diamondTypes) {
    std::vector<std::string> cycleVector;
    if (cycleTypes.count(type->name) > 0) {
        // This graph contains a cycle.
        cycleVector.push_back(type->name);
        return cycleVector;
    }
    if (diamondTypes.count(type->name) > 0) {
        // This graph contains a diamond.
        type->file->logError(type->node->start, "The type '"+type->name+"' is inherited more than once by the same type, forming an 'inheritance diamond'.");
        return cycleVector;
    }
    diamondTypes.insert(type->name);
    if (type->supertypes.size()) {
        cycleTypes.insert(type->name);
        for (const MaPLGenericType &supertype : type->supertypes) {
            const MaPLTypeAPI *parentType = api->findType(supertype.pointerType);
            if (!parentType) {
                continue;
            }
            std::vector<std::string> foundCycle = findInheritanceCyclesAndDiamonds(api, parentType, cycleTypes, diamondTypes);
            if (foundCycle.size() > 0) {
                cycleVector.push_back(type->name);
                cycleVector.insert(cycleVector.end(), foundCycle.begin(), foundCycle.end());
                break;
            }
        }
        cycleTypes.erase(type->name);
    }
    return cycleVector;
}

void MaPLAPI::findInheritanceCyclesAndDiamonds() {
    std::set<std::string> cycleTypes;
    std::set<std::string> diamondTypes;
    for (const auto&[key, type] : types) {
        std::vector<std::string> cycle = ::findInheritanceCyclesAndDiamonds(this, &type, cycleTypes, diamondTypes);
        if (cycle.size() > 0) {
            std::string cycleDescriptor;
            for (size_t i = 0; i < cycle.size(); i++) {
                cycleDescriptor += cycle[i];
                if (i < cycle.size()-1) {
                    cycleDescriptor += " -> ";
                }
            }
            cycleDescriptor += ".";
            type.file->logError(type.node->start, "Type inheritance forms a cycle: "+cycleDescriptor);
            return;
        }
        cycleTypes.clear();
        diamondTypes.clear();
    }
}

MaPLType MaPLAPI::findEquivalentGenerics(const MaPLType &fromType, const std::string &toAncestorName) const {
    if (fromType.primitiveType != MaPLPrimitiveType_Pointer ||
        toAncestorName.empty() ||
        fromType.pointerType == toAncestorName) {
        return fromType;
    }
    const MaPLTypeAPI *foundTypeAPI = findType(fromType.pointerType);
    if (foundTypeAPI) {
        for (const MaPLGenericType &genericSuperType : foundTypeAPI->supertypes) {
            MaPLType supertype = { genericSuperType.primitiveType, genericSuperType.pointerType };
            for (const MaPLGenericType &nestedGeneric : genericSuperType.generics) {
                supertype.generics.push_back(nestedGeneric.typeWithSubstitutedGenerics(fromType.generics));
            }
            MaPLType foundType = findEquivalentGenerics(supertype, toAncestorName);
            if (foundType.primitiveType != MaPLPrimitiveType_TypeError) {
                return foundType;
            }
        }
    }
    return { MaPLPrimitiveType_TypeError };
}

void MaPLAPI::collateSymbolsInAPI(std::map<std::string, MaPLSymbol> &symbolTable) {
    // Collate all symbol descriptors in this API. Assign a placeholder value of 0.
    for (const MaPLFunctionAPI &functionAPI : globalFunctions) {
        symbolTable[functionAPI.symbolDescriptor()] = 0;
    }
    for (const auto&[key, propertyAPI] : globalProperties) {
        symbolTable[propertyAPI.symbolDescriptor()] = 0;
    }
    for (const auto&[key, typeAPI] : types) {
        for (const MaPLFunctionAPI &functionAPI : typeAPI.functions) {
            symbolTable[functionAPI.symbolDescriptor()] = 0;
        }
        for (const auto&[key, propertyAPI] : typeAPI.properties) {
            symbolTable[propertyAPI.symbolDescriptor()] = 0;
        }
    }
}

//
//  EaSLHandler.h
//  MaPLGenerator
//
//  Created by Brad Bambara on 5/19/22.
//

#ifndef EaSLHandler_h
#define EaSLHandler_h

#include <filesystem>
#include <vector>

#include "EaSLParser.h"
#include "MaPLInterface.h"
#include "MaPLGeneratorCollections.h"

class SchemaEnum : public MaPLInterface {
public:
    SchemaEnum(EaSLParser::EnumDefinitionContext *enumContext);
    virtual MaPLParameter invokeFunction(MaPLSymbol functionSymbol, const MaPLParameter *argv, MaPLParameterCount argc);
    virtual MaPLParameter invokeSubscript(MaPLParameter index);
    
    std::string _name;
    MaPLArray<std::string> *_cases;
    MaPLArrayMap<std::string> *_annotations;
};

class SchemaAttribute : public MaPLInterface {
public:
    SchemaAttribute(EaSLParser::AttributeContext *attributeContext);
    virtual MaPLParameter invokeFunction(MaPLSymbol functionSymbol, const MaPLParameter *argv, MaPLParameterCount argc);
    virtual MaPLParameter invokeSubscript(MaPLParameter index);
    
    std::string _name;
    std::string _typeName;
    std::string _typeNamespace;
    bool _typeIsUIDReference;
    bool _isPrimitiveType;
    MaPLArrayMap<std::string> *_annotations;
    MaPLArray<std::string> *_defaultValues;
    uint32_t _minOccurrences;
    uint32_t _maxOccurrences;
};

class SchemaClass : public MaPLInterface {
public:
    SchemaClass(EaSLParser::ClassDefinitionContext *classContext);
    virtual MaPLParameter invokeFunction(MaPLSymbol functionSymbol, const MaPLParameter *argv, MaPLParameterCount argc);
    virtual MaPLParameter invokeSubscript(MaPLParameter index);
    
    bool hasUID();
    
    std::string _name;
    std::string _superclass;
    std::string _superclassNamespace;
    MaPLArrayMap<SchemaAttribute *> *_attributes;
    MaPLArrayMap<std::string> *_annotations;
};

class Schema : public MaPLInterface {
public:
    Schema(EaSLParser::SchemaContext *schemaContext);
    virtual MaPLParameter invokeFunction(MaPLSymbol functionSymbol, const MaPLParameter *argv, MaPLParameterCount argc);
    virtual MaPLParameter invokeSubscript(MaPLParameter index);

    std::string _namespace;
    MaPLArrayMap<SchemaEnum *> *_enums;
    MaPLArrayMap<SchemaClass *> *_classes;
};

MaPLArrayMap<Schema *> *schemasForPaths(const std::vector<std::filesystem::path> &schemaPaths);

#endif /* EaSLHandler_h */

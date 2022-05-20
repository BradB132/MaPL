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
    ~SchemaEnum();
    virtual MaPLParameter invokeFunction(MaPLSymbol functionSymbol, const MaPLParameter *argv, MaPLParameterCount argc);
    virtual MaPLParameter invokeSubscript(MaPLParameter index);
    
    std::string _name;
    MaPLArray<std::string> *_cases;
    MaPLArrayMap<std::string> *_annotations;
};

class SchemaAttribute : public MaPLInterface {
public:
    SchemaAttribute(EaSLParser::AttributeContext *attributeContext);
    ~SchemaAttribute();
    virtual MaPLParameter invokeFunction(MaPLSymbol functionSymbol, const MaPLParameter *argv, MaPLParameterCount argc);
    virtual MaPLParameter invokeSubscript(MaPLParameter index);
    
    std::string _name;
    std::string _typeName;
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
    ~SchemaClass();
    virtual MaPLParameter invokeFunction(MaPLSymbol functionSymbol, const MaPLParameter *argv, MaPLParameterCount argc);
    virtual MaPLParameter invokeSubscript(MaPLParameter index);
    
    std::string _name;
    std::string _superclass;
    MaPLArrayMap<SchemaAttribute *> *_attributes;
    MaPLArrayMap<std::string> *_annotations;
};

class Schema : public MaPLInterface {
public:
    Schema(EaSLParser::SchemaContext *schemaContext);
    ~Schema();
    virtual MaPLParameter invokeFunction(MaPLSymbol functionSymbol, const MaPLParameter *argv, MaPLParameterCount argc);
    virtual MaPLParameter invokeSubscript(MaPLParameter index);

    std::string _namespace;
    MaPLArrayMap<SchemaEnum *> *_enums;
    MaPLArrayMap<SchemaClass *> *_classes;
};

std::vector<EaSLParser::SchemaContext *> schemasForPaths(const std::vector<std::filesystem::path> &schemaPaths);

void validateSchemas(const std::vector<EaSLParser::SchemaContext *> &schemas);

#endif /* EaSLHandler_h */

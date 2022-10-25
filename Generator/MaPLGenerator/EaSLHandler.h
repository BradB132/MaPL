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
#include <libxml/tree.h>

#include "EaSLParser.h"
#include "MaPLInterface.h"
#include "MaPLGeneratorCollections.h"
#include "ErrorLogger.h"
#include "XmlHandler.h"

class SchemaEnum : public MaPLInterface {
public:
    SchemaEnum(EaSLParser::EnumDefinitionContext *enumContext, ErrorLogger *errorLogger);
    virtual MaPLParameter invokeFunction(MaPLSymbol functionSymbol, const MaPLParameter *argv, MaPLParameterCount argc);
    virtual MaPLParameter invokeSubscript(MaPLParameter index);
    
    EaSLParser::EnumDefinitionContext *_enumContext;
    std::string _name;
    MaPLArrayMap<std::string> *_cases;
    MaPLArrayMap<std::string> *_annotations;
};

class SchemaAttribute : public MaPLInterface {
public:
    SchemaAttribute(EaSLParser::AttributeContext *attributeContext, const std::string &defaultNamespace, ErrorLogger *errorLogger);
    virtual MaPLParameter invokeFunction(MaPLSymbol functionSymbol, const MaPLParameter *argv, MaPLParameterCount argc);
    virtual MaPLParameter invokeSubscript(MaPLParameter index);
    
    EaSLParser::AttributeContext *_attributeContext;
    std::string _name;
    std::string _typeName;
    std::string _typeNamespace;
    bool _typeIsUIDReference;
    bool _typeIsClass;
    bool _typeIsEnum;
    MaPLArrayMap<std::string> *_annotations;
    MaPLArray<std::string> *_defaultValues;
    uint32_t _minOccurrences;
    uint32_t _maxOccurrences;
};

class SchemaClass : public MaPLInterface {
public:
    SchemaClass(EaSLParser::ClassDefinitionContext *classContext, const std::string &namespace_, ErrorLogger *errorLogger);
    virtual MaPLParameter invokeFunction(MaPLSymbol functionSymbol, const MaPLParameter *argv, MaPLParameterCount argc);
    virtual MaPLParameter invokeSubscript(MaPLParameter index);
    
    bool hasUID();
    
    EaSLParser::ClassDefinitionContext *_classContext;
    std::string _name;
    std::string _namespace;
    SchemaClass *_superclass;
    MaPLArrayMap<SchemaAttribute *> *_attributes;
    MaPLArrayMap<std::string> *_annotations;
};

class Schema : public MaPLInterface {
public:
    Schema(const std::filesystem::path &filePath, EaSLParser::SchemaContext *schemaContext);
    virtual MaPLParameter invokeFunction(MaPLSymbol functionSymbol, const MaPLParameter *argv, MaPLParameterCount argc);
    virtual MaPLParameter invokeSubscript(MaPLParameter index);
    
    EaSLParser::SchemaContext *_schemaContext;
    ErrorLogger _errorLogger;
    std::string _namespace;
    MaPLArrayMap<SchemaEnum *> *_enums;
    MaPLArrayMap<SchemaClass *> *_classes;
};

MaPLArrayMap<Schema *> *schemasForPaths(const std::vector<std::filesystem::path> &schemaPaths);

void validateXML(MaPLArray<XmlFile *> *xmlFiles, MaPLArrayMap<Schema *> *schemas);

#endif /* EaSLHandler_h */

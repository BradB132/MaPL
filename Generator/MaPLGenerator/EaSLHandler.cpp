//
//  EaSLHandler.cpp
//  MaPLGenerator
//
//  Created by Brad Bambara on 5/19/22.
//

#include "EaSLHandler.h"

#include <fstream>
#include <sstream>
#include <unordered_set>

#include "antlr4-runtime.h"
#include "EaSLLexer.h"

MaPLArrayMap<std::string> *parseAnnotations(const std::vector<antlr4::tree::TerminalNode *> &annotationNodes) {
    std::vector<std::string> backingVector;
    std::unordered_map<std::string, std::string> backingMap;
    for (antlr4::tree::TerminalNode *annotationNode : annotationNodes) {
        std::string tokenText = annotationNode->getText();
        std::string annotationString = tokenText.substr(1);
        backingVector.push_back(annotationString);
        backingMap[annotationString] = annotationString;
    }
    return new MaPLArrayMap<std::string>(backingVector, backingMap);
}

bool conflictsWithPrimitiveName(const std::string &typeName) {
    return typeName == "char" ||
        typeName == "int32" ||
        typeName == "int64" ||
        typeName == "uint32" ||
        typeName == "uint64" ||
        typeName == "float32" ||
        typeName == "float64" ||
        typeName == "bool" ||
        typeName == "string" ||
        typeName == "UID";
}

SchemaEnum::SchemaEnum(EaSLParser::EnumDefinitionContext *enumContext, ErrorLogger *errorLogger) :
_enumContext(enumContext),
_name(enumContext->enumName->getText()),
_annotations(parseAnnotations(enumContext->ANNOTATION())) {
    if (conflictsWithPrimitiveName(_name)) {
        errorLogger->logError(enumContext->enumName->start, "Enum name '"+_name+"' conflicts with the name of a primitive type.");
    }
    std::vector<std::string> cases;
    std::unordered_set<std::string> caseSet;
    for (EaSLParser::IdentifierContext *caseNode : enumContext->enumValue) {
        std::string caseText = caseNode->getText();
        cases.push_back(caseText);
        if (caseSet.count(caseText)) {
            errorLogger->logError(caseNode->start, "Case '"+caseText+"' exists more than once in enum '"+_name+"'.");
        }
        caseSet.insert(caseText);
    }
    _cases = new MaPLArray<std::string>(cases);
}

MaPLParameter SchemaEnum::invokeFunction(MaPLSymbol functionSymbol, const MaPLParameter *argv, MaPLParameterCount argc) {
    switch (functionSymbol) {
        case MaPLSymbols_SchemaEnum_annotations:
            return MaPLPointer(_annotations);
        case MaPLSymbols_SchemaEnum_cases:
            return MaPLPointer(_cases);
        case MaPLSymbols_SchemaEnum_name:
            return MaPLStringByReference(_name.c_str());
        default:
            return MaPLUninitialized();
    }
}

MaPLParameter SchemaEnum::invokeSubscript(MaPLParameter index) {
    return MaPLUninitialized();
}

uint32_t uintForSequenceLength(EaSLParser::SequenceDescriptorContext *sequenceDescriptor, EaSLParser::SequenceLengthContext *lengthContext, ErrorLogger *errorLogger) {
    std::string lengthText = lengthContext->getText();
    if (lengthText[0] == '-') {
        errorLogger->logError(sequenceDescriptor->start, "Error in sequence '"+sequenceDescriptor->getText()+"': sequence string '"+lengthText+"' cannot be negative.");
    }
    return (uint32_t)std::stoul(lengthText);
}

SchemaAttribute::SchemaAttribute(EaSLParser::AttributeContext *attributeContext, ErrorLogger *errorLogger) :
_attributeContext(attributeContext),
_name(attributeContext->identifier()->getText()),
_annotations(parseAnnotations(attributeContext->ANNOTATION())) {
    EaSLParser::TypeContext *typeContext = attributeContext->type();
    _typeIsUIDReference = typeContext->typeToken && typeContext->typeToken->getType() == EaSLParser::REFERENCE;
    
    EaSLParser::ClassTypeContext *classType = typeContext->classType();
    if (classType) {
        _typeName = classType->classIdentifier->getText();
        if (classType->namespaceIdentifier) {
            _typeNamespace = classType->namespaceIdentifier->getText();
        }
        _isPrimitiveType = _typeIsUIDReference;
    } else {
        _typeName = typeContext->getText();
        _isPrimitiveType = true;
    }
    
    EaSLParser::SequenceDescriptorContext *sequenceDescriptor = attributeContext->sequenceDescriptor();
    if (sequenceDescriptor) {
        std::vector<EaSLParser::SequenceLengthContext *> lengths = sequenceDescriptor->sequenceLength();
        _minOccurrences = 0;
        _maxOccurrences = UINT32_MAX;
        if (lengths.size() > 0) {
            EaSLParser::SequenceLengthContext *minLength = lengths[0];
            EaSLParser::SequenceLengthContext *maxLength = (lengths.size() > 1) ? lengths[1] : lengths[0];
            if (!minLength->SEQUENCE_WILDCARD()) {
                _minOccurrences = uintForSequenceLength(sequenceDescriptor, minLength, errorLogger);
            }
            if (!maxLength->SEQUENCE_WILDCARD()) {
                _maxOccurrences = uintForSequenceLength(sequenceDescriptor, maxLength, errorLogger);
            }
            if (_maxOccurrences < _minOccurrences) {
                errorLogger->logError(minLength->start, "Invalid range: '"+sequenceDescriptor->getText()+"' Minimum must be less than the maximum.");
            }
        }
    } else {
        _minOccurrences = 1;
        _maxOccurrences = 1;
    }
    
    std::vector<std::string> defaultValues;
    EaSLParser::DefaultValueContext *defaultContext = attributeContext->defaultValue();
    if (defaultContext) {
        for (EaSLParser::LiteralValueContext *literalContext : defaultContext->literalValue()) {
            size_t literalType = literalContext->literalToken->getType();
            std::string literalText = literalContext->getText();
            
            // Check the type of this literal to make sure it matches.
            bool valueMatchesType;
            if (_isPrimitiveType) {
                switch (typeContext->typeToken->getType()) {
                    case EaSLParser::DECL_CHAR: // Intentional fallthrough.
                    case EaSLParser::DECL_UINT32: // Intentional fallthrough.
                    case EaSLParser::DECL_UINT64:
                        // Number cannot be negative.
                        valueMatchesType = literalType == EaSLParser::LITERAL_INT && literalText[0] != '-';
                        break;
                    case EaSLParser::DECL_INT32: // Intentional fallthrough.
                    case EaSLParser::DECL_INT64:
                        valueMatchesType = literalType == EaSLParser::LITERAL_INT;
                        break;
                    case EaSLParser::DECL_FLOAT32: // Intentional fallthrough.
                    case EaSLParser::DECL_FLOAT64:
                        valueMatchesType = literalType == EaSLParser::LITERAL_INT || literalType == EaSLParser::LITERAL_FLOAT;
                        break;
                    case EaSLParser::DECL_BOOL:
                        valueMatchesType = literalType == EaSLParser::LITERAL_TRUE || literalType == EaSLParser::LITERAL_FALSE;
                        break;
                    case EaSLParser::DECL_STRING: // Intentional fallthrough.
                    case EaSLParser::DECL_UID: // Intentional fallthrough.
                    case EaSLParser::REFERENCE:
                        valueMatchesType = literalType == EaSLParser::LITERAL_STRING;
                        break;
                    default:
                        valueMatchesType = false;
                        break;
                }
            } else {
                valueMatchesType = literalType == EaSLParser::LITERAL_NULL;
            }
            if (!valueMatchesType) {
                errorLogger->logError(typeContext->typeToken, "Default value '"+literalText+"' in attribute '"+_name+"' doesn't match the attribute's type '"+typeContext->getText()+"'.");
            }
            defaultValues.push_back(literalText);
        }
        if (defaultValues.size() < _minOccurrences) {
            errorLogger->logError(defaultContext->start, "Number of default values in '"+defaultContext->getText()+"' is less than the required minimum of "+std::to_string(_minOccurrences)+".");
        }
        if (defaultValues.size() > _maxOccurrences) {
            errorLogger->logError(defaultContext->start, "Number of default values in '"+defaultContext->getText()+"' is greater than the allowed maximum of "+std::to_string(_maxOccurrences)+".");
        }
    }
    _defaultValues = new MaPLArray<std::string>(defaultValues);
}

MaPLParameter SchemaAttribute::invokeFunction(MaPLSymbol functionSymbol, const MaPLParameter *argv, MaPLParameterCount argc) {
    switch (functionSymbol) {
        case MaPLSymbols_SchemaAttribute_annotations:
            return MaPLPointer(_annotations);
        case MaPLSymbols_SchemaAttribute_defaultValues:
            return MaPLPointer(_defaultValues);
        case MaPLSymbols_SchemaAttribute_isPrimitiveType:
            return MaPLBool(_isPrimitiveType);
        case MaPLSymbols_SchemaAttribute_maxOccurrences:
            return MaPLUint32(_maxOccurrences);
        case MaPLSymbols_SchemaAttribute_minOccurrences:
            return MaPLUint32(_minOccurrences);
        case MaPLSymbols_SchemaAttribute_name:
            return MaPLStringByReference(_name.c_str());
        case MaPLSymbols_SchemaAttribute_typeIsUIDReference:
            return MaPLBool(_typeIsUIDReference);
        case MaPLSymbols_SchemaAttribute_typeName:
            return MaPLStringByReference(_typeName.c_str());
        case MaPLSymbols_SchemaAttribute_typeNamespace:
            return MaPLStringByReference(_typeNamespace.c_str());
        default:
            return MaPLUninitialized();
    }
}

MaPLParameter SchemaAttribute::invokeSubscript(MaPLParameter index) {
    return MaPLUninitialized();
}

SchemaClass::SchemaClass(EaSLParser::ClassDefinitionContext *classContext, ErrorLogger *errorLogger) :
_classContext(classContext),
_name(classContext->identifier()->getText()),
_annotations(parseAnnotations(classContext->ANNOTATION())) {
    if (conflictsWithPrimitiveName(_name)) {
        errorLogger->logError(classContext->identifier()->start, "Class name '"+_name+"' conflicts with the name of a primitive type.");
    }
    EaSLParser::ClassTypeContext *superClass = classContext->classType();
    if (superClass) {
        _superclass = superClass->classIdentifier->getText();
        if (superClass->namespaceIdentifier) {
            _superclassNamespace = superClass->namespaceIdentifier->getText();
        }
    }
    std::vector<SchemaAttribute *> attributes;
    std::unordered_map<std::string, SchemaAttribute *> attributeMap;
    for (EaSLParser::AttributeContext *attributeContext : classContext->attribute()) {
        SchemaAttribute *attribute = new SchemaAttribute(attributeContext, errorLogger);
        attributes.push_back(attribute);
        if (attributeMap.count(attribute->_name)) {
            errorLogger->logError(attributeContext->start, "Attribute name '"+attribute->_name+"' conflicts with a attribute of the same name in class '"+_name+"'.");
        }
        attributeMap[attribute->_name] = attribute;
    }
    _attributes = new MaPLArrayMap<SchemaAttribute *>(attributes, attributeMap);
}

MaPLParameter SchemaClass::invokeFunction(MaPLSymbol functionSymbol, const MaPLParameter *argv, MaPLParameterCount argc) {
    switch (functionSymbol) {
        case MaPLSymbols_SchemaClass_annotations:
            return MaPLPointer(_annotations);
        case MaPLSymbols_SchemaClass_attributes:
            return MaPLPointer(_attributes);
        case MaPLSymbols_SchemaClass_name:
            return MaPLStringByReference(_name.c_str());
        case MaPLSymbols_SchemaClass_superclass:
            return MaPLStringByReference(_superclass.c_str());
        case MaPLSymbols_SchemaClass_superclassNamespace:
            return MaPLStringByReference(_superclassNamespace.c_str());
        default:
            return MaPLUninitialized();
    }
}

MaPLParameter SchemaClass::invokeSubscript(MaPLParameter index) {
    return MaPLUninitialized();
}

bool SchemaClass::hasUID() {
    for (SchemaAttribute *attribute : _attributes->_backingVector) {
        if (attribute->_typeName == "UID") {
            return true;
        }
    }
    return false;
}

Schema::Schema(const std::filesystem::path &filePath, EaSLParser::SchemaContext *schemaContext) :
_schemaContext(schemaContext),
_namespace(schemaContext->namespace_()->identifier()->getText()),
_errorLogger(filePath) {
    std::vector<SchemaEnum *> enums;
    std::unordered_map<std::string, SchemaEnum *> enumMap;
    for (EaSLParser::EnumDefinitionContext *enumContext : schemaContext->enumDefinition()) {
        SchemaEnum *enumObj = new SchemaEnum(enumContext, &_errorLogger);
        enums.push_back(enumObj);
        enumMap[enumObj->_name] = enumObj;
    }
    _enums = new MaPLArrayMap<SchemaEnum *>(enums, enumMap);
    
    std::vector<SchemaClass *> classes;
    std::unordered_map<std::string, SchemaClass *> classMap;
    for (EaSLParser::ClassDefinitionContext *classContext : schemaContext->classDefinition()) {
        SchemaClass *classObj = new SchemaClass(classContext, &_errorLogger);
        classes.push_back(classObj);
        classMap[classObj->_name] = classObj;
    }
    _classes = new MaPLArrayMap<SchemaClass *>(classes, classMap);
}

MaPLParameter Schema::invokeFunction(MaPLSymbol functionSymbol, const MaPLParameter *argv, MaPLParameterCount argc) {
    switch (functionSymbol) {
        case MaPLSymbols_Schema_classes:
            return MaPLPointer(_classes);
        case MaPLSymbols_Schema_enums:
            return MaPLPointer(_enums);
        case MaPLSymbols_Schema_namespace:
            return MaPLStringByReference(_namespace.c_str());
        default:
            break;
    }
    return MaPLUninitialized();
}

MaPLParameter Schema::invokeSubscript(MaPLParameter index) {
    return MaPLUninitialized();
}

void validateSchemas(MaPLArrayMap<Schema *> *schemas) {
    std::unordered_set<std::string> topLevelNames;
    for (Schema *schema : schemas->_backingVector) {
        for (SchemaClass *schemaClass : schema->_classes->_backingVector) {
            if (topLevelNames.count(schemaClass->_name)) {
                schema->_errorLogger.logError(schemaClass->_classContext->identifier()->start, "Class '"+schema->_namespace+"::"+schemaClass->_name+"' conflicts with another declaration of the same name.");
            }
            if (!schemaClass->_superclass.empty()) {
                Schema *superclassSchema = schemaClass->_superclassNamespace.empty() ? schema : schemas->_backingMap[schemaClass->_superclassNamespace];
                if (!superclassSchema || !superclassSchema->_classes->_backingMap[schemaClass->_superclass]) {
                    schema->_errorLogger.logError(schemaClass->_classContext->classType()->start,
                                                  "Class '"+schema->_namespace+"::"+schemaClass->_name+"' references a superclass '"+schemaClass->_superclassNamespace+"::"+schemaClass->_superclass+"' which doesn't exist.");
                }
            }
            for (SchemaAttribute *attribute : schemaClass->_attributes->_backingVector) {
                // Iterate through all super classes to confirm that there are no attribute collisions.
                SchemaClass *superclass = schemaClass;
                while (superclass) {
                    if (superclass->_superclass.empty()) { break; }
                    Schema *superclassSchema = superclass->_superclassNamespace.empty() ? schema : schemas->_backingMap[superclass->_superclassNamespace];
                    if (!superclassSchema) { break; }
                    superclass = superclassSchema->_classes->_backingMap[superclass->_superclass];
                    if (!superclass) { break; }
                    if (superclass->_attributes->_backingMap[attribute->_name]) {
                        schema->_errorLogger.logError(attribute->_attributeContext->start,
                                                      "Attribute '"+schema->_namespace+"::"+schemaClass->_name+"::"+attribute->_name+"' conflicts with attribute '"+superclassSchema->_namespace+"::"+superclass->_name+"::"+attribute->_name+"'.");
                    }
                }
                if (!attribute->_isPrimitiveType) {
                    Schema *typeSchema = attribute->_typeNamespace.empty() ? schema : schemas->_backingMap[attribute->_typeNamespace];
                    if (!typeSchema ||
                        (!typeSchema->_classes->_backingMap[attribute->_typeName] &&
                        !typeSchema->_enums->_backingMap[attribute->_typeName])) {
                        std::string typeNamespace = attribute->_typeNamespace.empty() ? schema->_namespace : attribute->_typeNamespace;
                        schema->_errorLogger.logError(attribute->_attributeContext->start,
                                                      "Can't find class or enum '"+typeNamespace+"::"+attribute->_typeName+"' referenced by '"+schema->_namespace+"::"+schemaClass->_name+"::"+attribute->_name+"'.");
                    }
                }
                if (attribute->_typeIsUIDReference) {
                    Schema *typeSchema = attribute->_typeNamespace.empty() ? schema : schemas->_backingMap[attribute->_typeNamespace];
                    SchemaClass *typeClass = typeSchema ? typeSchema->_classes->_backingMap[attribute->_typeName] : NULL;
                    if (!typeClass) {
                        std::string typeNamespace = attribute->_typeNamespace.empty() ? schema->_namespace : attribute->_typeNamespace;
                        schema->_errorLogger.logError(attribute->_attributeContext->start,
                                                      "Can't find class '"+typeNamespace+"::"+attribute->_typeName+"' referenced by '"+schema->_namespace+"::"+schemaClass->_name+"::"+attribute->_name+"'.");
                    }
                    bool typeClassHasUID = false;
                    SchemaClass *typeSuperclass = typeClass;
                    while (typeSuperclass) {
                        if (typeSuperclass->hasUID()) {
                            typeClassHasUID = true;
                            break;
                        }
                        if (typeSuperclass->_superclass.empty()) { break; }
                        Schema *typeSuperclassSchema = typeSuperclass->_superclassNamespace.empty() ? schema : schemas->_backingMap[typeSuperclass->_superclassNamespace];
                        if (!typeSuperclassSchema) { break; }
                        typeSuperclass = typeSuperclassSchema->_classes->_backingMap[typeSuperclass->_superclass];
                    }
                    if (!typeClassHasUID) {
                        std::string typeNamespace = attribute->_typeNamespace.empty() ? schema->_namespace : attribute->_typeNamespace;
                        schema->_errorLogger.logError(attribute->_attributeContext->start,
                                                      "Attribute '"+schema->_namespace+"::"+schemaClass->_name+"::"+attribute->_name+"' references class '"+typeNamespace+"::"+attribute->_typeName+"', but that class has no UID attribute that could match this reference.");
                    }
                }
            }
        }
        for (SchemaEnum *schemaEnum : schema->_enums->_backingVector) {
            if (topLevelNames.count(schemaEnum->_name)) {
                schema->_errorLogger.logError(schemaEnum->_enumContext->start, "Enum '"+schema->_namespace+"::"+schemaEnum->_name+"' conflicts with another declaration of the same name.");
            }
        }
        // EaSL uses namespaces to distinguish between objects from different schemas.
        topLevelNames.clear();
    }
}

class EaSLErrorListener : public antlr4::BaseErrorListener {
public:
    ErrorLogger *_errorLogger;
    virtual void syntaxError(antlr4::Recognizer *recognizer,
                             antlr4::Token * offendingSymbol,
                             size_t line,
                             size_t charPositionInLine,
                             const std::string &msg,
                             std::exception_ptr e) override {
        _errorLogger->logError(offendingSymbol, msg);
    }
};

MaPLArrayMap<Schema *> *schemasForPaths(const std::vector<std::filesystem::path> &schemaPaths) {
    std::vector<Schema *> schemasVector;
    EaSLErrorListener errListener;
    for (const std::filesystem::path &schemaPath : schemaPaths) {
        // Read the raw schema from the file system.
        std::ifstream inputStream(schemaPath);
        if (!inputStream) {
            fprintf(stderr, "Unable to read schema file at path '%s'.\n", schemaPath.c_str());
            exit(1);
        }
        std::stringstream stringBuffer;
        stringBuffer << inputStream.rdbuf();
        std::string rawSchemaText = stringBuffer.str();
        
        antlr4::ANTLRInputStream antlrInputStream(rawSchemaText);
        EaSLLexer lexer(&antlrInputStream);
        antlr4::CommonTokenStream tokenStream(&lexer);
        EaSLParser parser(&tokenStream);
        
        ErrorLogger errLogger{ schemaPath };
        errListener._errorLogger = &errLogger;
        lexer.getErrorListenerDispatch().removeErrorListeners();
        parser.getErrorListenerDispatch().removeErrorListeners();
        lexer.addErrorListener(&errListener);
        parser.addErrorListener(&errListener);
        
        EaSLParser::SchemaContext *schemaContext = parser.schema();
        if (errListener._errorLogger->_hasLoggedError) {
            exit(1);
        }
        
        schemasVector.push_back(new Schema(schemaPath, schemaContext));
    }
    
    std::unordered_map<std::string, Schema *> schemasMap;
    for (Schema *schema : schemasVector) {
        schemasMap[schema->_namespace] = schema;
    }
    
    MaPLArrayMap<Schema *> *schemas = new MaPLArrayMap<Schema *>(schemasVector, schemasMap);
    validateSchemas(schemas);
    
    for (Schema *schema : schemasVector) {
        if (schema->_errorLogger._hasLoggedError) {
            exit(1);
        }
    }
    
    return schemas;
}

void validateXMLNode(XmlNode *xmlNode,
                     MaPLArrayMap<Schema *> *schemas,
                     std::unordered_set<std::string> &uidSet,
                     ErrorLogger &errorLogger) {
    Schema *schema = schemas->_backingMap[xmlNode->_namespace];
    if (!schema) {
        errorLogger.logError(xmlNode->_node, "Unable to find schema with namespace '"+xmlNode->_namespace+"'.");
        return;
    }
    SchemaClass *schemaClass = schema->_classes->_backingMap[xmlNode->_name];
    if (!schemaClass) {
        errorLogger.logError(xmlNode->_node, "Unable to find schema class '"+xmlNode->_name+"' in namespace '"+xmlNode->_namespace+"'.");
        return;
    }
    
    for (XmlAttribute *xmlAttribute : xmlNode->_attributes->_backingVector) {
        // Search the inhertiance graph for the schema attribute.
        SchemaClass *attributeParentClass = schemaClass;
        SchemaAttribute *schemaAttribute = NULL;
        while (attributeParentClass) {
            schemaAttribute = attributeParentClass->_attributes->_backingMap[xmlAttribute->_name];
            if (schemaAttribute || attributeParentClass->_superclass.empty()) {
                break;
            }
            Schema *attributeParentSchema = schemas->_backingMap[attributeParentClass->_superclassNamespace];
            if (!attributeParentSchema) {
                break;
            }
            attributeParentClass = attributeParentSchema->_classes->_backingMap[attributeParentClass->_superclass];
        }
        if (!schemaAttribute) {
            errorLogger.logError(xmlNode->_node, "XML node specifies attribute '"+xmlAttribute->_name+"' which does not appear in schema class '"+schema->_namespace+"::"+schemaClass->_name+"' or any superclasses.");
            continue;
        }
        
        // TODO: Make sure types of xml and schema attributes match.
    }
    
    for (XmlNode *child : xmlNode->_children->_backingVector) {
        validateXMLNode(child, schemas, uidSet, errorLogger);
    }
}

void validateXML(MaPLArray<XmlNode *> *xmlNodes, MaPLArrayMap<Schema *> *schemas) {
    bool hasLoggedError = false;
    std::unordered_set<std::string> uidSet;
    for (XmlNode *xmlNode : xmlNodes->_backingVector) {
        ErrorLogger errorLogger((char *)xmlNode->_node->doc->URL);
        validateXMLNode(xmlNode, schemas, uidSet, errorLogger);
        if (errorLogger._hasLoggedError) {
            hasLoggedError = true;
        }
    }
    if (hasLoggedError) {
        exit(1);
    }
}

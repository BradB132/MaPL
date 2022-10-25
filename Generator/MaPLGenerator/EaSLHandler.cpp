//
//  EaSLHandler.cpp
//  MaPLGenerator
//
//  Created by Brad Bambara on 5/19/22.
//

#include "EaSLHandler.h"

#include <fstream>
#include <sstream>
#include <unordered_map>

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
    std::unordered_map<std::string, std::string> caseMap;
    for (EaSLParser::IdentifierContext *caseNode : enumContext->enumValue) {
        std::string caseText = caseNode->getText();
        cases.push_back(caseText);
        if (caseMap.count(caseText)) {
            errorLogger->logError(caseNode->start, "Case '"+caseText+"' exists more than once in enum '"+_name+"'.");
        }
        caseMap[caseText] = caseText;
    }
    _cases = new MaPLArrayMap<std::string>(cases, caseMap);
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
    // Check if already 'hasLoggedError' because we tend to do this same check for identical min and max values.
    if (lengthText[0] == '-' && !errorLogger->_hasLoggedError) {
        errorLogger->logError(sequenceDescriptor->start, "Error in sequence '"+sequenceDescriptor->getText()+"': sequence string '"+lengthText+"' cannot be negative.");
    }
    return (uint32_t)std::stoul(lengthText);
}

SchemaAttribute::SchemaAttribute(EaSLParser::AttributeContext *attributeContext, const std::string &defaultNamespace, ErrorLogger *errorLogger) :
_attributeContext(attributeContext),
_name(attributeContext->identifier()->getText()),
_annotations(parseAnnotations(attributeContext->ANNOTATION())),
_typeIsClass(false),
_typeIsEnum(false) {
    EaSLParser::TypeContext *typeContext = attributeContext->type();
    _typeIsUIDReference = typeContext->typeToken && typeContext->typeToken->getType() == EaSLParser::REFERENCE;
    
    EaSLParser::ClassTypeContext *classType = typeContext->classType();
    bool isPrimitiveType;
    if (classType) {
        _typeName = classType->classIdentifier->getText();
        if (classType->namespaceIdentifier) {
            _typeNamespace = classType->namespaceIdentifier->getText();
        } else {
            _typeNamespace = defaultNamespace;
        }
        isPrimitiveType = _typeIsUIDReference;
    } else {
        _typeName = typeContext->getText();
        isPrimitiveType = true;
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
                if (_maxOccurrences < 1) {
                    errorLogger->logError(maxLength->start, "The maximum length for a sequence must be at least 1.");
                }
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
            size_t literalType = literalContext->literalToken ? literalContext->literalToken->getType() : EaSLParser::IDENTIFIER;
            std::string literalText = literalContext->getText();
            
            if (isPrimitiveType) {
                // We only have enough information in this context to judge for primitives whether the
                // literal type matches the declared type. For complex types (class and enum) the equivalent
                // check must be performed after we've built up a mapping of all types in the schema.
                bool valueMatchesType;
                size_t tokenType = typeContext->typeToken->getType();
                switch (tokenType) {
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
                if (!valueMatchesType) {
                    std::string typeClarification = (tokenType == EaSLParser::DECL_UID || tokenType == EaSLParser::REFERENCE) ? " (aka string)" : "";
                    errorLogger->logError(typeContext->typeToken, "Default value '"+literalText+"' in attribute '"+_name+"' doesn't match the attribute's type '"+typeContext->getText()+"'"+typeClarification+".");
                }
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
        case MaPLSymbols_SchemaAttribute_maxOccurrences:
            return MaPLUint32(_maxOccurrences);
        case MaPLSymbols_SchemaAttribute_minOccurrences:
            return MaPLUint32(_minOccurrences);
        case MaPLSymbols_SchemaAttribute_name:
            return MaPLStringByReference(_name.c_str());
        case MaPLSymbols_SchemaAttribute_typeIsClass:
            return MaPLBool(_typeIsClass);
        case MaPLSymbols_SchemaAttribute_typeIsEnum:
            return MaPLBool(_typeIsEnum);
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

SchemaClass::SchemaClass(EaSLParser::ClassDefinitionContext *classContext, const std::string &namespace_, ErrorLogger *errorLogger) :
_classContext(classContext),
_name(classContext->identifier()->getText()),
_namespace(namespace_),
_superclass(NULL),
_descendantClasses(NULL),
_annotations(parseAnnotations(classContext->ANNOTATION())) {
    if (conflictsWithPrimitiveName(_name)) {
        errorLogger->logError(classContext->identifier()->start, "Class name '"+_name+"' conflicts with the name of a primitive type.");
    }
    std::vector<SchemaAttribute *> attributes;
    std::unordered_map<std::string, SchemaAttribute *> attributeMap;
    for (EaSLParser::AttributeContext *attributeContext : classContext->attribute()) {
        SchemaAttribute *attribute = new SchemaAttribute(attributeContext, namespace_, errorLogger);
        attributes.push_back(attribute);
        if (attributeMap.count(attribute->_name)) {
            errorLogger->logError(attributeContext->start, "Attribute name '"+attribute->_name+"' conflicts with an attribute of the same name in class '"+_name+"'.");
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
        case MaPLSymbols_SchemaClass_descendantClasses:
            return MaPLPointer(_descendantClasses);
        case MaPLSymbols_SchemaClass_name:
            return MaPLStringByReference(_name.c_str());
        case MaPLSymbols_SchemaClass_namespace:
            return MaPLStringByReference(_namespace.c_str());
        case MaPLSymbols_SchemaClass_superclass:
            return MaPLPointer(_superclass);
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

void SchemaClass::initializeDescendantList(MaPLArrayMap<Schema *> *schemas) {
    if (_descendantClasses) {
        return;
    }
    std::vector<SchemaClass *> descendants;
    for (const Schema *schema : schemas->_backingVector) {
        for (SchemaClass *schemaClass : schema->_classes->_backingVector) {
            if (schemaClass == this || schemaClass->_superclass != this) {
                continue;
            }
            if (!schemaClass->_descendantClasses) {
                // If this subclass's descendant list is not initialized, initialize it now. This will
                // cause the initialization of these lists to happen in the correct topological order.
                schemaClass->initializeDescendantList(schemas);
            }
            descendants.insert(descendants.end(), schemaClass->_descendantClasses->_backingVector.begin(), schemaClass->_descendantClasses->_backingVector.end());
            descendants.push_back(schemaClass);
        }
    }
    _descendantClasses = new MaPLArray<SchemaClass *>(descendants);
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
        SchemaClass *classObj = new SchemaClass(classContext, _namespace, &_errorLogger);
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
            topLevelNames.insert(schemaClass->_name);
            for (SchemaAttribute *attribute : schemaClass->_attributes->_backingVector) {
                // Iterate through all super classes to confirm that there are no attribute collisions.
                SchemaClass *superclass = schemaClass;
                while (superclass) {
                    superclass = superclass->_superclass;
                    if (!superclass) { break; }
                    if (superclass->_attributes->_backingMap.count(attribute->_name)) {
                        schema->_errorLogger.logError(attribute->_attributeContext->start,
                                                      "Attribute '"+schema->_namespace+"::"+schemaClass->_name+"::"+attribute->_name+"' conflicts with attribute '"+superclass->_namespace+"::"+superclass->_name+"::"+attribute->_name+"'.");
                    }
                }
                if (!attribute->_typeNamespace.empty() && !attribute->_typeIsUIDReference) {
                    // Now that all schema objects are allocated, we have enough context to determine if this is a class or enum.
                    Schema *typeSchema = schemas->_backingMap.at(attribute->_typeNamespace);
                    if (typeSchema) {
                        attribute->_typeIsClass = typeSchema->_classes->_backingMap.count(attribute->_typeName) > 0;
                        attribute->_typeIsEnum = typeSchema->_enums->_backingMap.count(attribute->_typeName) > 0;
                        if (attribute->_typeIsClass) {
                            for (const std::string &defaultValue : attribute->_defaultValues->_backingVector) {
                                if (defaultValue != "NULL") {
                                    SchemaClass *errClass = typeSchema->_classes->_backingMap.at(attribute->_typeName);
                                    schema->_errorLogger.logError(attribute->_attributeContext->defaultValue()->start, "Default value of '"+defaultValue+"' was specified for an attribute of type '"+typeSchema->_namespace+"::"+errClass->_name+"'. Default values for classes can only be 'NULL'.");
                                }
                            }
                        } else if (attribute->_typeIsEnum) {
                            SchemaEnum *defaultsEnum = typeSchema->_enums->_backingMap.at(attribute->_typeName);
                            for (const std::string &defaultValue : attribute->_defaultValues->_backingVector) {
                                if (!defaultsEnum->_cases->_backingMap.count(defaultValue)) {
                                    schema->_errorLogger.logError(attribute->_attributeContext->defaultValue()->start, "Default value of '"+defaultValue+"' does not appear in enum '"+typeSchema->_namespace+"::"+defaultsEnum->_name+"'.");
                                }
                            }
                        }
                    }
                    if (!typeSchema || (!attribute->_typeIsClass && !attribute->_typeIsEnum)) {
                        std::string typeNamespace = attribute->_typeNamespace;
                        schema->_errorLogger.logError(attribute->_attributeContext->start,
                                                      "Can't find class or enum '"+typeNamespace+"::"+attribute->_typeName+"' referenced by '"+schema->_namespace+"::"+schemaClass->_name+"::"+attribute->_name+"'.");
                    }
                }
                if (attribute->_typeIsUIDReference) {
                    Schema *typeSchema = schemas->_backingMap.at(attribute->_typeNamespace);
                    SchemaClass *typeClass = typeSchema ? typeSchema->_classes->_backingMap.at(attribute->_typeName) : NULL;
                    if (!typeClass) {
                        schema->_errorLogger.logError(attribute->_attributeContext->start,
                                                      "Can't find class '"+attribute->_typeNamespace+"::"+attribute->_typeName+"' referenced by '"+schema->_namespace+"::"+schemaClass->_name+"::"+attribute->_name+"'.");
                    }
                    bool typeClassHasUID = false;
                    SchemaClass *typeSuperclass = typeClass;
                    while (typeSuperclass) {
                        if (typeSuperclass->hasUID()) {
                            typeClassHasUID = true;
                            break;
                        }
                        if (!typeSuperclass->_superclass) { break; }
                        typeSuperclass = typeSuperclass->_superclass;
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
            topLevelNames.insert(schemaEnum->_name);
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
        
        // The subsequent schema context object is kept alive only as long as these lexer/parser objects.
        // These objects are intentionally allocated and never deleted to keep that schema context alive.
        // This tradeoff is worth it because this function is only invoked once, and these objects are
        // needed until the program's termination. Properly managing this memory would add a lot of bookkeeping.
        antlr4::ANTLRInputStream *antlrInputStream = new antlr4::ANTLRInputStream(rawSchemaText);
        EaSLLexer *lexer = new EaSLLexer(antlrInputStream);
        antlr4::CommonTokenStream *tokenStream = new antlr4::CommonTokenStream(lexer);
        EaSLParser *parser = new EaSLParser(tokenStream);
        
        ErrorLogger errLogger{ schemaPath };
        errListener._errorLogger = &errLogger;
        lexer->getErrorListenerDispatch().removeErrorListeners();
        parser->getErrorListenerDispatch().removeErrorListeners();
        lexer->addErrorListener(&errListener);
        parser->addErrorListener(&errListener);
        
        EaSLParser::SchemaContext *schemaContext = parser->schema();
        if (errListener._errorLogger->_hasLoggedError) {
            exit(1);
        }
        
        schemasVector.push_back(new Schema(schemaPath, schemaContext));
    }
    
    std::unordered_map<std::string, Schema *> schemasMap;
    for (Schema *schema : schemasVector) {
        schemasMap[schema->_namespace] = schema;
    }
    
    // SchemaClass references its own superclass, make those connections now that all objects are initialized.
    for (Schema *schema : schemasVector) {
        for (SchemaClass *schemaClass : schema->_classes->_backingVector) {
            EaSLParser::ClassTypeContext *superClass = schemaClass->_classContext->classType();
            if (superClass) {
                std::string superclassName = superClass->classIdentifier->getText();
                std::string superclassNamespace;
                if (superClass->namespaceIdentifier) {
                    superclassNamespace = superClass->namespaceIdentifier->getText();
                } else {
                    superclassNamespace = schemaClass->_namespace;
                }
                if (!schemasMap.count(superclassNamespace)) {
                    schema->_errorLogger.logError(schemaClass->_classContext->classType()->start,
                                                  "Superclass for class '"+schemaClass->_namespace+"::"+schemaClass->_name+"' references a namespace '"+superclassNamespace+"' which doesn't exist.");
                    continue;
                }
                Schema *superclassSchema = schemasMap.at(superclassNamespace);
                if (!superclassSchema->_classes->_backingMap.count(superclassName)) {
                    schema->_errorLogger.logError(schemaClass->_classContext->classType()->start,
                                                  "Class '"+schemaClass->_namespace+"::"+schemaClass->_name+"' references a superclass '"+superclassNamespace+"::"+superclassName+"' which doesn't exist.");
                    continue;
                }
                schemaClass->_superclass = superclassSchema->_classes->_backingMap.at(superclassName);
            }
        }
    }
    
    MaPLArrayMap<Schema *> *schemas = new MaPLArrayMap<Schema *>(schemasVector, schemasMap);
    validateSchemas(schemas);
    
    // Now that superclasses are populated, there's enough information to populate the descendant graph.
    for (Schema *schema : schemasVector) {
        for (SchemaClass *schemaClass : schema->_classes->_backingVector) {
            schemaClass->initializeDescendantList(schemas);
        }
    }
    
    for (Schema *schema : schemasVector) {
        if (schema->_errorLogger._hasLoggedError) {
            exit(1);
        }
    }
    
    return schemas;
}

SchemaAttribute *schemaAttributeForXmlAttribute(XmlAttribute *xmlAttribute, MaPLArrayMap<Schema *> *schemas, SchemaClass *schemaClass) {
    SchemaClass *attributeParentClass = schemaClass;
    SchemaAttribute *schemaAttribute = NULL;
    while (attributeParentClass) {
        if (attributeParentClass->_attributes->_backingMap.count(xmlAttribute->_name)) {
            schemaAttribute = attributeParentClass->_attributes->_backingMap.at(xmlAttribute->_name);
        }
        if (schemaAttribute || !attributeParentClass->_superclass) {
            break;
        }
        attributeParentClass = attributeParentClass->_superclass;
    }
    return schemaAttribute;
}

bool isKindOfClass(const std::string &subclassName, const std::string &subclassNamespace,
                   const std::string &superclassName, const std::string &superclassNamespace,
                   MaPLArrayMap<Schema *> *schemas) {
    if (subclassName == superclassName && subclassNamespace == superclassNamespace) {
        return true;
    }
    SchemaClass *schemaClass = schemas->_backingMap.at(subclassNamespace)->_classes->_backingMap.at(subclassName);
    if (schemaClass->_superclass) {
        return isKindOfClass(schemaClass->_superclass->_name, schemaClass->_superclass->_namespace, superclassName, superclassNamespace, schemas);
    }
    return false;
}

bool confirmStringUnsigned(const std::string &numericString, XmlNode *node, XmlAttribute *xmlAttribute, ErrorLogger &errorLogger) {
    if (numericString.find('-') != std::string::npos) {
        errorLogger.logError(node->_node, "Value '"+numericString+"' in attribute '"+xmlAttribute->_name+"' cannot be negative.");
        return false;
    }
    return true;
}

void firstPassXMLValidation(XmlNode *xmlNode, MaPLArrayMap<Schema *> *schemas, std::unordered_map<std::string, XmlNode *> &uidMap, ErrorLogger &errorLogger) {
    // Do an initial pass over all nodes and attributes in the dataset:
    //  - Confirm all namespaces, classes, and attributes exist in the schema.
    //  - Collect the UIDs, confirm uniqueness, and make a mapping of objects they point to.
    Schema *schema = schemas->_backingMap.at(xmlNode->_namespace);
    if (!schema) {
        errorLogger.logError(xmlNode->_node, "Unable to find schema with namespace '"+xmlNode->_namespace+"'.");
        return;
    }
    SchemaClass *schemaClass = schema->_classes->_backingMap.at(xmlNode->_name);
    if (!schemaClass) {
        errorLogger.logError(xmlNode->_node, "Unable to find schema class '"+xmlNode->_name+"' in namespace '"+xmlNode->_namespace+"'.");
        return;
    }
    for (XmlAttribute *xmlAttribute : xmlNode->_attributes->_backingVector) {
        SchemaAttribute *schemaAttribute = schemaAttributeForXmlAttribute(xmlAttribute, schemas, schemaClass);
        if (!schemaAttribute) {
            errorLogger.logError(xmlNode->_node, "XML node specifies attribute '"+xmlAttribute->_name+"' which does not appear in schema class '"+schema->_namespace+"::"+schemaClass->_name+"' or any superclasses.");
            continue;
        }
        if (schemaAttribute->_typeName != "UID" || xmlAttribute->_value.empty()) { continue; }
        for (const std::string &value : xmlAttribute->_values->_backingVector) {
            if (uidMap.count(value) > 0) {
                errorLogger.logError(xmlNode->_node, "UID '"+value+"' must be unique, but it is used elsewhere in the dataset.");
            }
            uidMap[value] = xmlNode;
        }
    }
    for (XmlNode *childNode : xmlNode->_children->_backingVector) {
        firstPassXMLValidation(childNode, schemas, uidMap, errorLogger);
    }
}

void secondPassXMLValidation(XmlNode *xmlNode, MaPLArrayMap<Schema *> *schemas, const std::unordered_map<std::string, XmlNode *> &uidMap, ErrorLogger &errorLogger) {
    Schema *schema = schemas->_backingMap.at(xmlNode->_namespace);
    SchemaClass *schemaClass = schema->_classes->_backingMap.at(xmlNode->_name);
    
    struct AttributeCount {
        Schema *schema;
        SchemaClass *schemaClass;
        SchemaAttribute *schemaAttribute;
        uint32_t childCount;
    };
    std::vector<AttributeCount> attributeCounts;
    
    // Check all primitive schema attributes in this class to make sure required attributes exist in the XML.
    SchemaClass *parentSchemaClass = schemaClass;
    Schema *parentSchema = schema;
    while (parentSchemaClass) {
        std::vector<AttributeCount> attributeCountsForClass;
        for (SchemaAttribute *schemaAttribute : parentSchemaClass->_attributes->_backingVector) {
            if (schemaAttribute->_typeIsClass) {
                // Record this attribute so the number of child nodes can later be validated.
                attributeCountsForClass.push_back({ parentSchema, parentSchemaClass, schemaAttribute, 0 });
            } else {
                // This attribute is a primitive or enum, so its value is specified inline.
                bool isRequired = schemaAttribute->_defaultValues->_backingVector.size() == 0;
                if (isRequired && !xmlNode->_attributes->_backingMap.count(schemaAttribute->_name)) {
                    std::string subclassDescriptor;
                    if (schemaClass == parentSchemaClass) {
                        subclassDescriptor = "";
                    } else {
                        subclassDescriptor = " (a superclass of '"+schema->_namespace+"::"+schemaClass->_name+"')";
                    }
                    errorLogger.logError(xmlNode->_node, "Attribute '"+schemaAttribute->_name+"' was not specified, but is required by schema class '"+parentSchema->_namespace+"::"+parentSchemaClass->_name+"'"+subclassDescriptor+".");
                }
            }
        }
        
        // Prepend the counts for this class onto the list of all attributes for this type.
        attributeCounts.insert(attributeCounts.begin(), attributeCountsForClass.begin(), attributeCountsForClass.end());
        
        if (!parentSchemaClass->_superclass) { break; }
        parentSchemaClass = parentSchemaClass->_superclass;
    }
    
    for (XmlAttribute *xmlAttribute : xmlNode->_attributes->_backingVector) {
        SchemaAttribute *schemaAttribute = schemaAttributeForXmlAttribute(xmlAttribute, schemas, schemaClass);
        
        // If this is a single string, then commas are part of the content and no meaningful validation of sequence length is possible.
        if (schemaAttribute->_typeName == "string" && schemaAttribute->_maxOccurrences == 1) {
            continue;
        }
        
        if (xmlAttribute->_values->_backingVector.size() < schemaAttribute->_minOccurrences ||
            xmlAttribute->_values->_backingVector.size() > schemaAttribute->_maxOccurrences) {
            std::string sequenceDescriptor;
            if (schemaAttribute->_minOccurrences == schemaAttribute->_maxOccurrences) {
                sequenceDescriptor = std::to_string(schemaAttribute->_minOccurrences);
            } else {
                sequenceDescriptor = "between "+std::to_string(schemaAttribute->_minOccurrences)+" and "+std::to_string(schemaAttribute->_maxOccurrences);
            }
            errorLogger.logError(xmlNode->_node, "The number of values specified by '"+xmlAttribute->_name+"' is "+std::to_string(xmlAttribute->_values->_backingVector.size())+", but was expecting "+sequenceDescriptor+".");
        }
        
        for (const std::string &attributeValue : xmlAttribute->_values->_backingVector) {
            if (schemaAttribute->_typeIsUIDReference) {
                // If this attribute references another node, make sure that node is what we expect.
                if (!uidMap.count(attributeValue)) {
                    errorLogger.logError(xmlNode->_node, "UID '"+attributeValue+"' does not refer to any object that exists in the dataset.");
                    continue;
                }
                XmlNode *referencedNode = uidMap.at(attributeValue);
                if (referencedNode->_name != schemaAttribute->_typeName ||
                    referencedNode->_namespace != schemaAttribute->_typeNamespace) {
                    errorLogger.logError(xmlNode->_node, "UID '"+attributeValue+"' was expected to refer to a node of type '"+schemaAttribute->_typeNamespace+"::"+schemaAttribute->_typeName+"', but matched a node of type '"+referencedNode->_namespace+"::"+referencedNode->_name+"' instead.");
                }
            } else if (schemaAttribute->_typeIsEnum &&
                       !schemas->_backingMap.at(schemaAttribute->_typeNamespace)->_enums->_backingMap.at(schemaAttribute->_typeName)->_cases->_backingMap.count(attributeValue)) {
                errorLogger.logError(xmlNode->_node, "Value '"+attributeValue+"' is not one of the cases in the '"+schemaAttribute->_typeNamespace+"::"+schemaAttribute->_typeName+"' enum.");
            } else if (schemaAttribute->_typeName == "char") {
                if (!confirmStringUnsigned(attributeValue, xmlNode, xmlAttribute, errorLogger)) {
                    continue;
                }
                char *endPointer;
                uint64_t longValue = strtoul(attributeValue.c_str(), &endPointer, 10);
                if (*endPointer) {
                    errorLogger.logError(xmlNode->_node, "Value '"+attributeValue+"' is not a valid char.");
                }
                if (longValue > UINT8_MAX) {
                    errorLogger.logError(xmlNode->_node, "Value '"+attributeValue+"' is too large to store in a char.");
                }
            } else if (schemaAttribute->_typeName == "int32") {
                char *endPointer;
                int64_t longValue = strtol(attributeValue.c_str(), &endPointer, 10);
                if (*endPointer) {
                    errorLogger.logError(xmlNode->_node, "Value '"+attributeValue+"' is not a valid int32.");
                }
                if (longValue > INT32_MAX || longValue < INT32_MIN) {
                    errorLogger.logError(xmlNode->_node, "Value '"+attributeValue+"' is too large to store in a int32.");
                }
            } else if (schemaAttribute->_typeName == "int64") {
                char *endPointer;
                strtol(attributeValue.c_str(), &endPointer, 10);
                if (*endPointer) {
                    errorLogger.logError(xmlNode->_node, "Value '"+attributeValue+"' is not a valid int64.");
                }
            } else if (schemaAttribute->_typeName == "uint32") {
                if (!confirmStringUnsigned(attributeValue, xmlNode, xmlAttribute, errorLogger)) {
                    continue;
                }
                char *endPointer;
                uint64_t longValue = strtoul(attributeValue.c_str(), &endPointer, 10);
                if (*endPointer) {
                    errorLogger.logError(xmlNode->_node, "Value '"+attributeValue+"' is not a valid uint32.");
                }
                if (longValue > UINT32_MAX) {
                    errorLogger.logError(xmlNode->_node, "Value '"+attributeValue+"' is too large to store in a uint32.");
                }
            } else if (schemaAttribute->_typeName == "uint64") {
                if (!confirmStringUnsigned(attributeValue, xmlNode, xmlAttribute, errorLogger)) {
                    continue;
                }
                char *endPointer;
                strtoul(attributeValue.c_str(), &endPointer, 10);
                if (*endPointer) {
                    errorLogger.logError(xmlNode->_node, "Value '"+attributeValue+"' is not a valid uint64.");
                }
            } else if (schemaAttribute->_typeName == "float32") {
                char *endPointer;
                strtof(attributeValue.c_str(), &endPointer);
                if (*endPointer) {
                    errorLogger.logError(xmlNode->_node, "Value '"+attributeValue+"' is not a valid float32.");
                }
            } else if (schemaAttribute->_typeName == "float64") {
                char *endPointer;
                strtod(attributeValue.c_str(), &endPointer);
                if (*endPointer) {
                    errorLogger.logError(xmlNode->_node, "Value '"+attributeValue+"' is not a valid float64.");
                }
            } else if (schemaAttribute->_typeName == "bool" &&
                       attributeValue != "true" &&
                       attributeValue != "false") {
                errorLogger.logError(xmlNode->_node, "Boolean attribute '"+xmlAttribute->_name+"' must have a value that is either 'true' or 'false'.");
            }
        }
    }
    for (XmlNode *childNode : xmlNode->_children->_backingVector) {
        // Slot this child into the first entry in the attributeCounts list where it fits.
        bool didMatchAttribute = false;
        AttributeCount *potentialMatch = NULL;
        for (AttributeCount &attributeCount : attributeCounts) {
            if (isKindOfClass(childNode->_name, childNode->_namespace,
                              attributeCount.schemaAttribute->_typeName, attributeCount.schemaAttribute->_typeNamespace,
                              schemas)) {
                potentialMatch = &attributeCount;
                if (attributeCount.childCount < attributeCount.schemaAttribute->_maxOccurrences) {
                    // Child successfully matched to a sequence. Count it and add to filtered children list.
                    attributeCount.childCount++;
                    
                    MaPLArray<XmlNode *> *sequenceArray;
                    if (xmlNode->_childrenBySequence->_backingMap.count(attributeCount.schemaAttribute->_name)) {
                        sequenceArray = xmlNode->_childrenBySequence->_backingMap.at(attributeCount.schemaAttribute->_name);
                        sequenceArray->_backingVector.push_back(childNode);
                    } else {
                        std::vector<XmlNode *> sequenceVector = { childNode };
                        sequenceArray = new MaPLArray<XmlNode *>(sequenceVector);
                    }
                    xmlNode->_childrenBySequence->_backingVector.push_back(sequenceArray);
                    xmlNode->_childrenBySequence->_backingMap[attributeCount.schemaAttribute->_name] = sequenceArray;
                    
                    didMatchAttribute = true;
                    break;
                }
            }
        }
        if (!didMatchAttribute) {
            if (potentialMatch) {
                errorLogger.logError(childNode->_node, "Attempted to match child node of type '"+childNode->_namespace+"::"+childNode->_name+"' to attribute '"+potentialMatch->schema->_namespace+"::"+potentialMatch->schemaClass->_name+"::"+potentialMatch->schemaAttribute->_name+"', but the limit of "+std::to_string(potentialMatch->schemaAttribute->_maxOccurrences)+" occurrences was exceeded.");
            } else {
                errorLogger.logError(childNode->_node, "Child node of type '"+childNode->_namespace+"::"+childNode->_name+"' does not match any attribute on parent type '"+xmlNode->_namespace+"::"+xmlNode->_name+"'.");
            }
        }
        
        secondPassXMLValidation(childNode, schemas, uidMap, errorLogger);
    }
    
    // The above logic will ensure no schema attribute exceeds its maximum occurrences.
    // Check each attribute count to ensure the minimums were met.
    for (const AttributeCount &attributeCount : attributeCounts) {
        if (attributeCount.childCount < attributeCount.schemaAttribute->_minOccurrences) {
            errorLogger.logError(xmlNode->_node, "Node of type '"+xmlNode->_namespace+"::"+xmlNode->_name+"' expects at least "+std::to_string(attributeCount.schemaAttribute->_minOccurrences)+" children of type '"+attributeCount.schemaAttribute->_typeNamespace+"::"+attributeCount.schemaAttribute->_typeName+"', but found "+std::to_string(attributeCount.childCount)+".");
        }
    }
}

void validateXML(MaPLArray<XmlFile *> *xmlFiles, MaPLArrayMap<Schema *> *schemas) {
    bool hasLoggedError = false;
    
    std::unordered_map<std::string, XmlNode *> uidMap;
    for (XmlFile *xmlFile : xmlFiles->_backingVector) {
        ErrorLogger errorLogger(xmlFile->_filePath);
        firstPassXMLValidation(xmlFile->_rootNode, schemas, uidMap, errorLogger);
        if (errorLogger._hasLoggedError) {
            hasLoggedError = true;
        }
    }
    if (hasLoggedError) {
        exit(1);
    }
    for (XmlFile *xmlFile : xmlFiles->_backingVector) {
        ErrorLogger errorLogger(xmlFile->_filePath);
        secondPassXMLValidation(xmlFile->_rootNode, schemas, uidMap, errorLogger);
        if (errorLogger._hasLoggedError) {
            hasLoggedError = true;
        }
    }
    if (hasLoggedError) {
        exit(1);
    }
}

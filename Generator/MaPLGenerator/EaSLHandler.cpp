//
//  EaSLHandler.cpp
//  MaPLGenerator
//
//  Created by Brad Bambara on 5/19/22.
//

#include "EaSLHandler.h"

#include <fstream>
#include <sstream>

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

SchemaEnum::SchemaEnum(EaSLParser::EnumDefinitionContext *enumContext) :
_name(enumContext->enumName->getText()),
_annotations(parseAnnotations(enumContext->ANNOTATION())) {
    if (conflictsWithPrimitiveName(_name)) {
        fprintf(stderr, "Enum name '%s' conflicts with the name of a primitive type.\n", _name.c_str());
        exit(1);
    }
    std::vector<std::string> cases;
    std::set<std::string> caseSet;
    for (EaSLParser::IdentifierContext *caseNode : enumContext->enumValue) {
        std::string caseText = caseNode->getText();
        cases.push_back(caseText);
        if (caseSet.count(caseText)) {
            fprintf(stderr, "Case '%s' exists more than once in enum '%s'.\n", caseText.c_str(), _name.c_str());
            exit(1);
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

uint32_t uintForSequenceLength(EaSLParser::SequenceDescriptorContext *sequenceDescriptor, EaSLParser::SequenceLengthContext *lengthContext) {
    std::string lengthText = lengthContext->getText();
    if (lengthText[0] == '-') {
        fprintf(stderr, "Error in sequence '%s': sequence string '%s' cannot be negative.\n", sequenceDescriptor->getText().c_str(), lengthText.c_str());
        exit(1);
    }
    return (uint32_t)std::stoul(lengthText);
}

SchemaAttribute::SchemaAttribute(EaSLParser::AttributeContext *attributeContext) :
_name(attributeContext->identifier()->getText()),
_annotations(parseAnnotations(attributeContext->ANNOTATION())) {
    EaSLParser::TypeContext *typeContext = attributeContext->type();
    _typeIsUIDReference = typeContext->typeToken->getType() == EaSLParser::REFERENCE;
    
    EaSLParser::ClassTypeContext *classType = typeContext->classType();
    if (classType) {
        _typeName = classType->classIdentifier->getText();
        _typeNamespace = classType->namespaceIdentifier->getText();
        _isPrimitiveType = !_typeIsUIDReference;
    } else {
        _typeName = typeContext->getText();
        _isPrimitiveType = true;
    }
    
    EaSLParser::SequenceDescriptorContext *sequenceDescriptor = attributeContext->sequenceDescriptor();
    if (sequenceDescriptor) {
        std::vector<EaSLParser::SequenceLengthContext *> lengths = sequenceDescriptor->sequenceLength();
        EaSLParser::SequenceLengthContext *minLength = lengths[0];
        EaSLParser::SequenceLengthContext *maxLength = (lengths.size() > 1) ? lengths[1] : lengths[0];
        if (minLength->SEQUENCE_WILDCARD()) {
            _minOccurrences = 0;
        } else {
            _minOccurrences = uintForSequenceLength(sequenceDescriptor, minLength);
        }
        if (maxLength->SEQUENCE_WILDCARD()) {
            _maxOccurrences = UINT32_MAX;
        } else {
            _maxOccurrences = uintForSequenceLength(sequenceDescriptor, maxLength);
        }
        if (_maxOccurrences < _minOccurrences) {
            fprintf(stderr, "Invalid range: '%s' Minimum must be less than the maximum.", sequenceDescriptor->getText().c_str());
            exit(1);
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
                fprintf(stderr, "Default value '%s' in attribute '%s' doesn't match the attribute's type '%s'.\n", literalText.c_str(), _name.c_str(), typeContext->getText().c_str());
                exit(1);
            }
            defaultValues.push_back(literalText);
        }
        if (defaultValues.size() < _minOccurrences) {
            fprintf(stderr, "Number of default values '%s' is less than the minimum required number of values (%u).", defaultContext->getText().c_str(), _minOccurrences);
            exit(1);
        }
        if (defaultValues.size() > _minOccurrences) {
            fprintf(stderr, "Number of default values '%s' is greater than the maximum required number of values (%u).", defaultContext->getText().c_str(), _maxOccurrences);
            exit(1);
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

SchemaClass::SchemaClass(EaSLParser::ClassDefinitionContext *classContext) :
_name(classContext->name->getText()),
_superclass(classContext->superclass ? classContext->superclass->getText() : ""),
_annotations(parseAnnotations(classContext->ANNOTATION())) {
    if (conflictsWithPrimitiveName(_name)) {
        fprintf(stderr, "Class name '%s' conflicts with the name of a primitive type.\n", _name.c_str());
        exit(1);
    }
    std::vector<SchemaAttribute *> attributes;
    std::unordered_map<std::string, SchemaAttribute *> attributeMap;
    for (EaSLParser::AttributeContext *attributeContext : classContext->attribute()) {
        SchemaAttribute *attribute = new SchemaAttribute(attributeContext);
        attributes.push_back(attribute);
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
        default:
            return MaPLUninitialized();
    }
}

MaPLParameter SchemaClass::invokeSubscript(MaPLParameter index) {
    return MaPLUninitialized();
}

Schema::Schema(EaSLParser::SchemaContext *schemaContext) :
_namespace(schemaContext->namespace_()->identifier()->getText()) {
    std::vector<SchemaEnum *> enums;
    std::unordered_map<std::string, SchemaEnum *> enumMap;
    for (EaSLParser::EnumDefinitionContext *enumContext : schemaContext->enumDefinition()) {
        SchemaEnum *enumObj = new SchemaEnum(enumContext);
        enums.push_back(enumObj);
        enumMap[enumObj->_name] = enumObj;
    }
    _enums = new MaPLArrayMap<SchemaEnum *>(enums, enumMap);
    
    std::vector<SchemaClass *> classes;
    std::unordered_map<std::string, SchemaClass *> classMap;
    for (EaSLParser::ClassDefinitionContext *classContext : schemaContext->classDefinition()) {
        SchemaClass *classObj = new SchemaClass(classContext);
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

class EaSLErrorListener : public antlr4::BaseErrorListener {
public:
    unsigned int errorCount = 0;
    const std::filesystem::path *currentPath;
    virtual void syntaxError(antlr4::Recognizer *recognizer,
                             antlr4::Token * offendingSymbol,
                             size_t line,
                             size_t charPositionInLine,
                             const std::string &msg,
                             std::exception_ptr e) override {
        fprintf(stderr, "%s:%lu:%lu: error: %s.\n", currentPath->c_str(), line, charPositionInLine, msg.c_str());
        errorCount++;
    }
};

void validateSchemas(const std::vector<Schema *> &schemas, const std::unordered_map<std::string, Schema *> &schemasMap) {
    // TODO: Validate schema against itself.
}

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
        
        errListener.currentPath = &schemaPath;
        lexer.getErrorListenerDispatch().removeErrorListeners();
        parser.getErrorListenerDispatch().removeErrorListeners();
        lexer.addErrorListener(&errListener);
        parser.addErrorListener(&errListener);
        
        EaSLParser::SchemaContext *schemaContext = parser.schema();
        if (errListener.errorCount > 0) {
            // If we're inside this conditional, the error has already been logged via the default listener.
            exit(1);
        }
        
        schemasVector.push_back(new Schema(schemaContext));
    }
    
    std::unordered_map<std::string, Schema *> schemasMap;
    for (Schema *schema : schemasVector) {
        schemasMap[schema->_namespace] = schema;
    }
    
    validateSchemas(schemasVector, schemasMap);
    
    return new MaPLArrayMap<Schema *>(schemasVector, schemasMap);
}

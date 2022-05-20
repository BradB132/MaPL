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

SchemaEnum::SchemaEnum(EaSLParser::EnumDefinitionContext *enumContext) {
    // TODO: Init with context.
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

SchemaAttribute::SchemaAttribute(EaSLParser::AttributeContext *attributeContext) {
    // TODO: Init with context.
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
        default:
            return MaPLUninitialized();
    }
}

MaPLParameter SchemaAttribute::invokeSubscript(MaPLParameter index) {
    return MaPLUninitialized();
}

SchemaClass::SchemaClass(EaSLParser::ClassDefinitionContext *classContext) {
    // TODO: Init with context.
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

Schema::Schema(EaSLParser::SchemaContext *schemaContext) {
    // TODO: Init with context.
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
    virtual void syntaxError(antlr4::Recognizer *recognizer,
                             antlr4::Token * offendingSymbol,
                             size_t line, size_t charPositionInLine,
                             const std::string &msg,
                             std::exception_ptr e) override {
        errorCount++;
    }
};

std::vector<Schema *> schemasForPaths(const std::vector<std::filesystem::path> &schemaPaths) {
    std::vector<Schema *> parsedSchemas;
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
        
        lexer.addErrorListener(&errListener);
        parser.addErrorListener(&errListener);
        
        EaSLParser::SchemaContext *schemaContext = parser.schema();
        if (errListener.errorCount > 0) {
            // If we're inside this conditional, the error has already been logged via the default listener.
            exit(1);
        }
        
        parsedSchemas.push_back(new Schema(schemaContext));
    }
    return parsedSchemas;
}

void validateSchemas(const std::vector<Schema *> &schemas) {
    
}

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

std::vector<EaSLParser::SchemaContext *> schemasForPaths(const std::vector<std::filesystem::path> &schemaPaths) {
    std::vector<EaSLParser::SchemaContext *> parsedSchemas;
    EaSLErrorListener errListener;
    for (const std::filesystem::path &schemaPath : schemaPaths) {
        // Read the raw schema from the file system.
        std::ifstream inputStream(schemaPath);
        if (!inputStream) {
            printf("Unable to read schema file at path '%s'.\n", schemaPath.c_str());
            exit(1);
        }
        std::stringstream stringBuffer;
        stringBuffer << inputStream.rdbuf();
        std::string rawSchemaText = stringBuffer.str();
        
        // These objects will leak, but that's OK. All objects within the schema are deleted when
        // the parser is deleted, so the parser must be kept alive. This is a short-lived program,
        // so it's not worth the extra bookkeeping to track these objects and delete them later.
        antlr4::ANTLRInputStream *antlrInputStream = new antlr4::ANTLRInputStream(rawSchemaText);
        EaSLLexer *lexer = new EaSLLexer(antlrInputStream);
        antlr4::CommonTokenStream *tokenStream = new antlr4::CommonTokenStream(lexer);
        EaSLParser *parser = new EaSLParser(tokenStream);
        
        lexer->addErrorListener(&errListener);
        parser->addErrorListener(&errListener);
        
        EaSLParser::SchemaContext *schema = parser->schema();
        if (errListener.errorCount > 0) {
            // If we're inside this conditional, the error has already been logged via the default listener.
            exit(1);
        }
        
        parsedSchemas.push_back(schema);
    }
    return parsedSchemas;
}

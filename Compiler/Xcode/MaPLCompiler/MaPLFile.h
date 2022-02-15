//
//  MaPLFile.h
//  MaPLCompiler
//
//  Created by Brad Bambara on 3/27/21.
//

#ifndef MaPLFile_h
#define MaPLFile_h

#include <stdio.h>
#include <string>
#include <filesystem>
#include <unordered_map>
#include <vector>

#include "MaPLParser.h"
#include "BaseErrorListener.h"
#include "MaPLCompilerHelpers.h"

class MaPLLexer;
class MaPLFileCache;
class MaPLBuffer;

/**
 * Represents a single MaPL file in from the filesystem.
 */
class  MaPLFile : public antlr4::BaseErrorListener {
public:
    
    /**
     * @param normalizedFilePath The normalized filesystem path to the MaPL script file. Must be an absolute path.
     * @param fileCache The file cache that this object can use to fetch information about any dependent files.
     */
    MaPLFile(std::filesystem::path &normalizedFilePath, MaPLFileCache *fileCache);
    
    /**
     * @return The bytecode representation of the script in this file. This recusrively includes bytecode from dependent files. NULL if error.
     */
    MaPLBuffer *getBytecode();
    
    /**
     * @return The parse tree that represents the script in this file. Does not include nodes from dependent files. NULL if error.
     */
    MaPLParser::ProgramContext *getParseTree();
    
    /**
     * @return A list of files that were included in this file's script via #import statements. List is empty if error.
     */
    std::vector<MaPLFile *> getDependencies();
    
private:
    
    bool readRawScriptFromDisk();
    bool parseRawScript();
    void compileChildNodes(antlr4::ParserRuleContext *node, MaPLBuffer *currentBuffer);
    void compileNode(antlr4::ParserRuleContext *node, MaPLBuffer *currentBuffer);
    virtual void syntaxError(antlr4::Recognizer *recognizer,
                             antlr4::Token * offendingSymbol,
                             size_t line, size_t charPositionInLine,
                             const std::string &msg,
                             std::exception_ptr e) override;
    void logError(antlr4::Token *token, const std::string &msg);
    MaPLType dataTypeForExpression(MaPLParser::ExpressionContext *expression);
    MaPLPrimitiveType reconcileNumericTypes(MaPLPrimitiveType left,
                                            MaPLPrimitiveType right,
                                            antlr4::Token *errorToken);
    MaPLPrimitiveType typeReconciliationError(antlr4::Token *errorToken);
    MaPLType objectExpressionReturnType(MaPLParser::ObjectExpressionContext *expression,
                                        std::string invokedOnType);
    
    std::filesystem::path _normalizedFilePath;
    MaPLFileCache *_fileCache;
    std::string _rawScriptText;
    MaPLBuffer *_bytecode;
    std::vector<MaPLFile *> _dependencies;
    
    antlr4::ANTLRInputStream *_inputStream;
    MaPLLexer *_lexer;
    antlr4::CommonTokenStream *_tokenStream;
    MaPLParser *_parser;
    MaPLParser::ProgramContext *_program;
};

#endif /* MaPLFile_h */

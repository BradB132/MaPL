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

#include "MaPLCompiler.h"
#include "MaPLParser.h"
#include "BaseErrorListener.h"
#include "MaPLCompilerHelpers.h"
#include "MaPLVariableStack.h"

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
    MaPLFile(const std::filesystem::path &normalizedFilePath, MaPLFileCache *fileCache);
    
    /**
     * @return The bytecode representation of the script in this file. This recusrively includes bytecode from dependent files. Function symbols are not initially filled in. NULL if error.
     */
    MaPLBuffer *getBytecode();
    
    /**
     * @return The variable stack containing all top-level variables declared in this file. This recusrively includes variables from dependent files. NULL if error.
     */
    MaPLVariableStack *getVariableStack();
    
    /**
     * @return The parse tree that represents the script in this file. Does not include nodes from dependent files. NULL if error.
     */
    MaPLParser::ProgramContext *getParseTree();
    
    /**
     * @return A list of files that were included in this file's script via #import statements. List is empty if error.
     */
    std::vector<MaPLFile *> getDependencies();
    
    /**
     * @return The path in the filesystem that points to this file.
     */
    std::filesystem::path getNormalizedFilePath();
    
    /**
     * Logs an error that is then retrievable via @c getErrors().
     */
    void logError(antlr4::Token *token, const std::string &msg);
    
    /**
     * @return A list of all errors logged via @c logError().
     */
    std::vector<std::string> getErrors();
    
    /**
     * Mutator for this file's options. Options can change the way that the bytecode is compiled, but this
     * must be assigned before compilation is triggered by accessing the bytecode buffer or error list.
     */
    void setOptions(const MaPLCompileOptions &options);
    
private:
    
    bool parseRawScript();
    void compileChildNodes(antlr4::ParserRuleContext *node, const MaPLType &expectedType, MaPLBuffer *currentBuffer);
    void compileNode(antlr4::ParserRuleContext *node, const MaPLType &expectedType, MaPLBuffer *currentBuffer);
    MaPLType compileObjectExpression(MaPLParser::ObjectExpressionContext *expression,
                                     MaPLParser::ObjectExpressionContext *invokedOnExpression,
                                     MaPLBuffer *currentBuffer);
    void compileDebugVariableUpdate(const std::string &variableName,
                                    const MaPLVariable &variable,
                                    MaPLBuffer *currentBuffer);
    void compileDebugPopFromTopStackFrame(MaPLBuffer *currentBuffer);
    MaPLLiteral constantValueForExpression(MaPLParser::ExpressionContext *expression);
    MaPLType dataTypeForExpression(MaPLParser::ExpressionContext *expression);
    MaPLPrimitiveType reconcileTypes(MaPLPrimitiveType left,
                                     MaPLPrimitiveType right,
                                     antlr4::Token *errorToken);
    MaPLType reconcileExpressionTypes(MaPLParser::ExpressionContext *expression1,
                                      MaPLParser::ExpressionContext *expression2,
                                      antlr4::Token *errorToken);
    MaPLType objectExpressionReturnType(MaPLParser::ObjectExpressionContext *expression,
                                        const std::string &invokedOnType);
    
    virtual void syntaxError(antlr4::Recognizer *recognizer,
                             antlr4::Token * offendingSymbol,
                             size_t line, size_t charPositionInLine,
                             const std::string &msg,
                             std::exception_ptr e) override;
    MaPLPrimitiveType logTypeReconciliationError(MaPLPrimitiveType left,
                                                 MaPLPrimitiveType right,
                                                 antlr4::Token *errorToken);
    void logAmbiguousLiteralError(MaPLPrimitiveType type, antlr4::Token *token);
    void logMissingTypeError(antlr4::Token *errorToken, const std::string &typeName);
    void logNonNumericOperandsError(antlr4::Token *token);
    void logNotAssignableError(antlr4::Token *token);
    
    std::filesystem::path _normalizedFilePath;
    MaPLCompileOptions _options;
    MaPLFileCache *_fileCache;
    MaPLBuffer *_bytecode;
    MaPLVariableStack *_variableStack;
    std::vector<MaPLFile *> _dependencies;
    std::vector<std::string> _errors;
    
    antlr4::ANTLRInputStream *_inputStream;
    MaPLLexer *_lexer;
    antlr4::CommonTokenStream *_tokenStream;
    MaPLParser *_parser;
    MaPLParser::ProgramContext *_program;
};

#endif /* MaPLFile_h */

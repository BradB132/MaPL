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

class MaPLLexer;
class MaPLCompilerContext;
class MaPLBuffer;

typedef enum {
    MaPLExpectedReturnType_Int8,
    MaPLExpectedReturnType_Int16,
    MaPLExpectedReturnType_Int32,
    MaPLExpectedReturnType_Int64,
    MaPLExpectedReturnType_UInt8,
    MaPLExpectedReturnType_UInt16,
    MaPLExpectedReturnType_UInt32,
    MaPLExpectedReturnType_UInt64,
    MaPLExpectedReturnType_Float32,
    MaPLExpectedReturnType_Float64,
    MaPLExpectedReturnType_String,
    MaPLExpectedReturnType_Boolean,
    MaPLExpectedReturnType_Pointer,
    
    // Numeric literals can be intepreted differently depending on surrounding context.
    MaPLExpectedReturnType_SignedInt_AmbiguousSize,
    MaPLExpectedReturnType_Int_AmbiguousSizeAndSign,
    MaPLExpectedReturnType_Float_AmbiguousSize,
    
    MaPLExpectedReturnType_InvalidType,
} MaPLExpectedReturnType;

typedef struct {
    // Specifies the type of primitive value that will be returned from a call to the host program.
    MaPLExpectedReturnType type;
    // In the case that `type` is a pointer, what is the name of the `#type` that it represents.
    std::string pointerType;
} MaPLFunctionReturnType;

/**
 * Represents a single MaPL file in from the filesystem.
 */
class  MaPLFile : public antlr4::BaseErrorListener {
public:
    
    /**
     * @param normalizedFilePath The normalized filesystem path to the MaPL script file. Must be an absolute path.
     * @param parentContext The compiling context that this file exists within.
     */
    MaPLFile(std::filesystem::path &normalizedFilePath, MaPLCompilerContext *parentContext);
    
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
    MaPLFunctionReturnType dataTypeForExpression(MaPLParser::ExpressionContext *expression);
    MaPLExpectedReturnType reconcileNumericTypes(MaPLExpectedReturnType left,
                                                 MaPLExpectedReturnType right,
                                                 antlr4::Token *errorToken);
    MaPLExpectedReturnType typeReconciliationError(antlr4::Token *errorToken);
    MaPLFunctionReturnType objectExpressionReturnType(MaPLParser::ObjectExpressionContext *expression,
                                                      std::string invokedOnType);
    
    std::filesystem::path _normalizedFilePath;
    MaPLCompilerContext *_parentContext;
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

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
     * @return The bytecode representation of the script in this file. This recusrively includes bytecode from dependent files.
     */
    MaPLBuffer *getBytecode();
    
    /**
     * @return The abstract syntax tree that represents the script in this file. Does not include nodes from dependent files.
     */
    MaPLParser::ProgramContext *getAST();
    
    /**
     * @return A list of files that were included in this file's script via #import statements.
     */
    std::vector<MaPLFile *> getDependencies();
    
private:
    
    bool readRawScriptFromDisk();
    void parseRawScript();
    virtual void syntaxError(antlr4::Recognizer *recognizer,
                             antlr4::Token * offendingSymbol,
                             size_t line, size_t charPositionInLine,
                             const std::string &msg,
                             std::exception_ptr e) override;
    
    std::filesystem::path _normalizedFilePath;
    MaPLCompilerContext *_parentContext;
    std::string _rawScriptText;
    antlr4::ANTLRInputStream *_inputStream;
    MaPLLexer *_lexer;
    antlr4::CommonTokenStream *_tokenStream;
    MaPLParser *_parser;
    MaPLParser::ProgramContext *_program;
    MaPLBuffer *_bytecode;
    std::vector<MaPLFile *> _dependencies;
};

#endif /* MaPLFile_h */

//
//  MaPLFile.cpp
//  MaPLCompiler
//
//  Created by Brad Bambara on 3/27/21.
//

#include "MaPLFile.h"
#include <fstream>
#include <sstream>

#include "antlr4-runtime.h"
#include "MaPLLexer.h"
#include "MaPLCompilerContext.h"
#include "MaPLBuffer.h"

MaPLFile::MaPLFile(std::filesystem::path &normalizedFilePath, MaPLCompilerContext *parentContext) :
    _normalizedFilePath(normalizedFilePath),
    _parentContext(parentContext),
    _rawScriptText(""),
    _inputStream(NULL),
    _lexer(NULL),
    _tokenStream(NULL),
    _parser(NULL),
    _program(NULL),
    _bytecode(NULL)
{
}

bool MaPLFile::readRawScriptFromDisk() {
    if (!_rawScriptText.empty()) {
        return true;
    }
    std::ifstream inputStream(_normalizedFilePath);
    if (!inputStream) {
        return false;
    }
    std::stringstream stringBuffer;
    stringBuffer << inputStream.rdbuf();
    _rawScriptText = stringBuffer.str();
    return true;
}

void MaPLFile::parseRawScript() {
    if (_program) {
        return;
    }
    if (!readRawScriptFromDisk()) {
        // TODO: log an error here.
        return;
    }
    
    _inputStream = new antlr4::ANTLRInputStream(_rawScriptText);
    _lexer = new MaPLLexer(_inputStream);
    _tokenStream = new antlr4::CommonTokenStream(_lexer);
    _parser = new MaPLParser(_tokenStream);
    _program = _parser->program();
    
    // Iterate over any API imports and add the corresponding files to the parent context.
    for(MaPLParser::StatementContext *statement : _program->statement()) {
        MaPLParser::ApiImportContext *apiImport = statement->apiImport();
        if(!apiImport) {
            continue;
        }
        std::string importString = apiImport->STRING()->getText();
        
        // The text will always contain the string quotes, substring call removes them.
        std::filesystem::path importPath = importString.substr(1, importString.length()-2);
        
        // Generate a normalized path from the path specified in the import statement.
        if (importPath.is_relative()) {
            importPath = _normalizedFilePath.parent_path() / importPath;
        }
        importPath = importPath.lexically_normal();
        
        MaPLFile *dependencyFile = _parentContext->fileForAbsolutePath(importPath);
        if (!dependencyFile) {
            // TODO: log an error here.
            continue;
        }
        _dependencies.push_back(dependencyFile);
    }
}

MaPLParser::ProgramContext *MaPLFile::getAST() {
    parseRawScript();
    return _program;
}

std::vector<MaPLFile *> MaPLFile::getDependencies() {
    parseRawScript();
    return _dependencies;
}

MaPLBuffer *MaPLFile::getBytecode() {
    if (_bytecode) {
        return _bytecode;
    }
    
    parseRawScript();
    _bytecode = new MaPLBuffer(10);
    
    // Concatenate all preceding bytecode from dependencies.
    for(MaPLFile *file : _dependencies) {
        MaPLBuffer *dependencyBytecode = file->getBytecode();
        _bytecode->appendBytes(dependencyBytecode->getBytes(), dependencyBytecode->getByteCount());
    }
    
    // TODO: compile this file and concat bytes.
    
    return _bytecode;
}

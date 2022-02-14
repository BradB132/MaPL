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
#include "MaPLBytecodeConstants.h"

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

bool MaPLFile::parseRawScript() {
    if (_program) {
        return true;
    }
    if (!readRawScriptFromDisk()) {
        logError(NULL, "Unable to read script file.");
        return false;
    }
    
    _inputStream = new antlr4::ANTLRInputStream(_rawScriptText);
    _lexer = new MaPLLexer(_inputStream);
    _tokenStream = new antlr4::CommonTokenStream(_lexer);
    _parser = new MaPLParser(_tokenStream);
    
    _parser->getErrorListenerDispatch().removeErrorListeners();
    _parser->addErrorListener(this);
    
    _program = _parser->program();
    
    // Iterate over any API imports and add the corresponding files to the parent context.
    for(MaPLParser::StatementContext *statement : _program->statement()) {
        MaPLParser::ApiImportContext *apiImport = statement->apiImport();
        if(!apiImport) {
            continue;
        }
        std::string importString = apiImport->LITERAL_STRING()->getText();
        
        // The text will always contain the string quotes, substring call removes them.
        std::filesystem::path importPath = importString.substr(1, importString.length()-2);
        
        // Generate a normalized path from the path specified in the import statement.
        if (importPath.is_relative()) {
            importPath = _normalizedFilePath.parent_path() / importPath;
        }
        importPath = importPath.lexically_normal();
        
        MaPLFile *dependencyFile = _parentContext->fileForAbsolutePath(importPath);
        if (!dependencyFile) {
            logError(apiImport->start, "Unable to resolve path for import statement: "+importString);
            continue;
        }
        _dependencies.push_back(dependencyFile);
    }
    return true;
}

MaPLParser::ProgramContext *MaPLFile::getParseTree() {
    if (!parseRawScript()) {
        return NULL;
    }
    return _program;
}

std::vector<MaPLFile *> MaPLFile::getDependencies() {
    parseRawScript();
    return _dependencies;
}

MaPLParser::ApiPropertyContext *findProperty(MaPLFile *file, std::string type, std::string name) {
    MaPLParser::ProgramContext *program = file->getParseTree();
    if (!program) {
        return NULL;
    }
    
    // Search all API property declarations in this file.
    bool isGlobal = type.empty();
    for (MaPLParser::StatementContext *statement : program->statement()) {
        MaPLParser::ApiDeclarationContext *apiDeclaration = statement->apiDeclaration();
        if (!apiDeclaration) { continue; }
        switch (apiDeclaration->keyToken->getType()) {
            case MaPLParser::API_GLOBAL: {
                if (!isGlobal) { continue; }
                MaPLParser::ApiPropertyContext *property = apiDeclaration->apiProperty(0);
                if (!property) { continue; }
                MaPLParser::IdentifierContext *identifier = property->identifier();
                if (identifier && identifier->getText() == name) {
                    return property;
                }
            }
                break;
            case MaPLParser::API_TYPE: {
                if (isGlobal) { continue; }
                MaPLParser::IdentifierContext *identifier = apiDeclaration->identifier();
                if (!identifier || identifier->getText() != type) {
                    continue;
                }
                for (MaPLParser::ApiPropertyContext *property : apiDeclaration->apiProperty()) {
                    MaPLParser::IdentifierContext *identifier = property->identifier();
                    if (identifier && identifier->getText() == name) {
                        return property;
                    }
                }
                MaPLParser::ApiInheritanceContext *inheritance = apiDeclaration->apiInheritance();
                if (inheritance) {
                    for (MaPLParser::IdentifierContext *identifier : inheritance->identifier()) {
                        MaPLParser::ApiPropertyContext *foundProperty = findProperty(file, identifier->getText(), name);
                        if (foundProperty) {
                            return foundProperty;
                        }
                    }
                }
            }
                break;
            default:
                break;
        }
    }
    
    // If no matching property was found in this file, recurse through all dependent files.
    for (MaPLFile *dependency : file->getDependencies()) {
        MaPLParser::ApiPropertyContext *foundProperty = findProperty(dependency, type, name);
        if (foundProperty) {
            return foundProperty;
        }
    }
    
    return NULL;
}

MaPLBuffer *MaPLFile::getBytecode() {
    if (_bytecode) {
        return _bytecode;
    }
    if (!parseRawScript()) {
        return NULL;
    }
    _bytecode = new MaPLBuffer(10);
    
    // Concatenate all preceding bytecode from dependencies.
    for(MaPLFile *file : _dependencies) {
        MaPLBuffer *dependencyBytecode = file->getBytecode();
        if (dependencyBytecode) {
            _bytecode->appendBytes(dependencyBytecode->getBytes(), dependencyBytecode->getByteCount());
        }
    }
    
    // Compile the bytecode from this file.
    compileChildNodes(_program, _bytecode);
    
    return _bytecode;
}

void MaPLFile::syntaxError(antlr4::Recognizer *recognizer,
                                    antlr4::Token *offendingSymbol,
                                    size_t line,
                                    size_t charPositionInLine,
                                    const std::string &msg,
                                    std::exception_ptr e) {
    // This method is overriding `ANTLRErrorListener`. Forward this call to the more generally usable function.
    logError(offendingSymbol, msg);
}

void MaPLFile::logError(antlr4::Token *token,
                        const std::string &msg) {
    if (token) {
        printf("%s %ld:%ld: %s\n", _normalizedFilePath.c_str(), token->getLine(), token->getCharPositionInLine(), msg.c_str());
    } else {
        printf("%s: %s\n", _normalizedFilePath.c_str(), msg.c_str());
    }
}

void MaPLFile::compileChildNodes(antlr4::ParserRuleContext *node, MaPLBuffer *currentBuffer) {
    for (antlr4::tree::ParseTree *child : node->children) {
        antlr4::ParserRuleContext *ruleContext = dynamic_cast<antlr4::ParserRuleContext *>(child);
        if (ruleContext) {
            compileNode(ruleContext, currentBuffer);
        }
    }
}

void MaPLFile::compileNode(antlr4::ParserRuleContext *node, MaPLBuffer *currentBuffer) {
    switch (node->getRuleIndex()) {
        case MaPLParser::RuleStatement: {
            MaPLParser::StatementContext *statement = (MaPLParser::StatementContext *)node;
            antlr4::tree::TerminalNode *terminalNode = statement->METADATA();
            if (terminalNode) {
                std::string rawMetadata = terminalNode->getText();
                std::string trimmedMetadata = rawMetadata.substr(2, rawMetadata.length()-4);
                currentBuffer->appendByte(MAPL_BYTE_METADATA);
                const char *metadataChars = trimmedMetadata.c_str();
                currentBuffer->appendBytes(metadataChars, strlen(metadataChars)+1);
                break;
            }
            compileChildNodes(node, currentBuffer);
        }
            break;
        case MaPLParser::RuleImperativeStatement: {
            MaPLParser::ImperativeStatementContext *statement = (MaPLParser::ImperativeStatementContext *)node;
            antlr4::tree::TerminalNode *terminalNode = statement->BREAK();
            if (terminalNode) {
                // TODO: implement this.
                break;
            }
            terminalNode = statement->CONTINUE();
            if (terminalNode) {
                // TODO: implement this.
                break;
            }
            terminalNode = statement->EXIT();
            if (terminalNode) {
                currentBuffer->appendByte(MAPL_BYTE_PROGRAM_EXIT);
                break;
            }
            compileChildNodes(node, currentBuffer);
        }
            break;
        case MaPLParser::RuleAssignStatement: {
            MaPLParser::AssignStatementContext *assignment = (MaPLParser::AssignStatementContext *)node;
            
            // Object expression is on the left side (receiving the assignment). This can be any object expression except for a function call.
            MaPLParser::ObjectExpressionContext *terminalObjectExpression = assignment->objectExpression();
            while (terminalObjectExpression->OBJECT_TO_MEMBER()) {
                terminalObjectExpression = terminalObjectExpression->objectExpression(1);
            }
            if (terminalObjectExpression->PAREN_OPEN()) {
                logError(terminalObjectExpression->start, "Functions are read only.");
                break;
            }
            // TODO: assigning to object expressions will be relatively complex, revisit this last.
        }
            break;
        case MaPLParser::RuleUnaryStatement: {
            MaPLParser::UnaryStatementContext *statement = (MaPLParser::UnaryStatementContext *)node;
            MaPLParser::ObjectExpressionContext *objectExpression = statement->objectExpression();
            // TODO: assigning to object expressions will be relatively complex, revisit this last.
        }
            break;
        case MaPLParser::RuleExpression: {
            MaPLParser::ExpressionContext *expression = (MaPLParser::ExpressionContext *)node;
            antlr4::tree::TerminalNode *terminalNode = expression->PAREN_OPEN();
            if (terminalNode) {
                // Expressions with parens are either a typecast or nested parenthesized expression.
                MaPLParser::TypeContext *type = expression->type();
                if (type) {
                    switch (type->start->getType()) {
                        case MaPLParser::DECL_INT8:
                            // TODO: add cases for all primitives, default case handles objects.
                            break;
                        default:
                            break;
                    }
                } else {
                    // Nested expressions can be handled by compiling child nodes.
                    compileChildNodes(node, currentBuffer);
                }
                break;
            }
            // TODO: implement other terminal nodes.
            
            // Object expressions can be handled by compiling child nodes.
            compileChildNodes(node, currentBuffer);
        }
            break;
        case MaPLParser::RuleVariableDeclaration: {
            MaPLParser::VariableDeclarationContext *declaration = (MaPLParser::VariableDeclarationContext *)node;
            compileChildNodes(node, currentBuffer);
        }
            break;
        default:
            break;
    }
}

bool isAmbiguousNumericType(MaPLExpectedReturnType returnType) {
    switch (returnType) {
        case MaPLExpectedReturnType_Float_AmbiguousSize:
        case MaPLExpectedReturnType_SignedInt_AmbiguousSize:
        case MaPLExpectedReturnType_Int_AmbiguousSizeAndSign:
            return true;
        default:
            return false;
    }
}

bool isFloat(MaPLExpectedReturnType returnType) {
    switch (returnType) {
        case MaPLExpectedReturnType_Float32:
        case MaPLExpectedReturnType_Float64:
            return true;
        default:
            return false;
    }
}

bool isSignedInt(MaPLExpectedReturnType returnType) {
    switch (returnType) {
        case MaPLExpectedReturnType_Int8:
        case MaPLExpectedReturnType_Int16:
        case MaPLExpectedReturnType_Int32:
        case MaPLExpectedReturnType_Int64:
            return true;
        default:
            return false;
    }
}

bool isUnsignedInt(MaPLExpectedReturnType returnType) {
    switch (returnType) {
        case MaPLExpectedReturnType_UInt8:
        case MaPLExpectedReturnType_UInt16:
        case MaPLExpectedReturnType_UInt32:
        case MaPLExpectedReturnType_UInt64:
            return true;
        default:
            return false;
    }
}

bool isNumeric(MaPLExpectedReturnType returnType) {
    return isFloat(returnType) ||
           isSignedInt(returnType) ||
           isUnsignedInt(returnType) ||
           isAmbiguousNumericType(returnType);
}

MaPLExpectedReturnType MaPLFile::typeReconciliationError(antlr4::Token *errorToken) {
    logError(errorToken, "Operands must have matching types.");
    return MaPLExpectedReturnType_InvalidType;
}

MaPLExpectedReturnType MaPLFile::reconcileNumericTypes(MaPLExpectedReturnType left,
                                                       MaPLExpectedReturnType right,
                                                       antlr4::Token *errorToken) {
    if (left == right) {
        return left;
    }
    // This function takes a pair of types, which potentially contain ambiguity due
    // to the values being specified as literals instead of variable declarations,
    // and resolves them down to the most specific type information that can be inferred.
    // For example, the expression "3+4.5" should be interpreted as a pair of floating
    // point numbers, even though the number on the left looks like an integer literal.
    if (isAmbiguousNumericType(left) || isAmbiguousNumericType(right)) {
        // Any ambiguous number can be stored in a float.
        if (isFloat(left)) { return left; }
        if (isFloat(right)) { return right; }
        
        // Signed integers can store any ambiguous non-float numbers.
        if (isSignedInt(left)) {
            if (right == MaPLExpectedReturnType_Float_AmbiguousSize) {
                return typeReconciliationError(errorToken);
            }
            return left;
        }
        if (isSignedInt(right)) {
            if (left == MaPLExpectedReturnType_Float_AmbiguousSize) {
                return typeReconciliationError(errorToken);
            }
            return right;
        }
        
        // Unsigned integers can store only ambiguously-sized, unsigned numbers.
        if (isUnsignedInt(left)) {
            if (right == MaPLExpectedReturnType_Int_AmbiguousSizeAndSign) {
                return left;
            }
            return typeReconciliationError(errorToken);
        }
        if (isUnsignedInt(right)) {
            if (left == MaPLExpectedReturnType_Int_AmbiguousSizeAndSign) {
                return right;
            }
            return typeReconciliationError(errorToken);
        }
        
        // All pairings containing a specific type have been handled above. All remaining are some combination of ambiguous types.
        if (left == MaPLExpectedReturnType_Float_AmbiguousSize || right == MaPLExpectedReturnType_Float_AmbiguousSize) {
            // All ambigous integer types can convert to float.
            return MaPLExpectedReturnType_Float_AmbiguousSize;
        }
        // Only permutation remaining is SignedInt_AmbiguousSize and Int_AmbiguousSizeAndSign.
        return MaPLExpectedReturnType_SignedInt_AmbiguousSize;
    }
    return typeReconciliationError(errorToken);
}

MaPLFunctionReturnType MaPLFile::dataTypeForExpression(MaPLParser::ExpressionContext *expression) {
    switch (expression->keyToken->getType()) {
        case MaPLParser::PAREN_OPEN: {
            // This is a typecast, return the type that is specified in the cast.
            MaPLParser::TypeContext *type = expression->type();
            if (type->identifier()) {
                return { MaPLExpectedReturnType_Pointer, type->identifier()->getText() };
            }
            switch (type->start->getType()) {
                case MaPLParser::DECL_INT8: return { MaPLExpectedReturnType_Int8, "" };
                case MaPLParser::DECL_INT16: return { MaPLExpectedReturnType_Int16, "" };
                case MaPLParser::DECL_INT32: return { MaPLExpectedReturnType_Int32, "" };
                case MaPLParser::DECL_INT64: return { MaPLExpectedReturnType_Int64, "" };
                case MaPLParser::DECL_UINT8: return { MaPLExpectedReturnType_UInt8, "" };
                case MaPLParser::DECL_UINT16: return { MaPLExpectedReturnType_UInt16, "" };
                case MaPLParser::DECL_UINT32: return { MaPLExpectedReturnType_UInt32, "" };
                case MaPLParser::DECL_UINT64: return { MaPLExpectedReturnType_UInt64, "" };
                case MaPLParser::DECL_FLOAT32: return { MaPLExpectedReturnType_Float32, "" };
                case MaPLParser::DECL_FLOAT64: return { MaPLExpectedReturnType_Float64, "" };
                case MaPLParser::DECL_BOOL: return { MaPLExpectedReturnType_Boolean, "" };
                case MaPLParser::DECL_STRING: return { MaPLExpectedReturnType_String, "" };
            }
        }
            break;
        case MaPLParser::PAREN_CLOSE:
            // This is a nested expression, return type of the child expression.
            return dataTypeForExpression(expression->expression(0));
        case MaPLParser::LOGICAL_AND: // Intentional fallthrough.
        case MaPLParser::LOGICAL_OR: // Intentional fallthrough.
        case MaPLParser::LOGICAL_EQUALITY: // Intentional fallthrough.
        case MaPLParser::LOGICAL_INEQUALITY: // Intentional fallthrough.
        case MaPLParser::LESS_THAN: // Intentional fallthrough.
        case MaPLParser::LESS_THAN_EQUAL: // Intentional fallthrough.
        case MaPLParser::GREATER_THAN: // Intentional fallthrough.
        case MaPLParser::GREATER_THAN_EQUAL: // Intentional fallthrough.
        case MaPLParser::LOGICAL_NEGATION: // Intentional fallthrough.
        case MaPLParser::LITERAL_TRUE: // Intentional fallthrough.
        case MaPLParser::LITERAL_FALSE:
            return { MaPLExpectedReturnType_Boolean, "" };
        case MaPLParser::SUBTRACT: {
            std::vector<MaPLParser::ExpressionContext *> childExpressions = expression->expression();
            if (childExpressions.size() == 1) {
                // There's only one operand, so this is numeric negation instead of subtraction.
                MaPLFunctionReturnType type1 = dataTypeForExpression(childExpressions[0]);
                if (!isNumeric(type1.type)) {
                    logError(expression->keyToken, "Numeric negation can only be applied to numeric data types.");
                    return { MaPLExpectedReturnType_InvalidType, "" };
                }
                if (type1.type == MaPLExpectedReturnType_Int_AmbiguousSizeAndSign) {
                    // If the datatype is ambiguous, but it's being negated, it must be a signed int.
                    return { MaPLExpectedReturnType_SignedInt_AmbiguousSize, "" };
                }
                return type1;
            }
        }
            // Intentional fallthrough handles the binary subtraction operator.
        case MaPLParser::MOD: // Intentional fallthrough.
        case MaPLParser::MULTIPLY: // Intentional fallthrough.
        case MaPLParser::DIVIDE: {
            std::vector<MaPLParser::ExpressionContext *> childExpressions = expression->expression();
            MaPLFunctionReturnType type1 = dataTypeForExpression(childExpressions[0]);
            MaPLFunctionReturnType type2 = dataTypeForExpression(childExpressions[1]);
            if (isNumeric(type1.type) && isNumeric(type2.type)) {
                // Types are not the same, but might be reconcilable if there's still ambiguity.
                return { reconcileNumericTypes(type1.type, type2.type, expression->keyToken), "" };
            }
            logError(expression->keyToken, "Both operands must be numeric.");
            return { MaPLExpectedReturnType_InvalidType, "" };
        }
        case MaPLParser::ADD: {
            std::vector<MaPLParser::ExpressionContext *> childExpressions = expression->expression();
            MaPLFunctionReturnType type1 = dataTypeForExpression(childExpressions[0]);
            MaPLFunctionReturnType type2 = dataTypeForExpression(childExpressions[1]);
            // Plus operator could be numeric add or string concatenation.
            if (type1.type == MaPLExpectedReturnType_String &&
                type2.type == MaPLExpectedReturnType_String &&
                expression->keyToken->getType() == MaPLParser::ADD) {
                return { MaPLExpectedReturnType_String, "" };
            } else if (isNumeric(type1.type) && isNumeric(type2.type)) {
                // Types are not the same, but might be reconcilable if there's still ambiguity.
                return { reconcileNumericTypes(type1.type, type2.type, expression->keyToken), "" };
            }
            logError(expression->keyToken, "Both operands must be either string (concatenation) or numeric (addition).");
            return { MaPLExpectedReturnType_InvalidType, "" };
        }
        case MaPLParser::LITERAL_INT: return { MaPLExpectedReturnType_Int_AmbiguousSizeAndSign, "" };
        case MaPLParser::LITERAL_FLOAT: return { MaPLExpectedReturnType_Float_AmbiguousSize, "" };
        case MaPLParser::LITERAL_STRING: return { MaPLExpectedReturnType_String, "" };
        case MaPLParser::LITERAL_NULL: return { MaPLExpectedReturnType_Pointer, "" };
            
    }
    MaPLParser::ObjectExpressionContext* objectExpression = expression->objectExpression();
    if (objectExpression) {
        return objectExpressionReturnType(objectExpression, "");
    }
    logError(expression->keyToken, "Type error.");
    return { MaPLExpectedReturnType_InvalidType, "" };
}

MaPLFunctionReturnType MaPLFile::objectExpressionReturnType(MaPLParser::ObjectExpressionContext *expression, std::string invokedOnType) {
    antlr4::Token *keyToken = expression->keyToken;
    if (keyToken) {
        switch (keyToken->getType()) {
            case MaPLParser::OBJECT_TO_MEMBER: {
                // Find the pointer type from the prefix and supply that type information to the suffix.
                // For example, in the expression "obj.func()", the prefix is "obj" and suffix is "func()".
                // To understand the return type of "func()", we have to understand it's being invoked on "obj".
                std::vector<MaPLParser::ObjectExpressionContext *> childExpressions = expression->objectExpression();
                MaPLFunctionReturnType prefixType = objectExpressionReturnType(childExpressions[0], invokedOnType);
                if (prefixType.type != MaPLExpectedReturnType_Pointer) {
                    logError(keyToken, "The '.' operator can only be used on pointers.");
                    return { MaPLExpectedReturnType_InvalidType, "" };
                }
                return objectExpressionReturnType(childExpressions[1], prefixType.pointerType);
            }
            case MaPLParser::SUBSCRIPT_OPEN: {
                // Subscript invocation.
                // TODO: This is a subscript on a type.
            }
            case MaPLParser::PAREN_OPEN: {
                // Function invocation.
                if (invokedOnType.empty()) {
                    // TODO: This is a global function call.
                } else {
                    // TODO: This is a function call on a type.
                }
            }
            default:
                break;
        }
    } else {
        MaPLParser::IdentifierContext *identifier = expression->identifier();
        if (identifier) {
            if (invokedOnType.empty()) {
                // TODO: This is either a variable or global property.
            } else {
                // TODO: This is a property on a type.
            }
        }
    }
    logError(keyToken, "Error determining the type of this expression.");
    return { MaPLExpectedReturnType_InvalidType, "" };
}

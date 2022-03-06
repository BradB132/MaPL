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
#include "MaPLFileCache.h"
#include "MaPLBuffer.h"
#include "MaPLBytecodeConstants.h"

MaPLFile::MaPLFile(std::filesystem::path &normalizedFilePath, MaPLFileCache *fileCache) :
    _normalizedFilePath(normalizedFilePath),
    _fileCache(fileCache),
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
        logError(this, NULL, "Unable to read script file.");
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
        
        MaPLFile *dependencyFile = _fileCache->fileForAbsolutePath(importPath);
        if (!dependencyFile) {
            logError(this, apiImport->start, "Unable to resolve path for import statement: "+importString);
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

std::filesystem::path MaPLFile::getNormalizedFilePath() {
    return _normalizedFilePath;
}

void MaPLFile::syntaxError(antlr4::Recognizer *recognizer,
                                    antlr4::Token *offendingSymbol,
                                    size_t line,
                                    size_t charPositionInLine,
                                    const std::string &msg,
                                    std::exception_ptr e) {
    // This method is overriding ANTLRErrorListener. Forward this call to the more generally usable function.
    logError(this, offendingSymbol, msg);
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
        // TODO: For APIs, we still need to check for duplicate types.
        // TODO: For APIs, we still need to confirm that any types which are referred to are actually declared somewhere.
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
                logError(this, terminalObjectExpression->start, "Functions are read only.");
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

MaPLPrimitiveType MaPLFile::typeReconciliationError(antlr4::Token *errorToken) {
    logError(this, errorToken, "Type mismatch.");
    return MaPLPrimitiveType_InvalidType;
}

MaPLPrimitiveType MaPLFile::reconcileTypes(MaPLPrimitiveType left,
                                           MaPLPrimitiveType right,
                                           antlr4::Token *errorToken) {
    if (left == right) {
        return left;
    }
    // Numeric types are the only ones with ambiguity. Anything else is definitely in error.
    if (!isNumeric(left) || !isNumeric(right)) {
        return typeReconciliationError(errorToken);
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
            if (right == MaPLPrimitiveType_Float_AmbiguousSize) {
                return typeReconciliationError(errorToken);
            }
            return left;
        }
        if (isSignedInt(right)) {
            if (left == MaPLPrimitiveType_Float_AmbiguousSize) {
                return typeReconciliationError(errorToken);
            }
            return right;
        }
        
        // Unsigned integers can store only ambiguously-sized, unsigned numbers.
        if (isUnsignedInt(left)) {
            if (right == MaPLPrimitiveType_Int_AmbiguousSizeAndSign) {
                return left;
            }
            return typeReconciliationError(errorToken);
        }
        if (isUnsignedInt(right)) {
            if (left == MaPLPrimitiveType_Int_AmbiguousSizeAndSign) {
                return right;
            }
            return typeReconciliationError(errorToken);
        }
        
        // All pairings containing a specific type have been handled above. All remaining are some combination of ambiguous types.
        if (left == MaPLPrimitiveType_Float_AmbiguousSize || right == MaPLPrimitiveType_Float_AmbiguousSize) {
            // All ambigous integer types can convert to float.
            return MaPLPrimitiveType_Float_AmbiguousSize;
        }
        // Only permutation remaining is SignedInt_AmbiguousSize and Int_AmbiguousSizeAndSign.
        return MaPLPrimitiveType_SignedInt_AmbiguousSize;
    }
    return typeReconciliationError(errorToken);
}

MaPLType MaPLFile::reconcileExpressionTypes(MaPLParser::ExpressionContext *expression1,
                                            MaPLParser::ExpressionContext *expression2,
                                            antlr4::Token *errorToken) {
    MaPLType type1 = dataTypeForExpression(expression1);
    MaPLType type2 = dataTypeForExpression(expression2);
    MaPLPrimitiveType reconciledPrimitive = reconcileTypes(type1.type, type2.type, errorToken);
    if (reconciledPrimitive == MaPLPrimitiveType_Pointer && type1.pointerType != type2.pointerType) {
        // The pointer types don't match. Generate an error message that gives some context.
        std::vector<std::string> possibleClasses = mutualSuperclasses(this, type1.pointerType, type2.pointerType);
        std::string errorSuffix;
        size_t possibleClassCount = possibleClasses.size();
        if (possibleClassCount == 1) {
            // In the case where there's only one possible choice, make the inference.
            return { MaPLPrimitiveType_Pointer, possibleClasses[0] };
        }
        if (possibleClassCount == 0) {
            errorSuffix = "There are no common ancestors for the types '"+type1.pointerType+"' and '"+type2.pointerType+"'.";
        } else {
            errorSuffix = "The possible ancestor types for '"+type1.pointerType+"' and '"+type2.pointerType+"' are ";
            for (size_t i = 0; i < possibleClassCount; i++) {
                std::string possibleClass = possibleClasses[i];
                errorSuffix += "'"+possibleClass+"'";
                if (i == (possibleClassCount-1)) {
                    errorSuffix += ".";
                } else {
                    errorSuffix += ", ";
                }
            }
        }
        logError(this, errorToken, "The return type of this expression is ambiguous and cannot be determined. Both expressions must have a matching type. "+errorSuffix);
        return { MaPLPrimitiveType_InvalidType };
    }
    return { reconciledPrimitive, type1.pointerType };
}

MaPLType MaPLFile::dataTypeForExpression(MaPLParser::ExpressionContext *expression) {
    if (expression->keyToken) {
        switch (expression->keyToken->getType()) {
            case MaPLParser::PAREN_OPEN: {
                // This is a typecast, return the type that is specified in the cast.
                MaPLParser::TypeContext *typeContext = expression->type();
                MaPLType type = typeForTypeContext(typeContext);
                if (type.type == MaPLPrimitiveType_InvalidType) {
                    // Break down to the bottom of this function to log a generic error.
                    break;
                }
                return type;
            }
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
                return { MaPLPrimitiveType_Boolean };
            case MaPLParser::SUBTRACT: {
                std::vector<MaPLParser::ExpressionContext *> childExpressions = expression->expression();
                if (childExpressions.size() == 1) {
                    // There's only one operand, so this is numeric negation instead of subtraction.
                    MaPLType type = dataTypeForExpression(childExpressions[0]);
                    if (isUnsignedInt(type.type)) {
                        logError(this, expression->keyToken, "Unsigned integers cannot be negated.");
                        return { MaPLPrimitiveType_InvalidType };
                    }
                    if (type.type == MaPLPrimitiveType_Int_AmbiguousSizeAndSign) {
                        // If the datatype is ambiguous, but it's being negated, it must be a signed int.
                        return { MaPLPrimitiveType_SignedInt_AmbiguousSize };
                    }
                    if (isNumeric(type.type)) {
                        return type;
                    }
                    logError(this, expression->keyToken, "Numeric negation can only be applied to numeric data types.");
                    return { MaPLPrimitiveType_InvalidType };
                }
            }
                // Intentional fallthrough handles the binary subtraction operator.
            case MaPLParser::MOD: // Intentional fallthrough.
            case MaPLParser::MULTIPLY: // Intentional fallthrough.
            case MaPLParser::DIVIDE: {
                std::vector<MaPLParser::ExpressionContext *> childExpressions = expression->expression();
                MaPLType type1 = dataTypeForExpression(childExpressions[0]);
                MaPLType type2 = dataTypeForExpression(childExpressions[1]);
                if (isNumeric(type1.type) && isNumeric(type2.type)) {
                    return { reconcileTypes(type1.type, type2.type, expression->keyToken) };
                }
                logError(this, expression->keyToken, "Both operands must be numeric.");
                return { MaPLPrimitiveType_InvalidType };
            }
            case MaPLParser::ADD: {
                std::vector<MaPLParser::ExpressionContext *> childExpressions = expression->expression();
                MaPLType type1 = dataTypeForExpression(childExpressions[0]);
                MaPLType type2 = dataTypeForExpression(childExpressions[1]);
                // Plus operator could be numeric add or string concatenation.
                if (type1.type == MaPLPrimitiveType_String &&
                    type2.type == MaPLPrimitiveType_String &&
                    expression->keyToken->getType() == MaPLParser::ADD) {
                    return { MaPLPrimitiveType_String };
                } else if (isNumeric(type1.type) && isNumeric(type2.type)) {
                    return { reconcileTypes(type1.type, type2.type, expression->keyToken) };
                }
                logError(this, expression->keyToken, "Both operands must be either string (concatenation) or numeric (addition).");
                return { MaPLPrimitiveType_InvalidType };
            }
            case MaPLParser::BITWISE_AND: // Intentional fallthrough.
            case MaPLParser::BITWISE_XOR: // Intentional fallthrough.
            case MaPLParser::BITWISE_OR: // Intentional fallthrough.
            case MaPLParser::BITWISE_SHIFT_LEFT: // Intentional fallthrough.
            case MaPLParser::BITWISE_SHIFT_RIGHT: {
                std::vector<MaPLParser::ExpressionContext *> childExpressions = expression->expression();
                MaPLType type1 = dataTypeForExpression(childExpressions[0]);
                MaPLType type2 = dataTypeForExpression(childExpressions[1]);
                if (isIntegral(type1.type) && isIntegral(type2.type)) {
                    return { reconcileTypes(type1.type, type2.type, expression->keyToken) };
                }
                logError(this, expression->keyToken, "Both operands must be integers.");
                return { MaPLPrimitiveType_InvalidType };
            }
            case MaPLParser::BITWISE_NEGATION: {
                    MaPLType type = dataTypeForExpression(expression->expression(0));
                    if (!isIntegral(type.type)) {
                        logError(this, expression->keyToken, "Bitwise negation can only be applied to integer data types.");
                        return { MaPLPrimitiveType_InvalidType };
                    }
                    return type;
            }
            case MaPLParser::TERNARY_CONDITIONAL: {
                std::vector<MaPLParser::ExpressionContext *> childExpressions = expression->expression();
                return reconcileExpressionTypes(childExpressions[1], childExpressions[2], expression->keyToken);
            }
            case MaPLParser::NULL_COALESCING: {
                std::vector<MaPLParser::ExpressionContext *> childExpressions = expression->expression();
                return reconcileExpressionTypes(childExpressions[0], childExpressions[1], expression->keyToken);
            }
            case MaPLParser::LITERAL_INT: return { MaPLPrimitiveType_Int_AmbiguousSizeAndSign };
            case MaPLParser::LITERAL_FLOAT: return { MaPLPrimitiveType_Float_AmbiguousSize };
            case MaPLParser::LITERAL_STRING: return { MaPLPrimitiveType_String };
            case MaPLParser::LITERAL_NULL: return { MaPLPrimitiveType_Pointer };
        }
    }
    MaPLParser::ObjectExpressionContext* objectExpression = expression->objectExpression();
    if (objectExpression) {
        return objectExpressionReturnType(objectExpression, "");
    }
    logError(this, expression->keyToken, "Error determining the type of this expression.");
    return { MaPLPrimitiveType_InvalidType };
}

MaPLType MaPLFile::objectExpressionReturnType(MaPLParser::ObjectExpressionContext *expression, std::string invokedOnType) {
    antlr4::Token *keyToken = expression->keyToken;
    if (keyToken) {
        switch (keyToken->getType()) {
            case MaPLParser::OBJECT_TO_MEMBER: {
                // Find the pointer type from the prefix and supply that type information to the suffix.
                // For example, in the expression "obj.func()", the prefix is "obj" and suffix is "func()".
                // To understand the return type of "func()", we have to understand it's being invoked on "obj".
                std::vector<MaPLParser::ObjectExpressionContext *> childExpressions = expression->objectExpression();
                MaPLType prefixType = objectExpressionReturnType(childExpressions[0], invokedOnType);
                if (prefixType.type != MaPLPrimitiveType_Pointer) {
                    logError(this, keyToken, "The '.' operator can only be used on pointers.");
                    return { MaPLPrimitiveType_InvalidType };
                }
                return objectExpressionReturnType(childExpressions[1], prefixType.pointerType);
            }
            case MaPLParser::SUBSCRIPT_OPEN: {
                // Subscript invocation.
                MaPLType prefixType = objectExpressionReturnType(expression->objectExpression(0), invokedOnType);
                if (prefixType.type != MaPLPrimitiveType_Pointer) {
                    logError(this, keyToken, "The subscript operator can only be used on pointers.");
                    return { MaPLPrimitiveType_InvalidType };
                }
                MaPLType indexType = dataTypeForExpression(expression->expression(0));
                MaPLParser::ApiSubscriptContext *subscript = findSubscript(this, prefixType.pointerType, indexType);
                if (!subscript) {
                    logError(this, expression->keyToken, "Unable to find a subscript on type '"+invokedOnType+"' that takes this type of parameter.");
                    return { MaPLPrimitiveType_InvalidType };
                }
                return typeForTypeContext(subscript->type(0));
            }
            case MaPLParser::PAREN_OPEN: {
                // Function invocation.
                std::vector<MaPLType> parameterTypes;
                for (MaPLParser::ExpressionContext *parameterExpression : expression->expression()) {
                    parameterTypes.push_back(dataTypeForExpression(parameterExpression));
                }
                std::string functionName = expression->identifier()->getText();
                MaPLParser::ApiFunctionContext *function = findFunction(this,
                                                                        invokedOnType,
                                                                        functionName,
                                                                        parameterTypes);
                if (!function) {
                    if (invokedOnType.empty()) {
                        logError(this, expression->identifier()->start, "Unable to find a global '"+functionName+"' function with matching parameters.");
                    } else {
                        logError(this, expression->identifier()->start, "Unable to find a '"+functionName+"' function on type '"+invokedOnType+"' with matching parameters.");
                    }
                    return { MaPLPrimitiveType_InvalidType };
                }
                if (function->API_VOID()) {
                    return { MaPLPrimitiveType_Void };
                }
                return typeForTypeContext(function->type());
            }
            default:
                break;
        }
    } else {
        MaPLParser::IdentifierContext *identifier = expression->identifier();
        if (identifier) {
            // TODO: Handle script variables. Prefer variables over properties.
            
            std::string propertyName = expression->identifier()->getText();
            MaPLParser::ApiPropertyContext *property = findProperty(this,
                                                         invokedOnType,
                                                         propertyName);
            if (!property) {
                if (invokedOnType.empty()) {
                    logError(this, expression->identifier()->start, "Unable to find a variable or global property named '"+propertyName+"'.");
                } else {
                    logError(this, expression->identifier()->start, "Unable to find a '"+propertyName+"' property on type '"+invokedOnType+"'.");
                }
                return { MaPLPrimitiveType_InvalidType };
            }
            return typeForTypeContext(property->type());
        }
    }
    logError(this, keyToken, "Error determining the type of this expression.");
    return { MaPLPrimitiveType_InvalidType };
}

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
#include "MaPLVariableStack.h"
#include "MaPLBytecodeConstants.h"

MaPLFile::MaPLFile(std::filesystem::path &normalizedFilePath, MaPLFileCache *fileCache) :
    _normalizedFilePath(normalizedFilePath),
    _fileCache(fileCache),
    _inputStream(NULL),
    _lexer(NULL),
    _tokenStream(NULL),
    _parser(NULL),
    _program(NULL),
    _bytecode(NULL),
    _variableStack(new MaPLVariableStack())
{
}

bool MaPLFile::parseRawScript() {
    if (_program) {
        return true;
    }
    // Read the raw script from the file system.
    std::ifstream inputStream(_normalizedFilePath);
    if (!inputStream) {
        logError(this, NULL, "Unable to read script file.");
        return false;
    }
    std::stringstream stringBuffer;
    stringBuffer << inputStream.rdbuf();
    std::string rawScriptText = stringBuffer.str();
    
    // Parse the program with ANTLR.
    _inputStream = new antlr4::ANTLRInputStream(rawScriptText);
    _lexer = new MaPLLexer(_inputStream);
    _tokenStream = new antlr4::CommonTokenStream(_lexer);
    _parser = new MaPLParser(_tokenStream);
    
    _parser->getErrorListenerDispatch().removeErrorListeners();
    _parser->addErrorListener(this);
    
    _program = _parser->program();
    
    // Iterate over any API imports and add the corresponding files to the file cache.
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
    
    // Concatenate all preceding bytecode and variables from dependencies.
    for(MaPLFile *file : _dependencies) {
        MaPLBuffer *dependencyBytecode = file->getBytecode();
        if (dependencyBytecode) {
            _bytecode->appendBytes(dependencyBytecode->getBytes(), dependencyBytecode->getByteCount());
        }
        MaPLVariableStack *dependencyStack = file->getVariableStack();
        if (dependencyStack) {
            for (std::pair<std::string, MaPLVariable> pair : dependencyStack->getTopLevelVariables()) {
                _variableStack->declareVariable(pair.first, pair.second);
            }
        }
    }
    
    // Compile the bytecode from this file.
    compileChildNodes(_program, { MaPLPrimitiveType_InvalidType }, _bytecode);
    
    return _bytecode;
}

MaPLVariableStack *MaPLFile::getVariableStack() {
    // Compile the script if needed.
    if (!getBytecode()) {
        return NULL;
    }
    return _variableStack;
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

void MaPLFile::compileChildNodes(antlr4::ParserRuleContext *node, MaPLType expectedType, MaPLBuffer *currentBuffer) {
    for (antlr4::tree::ParseTree *child : node->children) {
        antlr4::ParserRuleContext *ruleContext = dynamic_cast<antlr4::ParserRuleContext *>(child);
        if (ruleContext) {
            compileNode(ruleContext, expectedType, currentBuffer);
        }
    }
}

void MaPLFile::compileNode(antlr4::ParserRuleContext *node, MaPLType expectedType, MaPLBuffer *currentBuffer) {
    switch (node->getRuleIndex()) {
        case MaPLParser::RuleApiDeclaration:
            // TODO: For #type declarations, check for duplicate types and functions, properties, and subscripts within types.
            compileChildNodes(node, expectedType, currentBuffer);
            break;
        case MaPLParser::RuleApiInheritance: {
            MaPLParser::ApiInheritanceContext *inheritance = (MaPLParser::ApiInheritanceContext *)node;
            for (MaPLParser::IdentifierContext *identifier : inheritance->identifier()) {
                if (!findType(this, identifier->getText())) {
                    missingTypeError(identifier->start, identifier->getText());
                }
            }
        }
            break;
        case MaPLParser::RuleApiFunction: {
            MaPLParser::ApiFunctionContext *function = (MaPLParser::ApiFunctionContext *)node;
            if (!function->API_VOID()) {
                MaPLParser::TypeContext *typeContext = function->type();
                MaPLType returnType = typeForTypeContext(typeContext);
                if (returnType.primitiveType == MaPLPrimitiveType_Pointer && !findType(this, returnType.pointerType)) {
                    missingTypeError(typeContext->start, returnType.pointerType);
                }
            }
            compileNode(function->apiFunctionArgs(), expectedType, currentBuffer);
        }
            break;
        case MaPLParser::RuleApiFunctionArgs: {
            MaPLParser::ApiFunctionArgsContext *args = (MaPLParser::ApiFunctionArgsContext *)node;
            for (MaPLParser::TypeContext *typeContext : args->type()) {
                MaPLType parameterType = typeForTypeContext(typeContext);
                if (parameterType.primitiveType == MaPLPrimitiveType_Pointer && !findType(this, parameterType.pointerType)) {
                    missingTypeError(typeContext->start, parameterType.pointerType);
                }
            }
        }
            break;
        case MaPLParser::RuleApiProperty: {
            MaPLParser::ApiPropertyContext *property = (MaPLParser::ApiPropertyContext *)node;
            MaPLParser::TypeContext *typeContext = property->type();
            MaPLType returnType = typeForTypeContext(typeContext);
            if (returnType.primitiveType == MaPLPrimitiveType_Pointer && !findType(this, returnType.pointerType)) {
                missingTypeError(typeContext->start, returnType.pointerType);
            }
        }
            break;
        case MaPLParser::RuleApiSubscript: {
            MaPLParser::ApiSubscriptContext *subscript = (MaPLParser::ApiSubscriptContext *)node;
            for (MaPLParser::TypeContext *typeContext : subscript->type()) {
                MaPLType type = typeForTypeContext(typeContext);
                if (type.primitiveType == MaPLPrimitiveType_Pointer && !findType(this, type.pointerType)) {
                    missingTypeError(typeContext->start, type.pointerType);
                }
            }
        }
            break;
        case MaPLParser::RuleStatement: {
            MaPLParser::StatementContext *statement = (MaPLParser::StatementContext *)node;
            antlr4::tree::TerminalNode *terminalNode = statement->METADATA();
            if (terminalNode) {
                std::string rawMetadata = terminalNode->getText();
                std::string trimmedMetadata = rawMetadata.substr(2, rawMetadata.length()-4);
                currentBuffer->appendByte(MAPL_BYTE_METADATA);
                const char *metadataChars = trimmedMetadata.c_str();
                currentBuffer->appendBytes(metadataChars, strlen(metadataChars)+1);
            } else {
                compileChildNodes(node, expectedType, currentBuffer);
            }
        }
            break;
        case MaPLParser::RuleImperativeStatement: {
            MaPLParser::ImperativeStatementContext *statement = (MaPLParser::ImperativeStatementContext *)node;
            if (statement->keyToken) {
                switch (statement->keyToken->getType()) {
                    case MaPLParser::BREAK: {
                        currentBuffer->addAnnotation({ MaPLParser::BREAK, currentBuffer->getByteCount() });
                        currentBuffer->appendByte(MAPL_BYTE_CURSOR_MOVE_FORWARD);
                        MaPL_Index placeholderIndex = 0;
                        currentBuffer->appendBytes(&placeholderIndex, sizeof(MaPL_Index));
                    }
                        break;
                    case MaPLParser::CONTINUE: {
                        currentBuffer->addAnnotation({ MaPLParser::CONTINUE, currentBuffer->getByteCount() });
                        currentBuffer->appendByte(MAPL_BYTE_CURSOR_MOVE_BACK);
                        MaPL_Index placeholderIndex = 0;
                        currentBuffer->appendBytes(&placeholderIndex, sizeof(MaPL_Index));
                    }
                        break;
                    case MaPLParser::EXIT:
                        currentBuffer->appendByte(MAPL_BYTE_PROGRAM_EXIT);
                        break;
                    default: break;
                }
            } else {
                compileChildNodes(node, expectedType, currentBuffer);
            }
        }
            break;
        case MaPLParser::RuleVariableDeclaration: {
            MaPLParser::VariableDeclarationContext *declaration = (MaPLParser::VariableDeclarationContext *)node;
            MaPLParser::IdentifierContext *identifier = declaration->identifier();
            std::string variableName = identifier->getText();
            
            // Check if this variable name conflicts with any global property.
            if (findProperty(this, "", variableName)) {
                logError(this, identifier->start, "Variable with name '"+variableName+"' conflicts with global property of the same name.");
            }
            
            // Claim a spot in memory for this variable.
            MaPLVariable variable = { typeForTypeContext(declaration->type()), this, identifier->start };
            _variableStack->declareVariable(variableName, variable);
            
            // Assign the value to this variable if needed.
            MaPLParser::ExpressionContext *expression = declaration->expression();
            if (expression) {
                currentBuffer->appendByte(assignmentInstructionForPrimitive(variable.type.primitiveType));
                MaPL_Index variableByteOffset = _variableStack->getVariable(variableName).byteOffset;
                currentBuffer->appendBytes(&variableByteOffset, sizeof(MaPL_Index));
                compileNode(expression, variable.type, currentBuffer);
            }
        }
            break;
        case MaPLParser::RuleAssignStatement: {
            MaPLParser::AssignStatementContext *assignment = (MaPLParser::AssignStatementContext *)node;
            // TODO: assigning to object expressions will be relatively complex, revisit this last.
        }
            break;
        case MaPLParser::RuleUnaryStatement: {
            MaPLParser::UnaryStatementContext *statement = (MaPLParser::UnaryStatementContext *)node;
            // TODO: assigning to object expressions will be relatively complex, revisit this last.
        }
            break;
        case MaPLParser::RuleObjectExpression: {
            MaPLParser::ObjectExpressionContext *expression = (MaPLParser::ObjectExpressionContext *)node;
            switch (expression->keyToken->getType()) {
                case MaPLParser::OBJECT_TO_MEMBER: // Compound object expression.
                    
                    break;
                case MaPLParser::SUBSCRIPT_OPEN: // Subscript invocation.
                    
                    break;
                case MaPLParser::PAREN_OPEN: // Function invocation.
                    
                    break;
                default: // Property invokation.
                    
                    break;
            }
        }
            break;
        case MaPLParser::RuleExpression: {
            MaPLParser::ExpressionContext *expression = (MaPLParser::ExpressionContext *)node;
            if (expectedType.primitiveType != MaPLPrimitiveType_InvalidType) {
                MaPLType expressionType = dataTypeForExpression(expression);
                if(!isCompatibleType(this, expectedType, expressionType)) {
                    std::string error = "Expression is required to be of type "+descriptorForType(expectedType)+", but was "+descriptorForType(expressionType)+" instead.";
                    logError(this, expression->start, error);
                }
            }
            
            if (expression->keyToken) {
                switch (expression->keyToken->getType()) {
                    case MaPLParser::PAREN_OPEN: { // Typecast.
                        // TODO: Implement this.
                    }
                        break;
                    case MaPLParser::LOGICAL_AND:
                        currentBuffer->appendByte(MAPL_BYTE_LOGICAL_AND);
                        compileNode(expression->expression(0), { MaPLPrimitiveType_Boolean }, currentBuffer);
                        compileNode(expression->expression(1), { MaPLPrimitiveType_Boolean }, currentBuffer);
                        break;
                    case MaPLParser::LOGICAL_OR:
                        currentBuffer->appendByte(MAPL_BYTE_LOGICAL_OR);
                        compileNode(expression->expression(0), { MaPLPrimitiveType_Boolean }, currentBuffer);
                        compileNode(expression->expression(1), { MaPLPrimitiveType_Boolean }, currentBuffer);
                        break;
                    case MaPLParser::BITWISE_AND: {
                        // TODO: Implement this.
                    }
                        break;
                    case MaPLParser::BITWISE_XOR: {
                        // TODO: Implement this.
                    }
                        break;
                    case MaPLParser::BITWISE_OR: {
                        // TODO: Implement this.
                    }
                        break;
                    case MaPLParser::LOGICAL_EQUALITY: {
                        // TODO: Implement this.
                    }
                        break;
                    case MaPLParser::LOGICAL_INEQUALITY: {
                        // TODO: Implement this.
                    }
                        break;
                    case MaPLParser::LESS_THAN: {
                        // TODO: Implement this.
                    }
                        break;
                    case MaPLParser::LESS_THAN_EQUAL: {
                        // TODO: Implement this.
                    }
                        break;
                    case MaPLParser::GREATER_THAN: {
                        // TODO: Implement this.
                    }
                        break;
                    case MaPLParser::GREATER_THAN_EQUAL: {
                        // TODO: Implement this.
                    }
                        break;
                    case MaPLParser::LOGICAL_NEGATION: {
                        currentBuffer->appendByte(MAPL_BYTE_LOGICAL_NEGATION);
                        compileNode(expression->expression(0), { MaPLPrimitiveType_Boolean }, currentBuffer);
                    }
                        break;
                    case MaPLParser::BITWISE_NEGATION: {
                        // TODO: Implement this.
                    }
                        break;
                    case MaPLParser::BITWISE_SHIFT_LEFT: {
                        // TODO: Implement this.
                    }
                        break;
                    case MaPLParser::BITWISE_SHIFT_RIGHT: {
                        // TODO: Implement this.
                    }
                        break;
                    case MaPLParser::MOD: {
                        // TODO: Implement this.
                    }
                        break;
                    case MaPLParser::MULTIPLY: {
                        // TODO: Implement this.
                    }
                        break;
                    case MaPLParser::DIVIDE: {
                        // TODO: Implement this.
                    }
                        break;
                    case MaPLParser::ADD: {
                        // TODO: Implement this.
                    }
                        break;
                    case MaPLParser::SUBTRACT: {
                        // TODO: Implement this.
                    }
                        break;
                    case MaPLParser::TERNARY_CONDITIONAL: {
                        // TODO: Implement this.
                    }
                        break;
                    case MaPLParser::NULL_COALESCING: {
                        // TODO: Implement this.
                    }
                        break;
                    case MaPLParser::PAREN_CLOSE: // Parenthesized expression.
                        compileNode(expression->expression(0), expectedType, currentBuffer);
                        break;
                    case MaPLParser::LITERAL_TRUE:
                        currentBuffer->appendByte(MAPL_BYTE_LITERAL_BOOLEAN_TRUE);
                        break;
                    case MaPLParser::LITERAL_FALSE:
                        currentBuffer->appendByte(MAPL_BYTE_LITERAL_BOOLEAN_FALSE);
                        break;
                    case MaPLParser::LITERAL_NULL:
                        currentBuffer->appendByte(MAPL_BYTE_LITERAL_NULL);
                        break;
                    case MaPLParser::LITERAL_INT: {
                        // TODO: Implement this.
                    }
                        break;
                    case MaPLParser::LITERAL_FLOAT: {
                        // TODO: Implement this.
                    }
                        break;
                    case MaPLParser::LITERAL_STRING: {
                        currentBuffer->appendByte(MAPL_BYTE_LITERAL_STRING);
                        
                        // The text will always contain the string quotes, substring call removes them.
                        std::string literalString = expression->LITERAL_STRING()->getText();
                        std::filesystem::path string = literalString.substr(1, literalString.length()-2);
                        
                        // Append the string.
                        const char* cString = string.c_str();
                        size_t length = strlen(cString);
                        currentBuffer->appendBytes(cString, length);
                        
                        // Append the null char to terminate the string.
                        currentBuffer->appendByte(0);
                    }
                        break;
                    default: break;
                }
            } else {
                MaPLParser::ObjectExpressionContext *objectExpression = expression->objectExpression();
                if (objectExpression) {
                    compileNode(objectExpression, expectedType, currentBuffer);
                }
            }
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

void MaPLFile::missingTypeError(antlr4::Token *errorToken, std::string typeName) {
    logError(this, errorToken, "Unable to find the type declaration for '"+typeName+"'.");
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
    MaPLPrimitiveType reconciledPrimitive = reconcileTypes(type1.primitiveType, type2.primitiveType, errorToken);
    if (reconciledPrimitive == MaPLPrimitiveType_Pointer && type1.pointerType != type2.pointerType) {
        // The pointer types don't match. Generate an error message that gives some context.
        std::vector<std::string> possibleMatches = mutualAncestorTypes(this, type1.pointerType, type2.pointerType);
        size_t possibleMatchCount = possibleMatches.size();
        if (possibleMatchCount == 1) {
            // In the case where there's only one possible choice, make the inference.
            return { MaPLPrimitiveType_Pointer, possibleMatches[0] };
        }
        std::string errorSuffix;
        if (possibleMatchCount == 0) {
            errorSuffix = "There are no common ancestors for the types '"+type1.pointerType+"' and '"+type2.pointerType+"'.";
        } else {
            errorSuffix = "The possible ancestor types for '"+type1.pointerType+"' and '"+type2.pointerType+"' are ";
            for (size_t i = 0; i < possibleMatchCount; i++) {
                std::string possibleMatch = possibleMatches[i];
                errorSuffix += "'"+possibleMatch+"'";
                if (i == (possibleMatchCount-1)) {
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
                if (type.primitiveType == MaPLPrimitiveType_InvalidType) {
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
                    if (isUnsignedInt(type.primitiveType)) {
                        logError(this, expression->keyToken, "Unsigned integers cannot be negated.");
                        return { MaPLPrimitiveType_InvalidType };
                    }
                    if (type.primitiveType == MaPLPrimitiveType_Int_AmbiguousSizeAndSign) {
                        // If the datatype is ambiguous, but it's being negated, it must be a signed int.
                        return { MaPLPrimitiveType_SignedInt_AmbiguousSize };
                    }
                    if (isNumeric(type.primitiveType)) {
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
                if (isNumeric(type1.primitiveType) && isNumeric(type2.primitiveType)) {
                    return { reconcileTypes(type1.primitiveType, type2.primitiveType, expression->keyToken) };
                }
                logError(this, expression->keyToken, "Both operands must be numeric.");
                return { MaPLPrimitiveType_InvalidType };
            }
            case MaPLParser::ADD: {
                std::vector<MaPLParser::ExpressionContext *> childExpressions = expression->expression();
                MaPLType type1 = dataTypeForExpression(childExpressions[0]);
                MaPLType type2 = dataTypeForExpression(childExpressions[1]);
                // Plus operator could be numeric add or string concatenation.
                if (type1.primitiveType == MaPLPrimitiveType_String &&
                    type2.primitiveType == MaPLPrimitiveType_String &&
                    expression->keyToken->getType() == MaPLParser::ADD) {
                    return { MaPLPrimitiveType_String };
                } else if (isNumeric(type1.primitiveType) && isNumeric(type2.primitiveType)) {
                    return { reconcileTypes(type1.primitiveType, type2.primitiveType, expression->keyToken) };
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
                if (isIntegral(type1.primitiveType) && isIntegral(type2.primitiveType)) {
                    return { reconcileTypes(type1.primitiveType, type2.primitiveType, expression->keyToken) };
                }
                logError(this, expression->keyToken, "Both operands must be integers.");
                return { MaPLPrimitiveType_InvalidType };
            }
            case MaPLParser::BITWISE_NEGATION: {
                    MaPLType type = dataTypeForExpression(expression->expression(0));
                    if (!isIntegral(type.primitiveType)) {
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
                if (prefixType.primitiveType != MaPLPrimitiveType_Pointer) {
                    logError(this, keyToken, "The '.' operator can only be used on pointers.");
                    return { MaPLPrimitiveType_InvalidType };
                }
                return objectExpressionReturnType(childExpressions[1], prefixType.pointerType);
            }
            case MaPLParser::SUBSCRIPT_OPEN: {
                // Subscript invocation.
                MaPLType prefixType = objectExpressionReturnType(expression->objectExpression(0), invokedOnType);
                if (prefixType.primitiveType != MaPLPrimitiveType_Pointer) {
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
            std::string propertyOrVariableName = identifier->getText();
            
            // Prefer recognizing this identifier as a variable.
            bool isInvokedOnType = !invokedOnType.empty();
            if (!isInvokedOnType) {
                MaPLVariable variable = _variableStack->getVariable(propertyOrVariableName);
                if (variable.type.primitiveType != MaPLPrimitiveType_InvalidType) {
                    return variable.type;
                }
            }
            
            // The name doesn't match a variable, check if it matches a property.
            MaPLParser::ApiPropertyContext *property = findProperty(this,
                                                                    invokedOnType,
                                                                    propertyOrVariableName);
            if (!property) {
                if (isInvokedOnType) {
                    logError(this, identifier->start, "Unable to find a '"+propertyOrVariableName+"' property on type '"+invokedOnType+"'.");
                } else {
                    logError(this, identifier->start, "Unable to find a variable or global property named '"+propertyOrVariableName+"'.");
                }
                return { MaPLPrimitiveType_InvalidType };
            }
            return typeForTypeContext(property->type());
        }
    }
    logError(this, keyToken, "Error determining the type of this expression.");
    return { MaPLPrimitiveType_InvalidType };
}

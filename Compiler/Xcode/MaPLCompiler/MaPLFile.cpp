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
    if(findInheritanceCycle(this)) {
        return NULL;
    }
    _bytecode = new MaPLBuffer(10);
    
    // Concatenate all preceding bytecode and variables from dependencies.
    for(MaPLFile *file : _dependencies) {
        MaPLBuffer *dependencyBytecode = file->getBytecode();
        if (dependencyBytecode) {
            _bytecode->appendBuffer(dependencyBytecode, _variableStack->getMaximumMemoryUsed());
        }
        MaPLVariableStack *dependencyStack = file->getVariableStack();
        if (dependencyStack) {
            for (std::pair<std::string, MaPLVariable> pair : dependencyStack->getGlobalVariables()) {
                _variableStack->declareVariable(pair.first, pair.second);
            }
        }
    }
    
    // Compile the bytecode from this file.
    compileChildNodes(_program, { MaPLPrimitiveType_Uninitialized }, _bytecode);
    
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

void MaPLFile::compileChildNodes(antlr4::ParserRuleContext *node, const MaPLType &expectedType, MaPLBuffer *currentBuffer) {
    for (antlr4::tree::ParseTree *child : node->children) {
        antlr4::ParserRuleContext *ruleContext = dynamic_cast<antlr4::ParserRuleContext *>(child);
        if (ruleContext) {
            compileNode(ruleContext, expectedType, currentBuffer);
        }
    }
}

void MaPLFile::compileNode(antlr4::ParserRuleContext *node, const MaPLType &expectedType, MaPLBuffer *currentBuffer) {
    switch (node->getRuleIndex()) {
        case MaPLParser::RuleApiDeclaration: {
            MaPLParser::ApiDeclarationContext *apiDeclaration = (MaPLParser::ApiDeclarationContext *)node;
            if (apiDeclaration->keyToken->getType() == MaPLParser::API_TYPE) {
                MaPLParser::IdentifierContext *identifier = apiDeclaration->identifier();
                std::string typeName = identifier->getText();
                
                // Check for duplicate symbols.
                if (findType(this, typeName, apiDeclaration)) {
                    logError(this, identifier->start, "Type name '"+typeName+"' conflicts with another type of the same name.");
                }
            }
            compileChildNodes(node, expectedType, currentBuffer);
        }
            break;
        case MaPLParser::RuleApiInheritance: {
            MaPLParser::ApiInheritanceContext *inheritance = (MaPLParser::ApiInheritanceContext *)node;
            
            // Check all the types referenced in this API to make sure they exist.
            for (MaPLParser::IdentifierContext *identifier : inheritance->identifier()) {
                if (!findType(this, identifier->getText(), NULL)) {
                    missingTypeError(identifier->start, identifier->getText());
                }
            }
        }
            break;
        case MaPLParser::RuleApiFunction: {
            MaPLParser::ApiFunctionContext *function = (MaPLParser::ApiFunctionContext *)node;
            
            // Check the return type referenced in this API to make sure it exists.
            if (!function->API_VOID()) {
                MaPLParser::TypeContext *typeContext = function->type();
                MaPLType returnType = typeForTypeContext(typeContext);
                if (returnType.primitiveType == MaPLPrimitiveType_Pointer && !findType(this, returnType.pointerType, NULL)) {
                    missingTypeError(typeContext->start, returnType.pointerType);
                }
            }
            MaPLParser::ApiFunctionArgsContext *args = function->apiFunctionArgs();
            std::vector<MaPLType> parameterTypes;
            if (args) {
                for (MaPLParser::TypeContext *typeContext : args->type()) {
                    MaPLType parameterType = typeForTypeContext(typeContext);
                    parameterTypes.push_back(parameterType);
                    // Check all the types referenced in this API to make sure they exist.
                    if (parameterType.primitiveType == MaPLPrimitiveType_Pointer && !findType(this, parameterType.pointerType, NULL)) {
                        missingTypeError(typeContext->start, parameterType.pointerType);
                    }
                }
            }
            
            // Check for duplicate symbols.
            MaPLParser::ApiDeclarationContext *parentApi = dynamic_cast<MaPLParser::ApiDeclarationContext *>(function->parent);
            bool isGlobal = parentApi->keyToken->getType() == MaPLParser::API_GLOBAL;
            std::string parentTypeName = isGlobal ? "" : parentApi->identifier()->getText();
            MaPLParameterStrategy strategy = args && args->API_VARIADIC_ARGUMENTS() != NULL ? MaPLParameterStrategy_Exact_IncludeVariadicArgs : MaPLParameterStrategy_Exact_NoVariadicArgs;
            if (findFunction(this, parentTypeName, function->identifier()->getText(), parameterTypes, strategy, function)) {
                std::string functionSignature = descriptorForFunction(function->identifier()->getText(),
                                                                      parameterTypes,
                                                                      strategy == MaPLParameterStrategy_Exact_IncludeVariadicArgs);
                if (isGlobal) {
                    logError(this, function->identifier()->start, "Function '"+functionSignature+"' conflicts with another global function with the same name and parameters.");
                } else {
                    logError(this, function->identifier()->start, "Function '"+functionSignature+"' conflicts with another function with the same name and parameters in type '"+parentTypeName+"' or one of its parent types.");
                }
            }
        }
            break;
        case MaPLParser::RuleApiProperty: {
            MaPLParser::ApiPropertyContext *property = (MaPLParser::ApiPropertyContext *)node;
            
            // Check the return type referenced in this API to make sure it exists.
            MaPLParser::TypeContext *typeContext = property->type();
            MaPLType returnType = typeForTypeContext(typeContext);
            if (returnType.primitiveType == MaPLPrimitiveType_Pointer && !findType(this, returnType.pointerType, NULL)) {
                missingTypeError(typeContext->start, returnType.pointerType);
            }
            
            // Check for duplicate symbols.
            MaPLParser::ApiDeclarationContext *parentApi = dynamic_cast<MaPLParser::ApiDeclarationContext *>(property->parent);
            bool isGlobal = parentApi->keyToken->getType() == MaPLParser::API_GLOBAL;
            std::string parentTypeName = isGlobal ? "" : parentApi->identifier()->getText();
            if (findProperty(this, parentTypeName, property->identifier()->getText(), property)) {
                if (isGlobal) {
                    logError(this, property->identifier()->start, "Property name '"+property->identifier()->getText()+"' conflicts with another global property with the same name.");
                } else {
                    logError(this, property->identifier()->start, "Property name '"+property->identifier()->getText()+"' conflicts with another property with the same name in type '"+parentTypeName+"' or one of its parent types.");
                }
            }
        }
            break;
        case MaPLParser::RuleApiSubscript: {
            MaPLParser::ApiSubscriptContext *subscript = (MaPLParser::ApiSubscriptContext *)node;
            
            // Check all the types referenced in this API to make sure they exist.
            for (MaPLParser::TypeContext *typeContext : subscript->type()) {
                MaPLType type = typeForTypeContext(typeContext);
                if (type.primitiveType == MaPLPrimitiveType_Pointer && !findType(this, type.pointerType, NULL)) {
                    missingTypeError(typeContext->start, type.pointerType);
                }
            }
            
            // Check for duplicate symbols.
            MaPLParser::ApiDeclarationContext *parentApi = dynamic_cast<MaPLParser::ApiDeclarationContext *>(subscript->parent);
            MaPLType indexType = typeForTypeContext(subscript->type(1));
            if (findSubscript(this, parentApi->identifier()->getText(), indexType, subscript)) {
                logError(this, subscript->type(1)->start, "Subscript indexed by "+descriptorForType(indexType)+" conflicts with another subscript in type '"+parentApi->identifier()->getText()+"' or one of its parent types.");
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
                        if (!isInsideLoopScope(statement)) {
                            logError(this, statement->keyToken, "Break statements can only be used within loops.");
                        }
                        currentBuffer->addAnnotation({ currentBuffer->getByteCount(), MaPLBufferAnnotationType_Break });
                        currentBuffer->appendByte(0);
                        MaPL_Index placeholderIndex = 0;
                        currentBuffer->appendBytes(&placeholderIndex, sizeof(MaPL_Index));
                    }
                        break;
                    case MaPLParser::CONTINUE: {
                        if (!isInsideLoopScope(statement)) {
                            logError(this, statement->keyToken, "Continue statements can only be used within loops.");
                        }
                        currentBuffer->addAnnotation({ currentBuffer->getByteCount(), MaPLBufferAnnotationType_Continue });
                        currentBuffer->appendByte(0);
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
            if (findProperty(this, "", variableName, NULL)) {
                logError(this, identifier->start, "Variable with name '"+variableName+"' conflicts with global property of the same name.");
            }
            
            // Claim a spot in memory for this variable.
            MaPLVariable variable = { typeForTypeContext(declaration->type()), this, identifier->start };
            _variableStack->declareVariable(variableName, variable);
            
            // Assign the value to this variable if needed.
            MaPLParser::ExpressionContext *expression = declaration->expression();
            if (expression) {
                currentBuffer->appendByte(assignmentInstructionForPrimitive(variable.type.primitiveType));
                currentBuffer->addAnnotation({ currentBuffer->getByteCount(), MaPLBufferAnnotationType_VariableOffset });
                MaPL_Index variableByteOffset = _variableStack->getVariable(variableName).byteOffset;
                currentBuffer->appendBytes(&variableByteOffset, sizeof(MaPL_Index));
                compileNode(expression, variable.type, currentBuffer);
            }
        }
            break;
        case MaPLParser::RuleAssignStatement: {
            MaPLParser::AssignStatementContext *assignment = (MaPLParser::AssignStatementContext *)node;
            // TODO: Assigning to object expressions will be relatively complex, revisit this last.
            // TODO: Check if APIs are 'readonly' before assigning to them.
        }
            break;
        case MaPLParser::RuleUnaryStatement: {
            MaPLParser::UnaryStatementContext *statement = (MaPLParser::UnaryStatementContext *)node;
            // TODO: Assigning to object expressions will be relatively complex, revisit this last.
            // TODO: Check if APIs are 'readonly' before assigning to them.
        }
            break;
        case MaPLParser::RuleObjectExpression: {
            MaPLParser::ObjectExpressionContext *expression = (MaPLParser::ObjectExpressionContext *)node;
            // TODO: Object expressions can appear within imperative statements, but the only type of expression that's valid in that context is a function invocation. Check parent node for this type of error.
            if (expression->keyToken) {
                switch (expression->keyToken->getType()) {
                    case MaPLParser::OBJECT_TO_MEMBER: // Compound object expression.
                        // TODO: Implement this.
                        break;
                    case MaPLParser::SUBSCRIPT_OPEN: // Subscript invocation.
                        // TODO: Implement this.
                        break;
                    case MaPLParser::PAREN_OPEN: // Function invocation.
                        // TODO: Implement this.
                        break;
                    default: break;
                }
            } else {
                // Property invocation.
                // TODO: Implement this.
            }
        }
            break;
        case MaPLParser::RuleExpression: {
            MaPLParser::ExpressionContext *expression = (MaPLParser::ExpressionContext *)node;
            if (!isConcreteType(expectedType.primitiveType)) {
                // It's safe to assume initialized expectedType in every context in which expressions are found:
                //   On the right side of an assignment. -> Implied by the type being assigned to.
                //   Nested within other expressions. -> Inherited from parent expression.
                //   As an index for a subscript. -> Type implied by index param.
                //   As a parameter for a function. -> Type implied by param. Untyped for variadic args, but can "logAmbiguousLiteralError" if needed.
                //   As the conditional for a control flow statement (if, while, for, doWhile). -> Always bool.
                logError(this, expression->start, "Internal compiler error. No concrete expected type for expression.");
                break;
            }
            MaPLType expressionType = dataTypeForExpression(expression);
            if (expressionType.primitiveType == MaPLPrimitiveType_TypeError) {
                // If expressionType is "TypeError", an error describing why was already logged.
                break;
            }
            if(!isAssignable(this, expressionType, expectedType)) {
                std::string error = "Expression is required to be of type "+descriptorForType(expectedType)+", but was "+descriptorForType(expressionType)+" instead.";
                logError(this, expression->start, error);
                break;
            }
            
            MaPLLiteral literal = constantValueForExpression(expression);
            if (literal.type.primitiveType != MaPLPrimitiveType_Uninitialized) {
                // This expression can be boiled down to a single compile-time constant.
                if (isAmbiguousNumericType(literal.type.primitiveType)) {
                    literal = castLiteralToType(literal, expectedType);
                }
                switch (literal.type.primitiveType) {
                    case MaPLPrimitiveType_Int8:
                        currentBuffer->appendByte(MAPL_BYTE_LITERAL_INT8);
                        currentBuffer->appendBytes(&(literal.int8Value), sizeof(literal.int8Value));
                        break;
                    case MaPLPrimitiveType_Int16:
                        currentBuffer->appendByte(MAPL_BYTE_LITERAL_INT16);
                        currentBuffer->appendBytes(&(literal.int16Value), sizeof(literal.int16Value));
                        break;
                    case MaPLPrimitiveType_Int32:
                        currentBuffer->appendByte(MAPL_BYTE_LITERAL_INT32);
                        currentBuffer->appendBytes(&(literal.int32Value), sizeof(literal.int32Value));
                        break;
                    case MaPLPrimitiveType_Int64:
                        currentBuffer->appendByte(MAPL_BYTE_LITERAL_INT64);
                        currentBuffer->appendBytes(&(literal.int64Value), sizeof(literal.int64Value));
                        break;
                    case MaPLPrimitiveType_UInt8:
                        currentBuffer->appendByte(MAPL_BYTE_LITERAL_UINT8);
                        currentBuffer->appendBytes(&(literal.uInt8Value), sizeof(literal.uInt8Value));
                        break;
                    case MaPLPrimitiveType_UInt16:
                        currentBuffer->appendByte(MAPL_BYTE_LITERAL_UINT16);
                        currentBuffer->appendBytes(&(literal.uInt16Value), sizeof(literal.uInt16Value));
                        break;
                    case MaPLPrimitiveType_UInt32:
                        currentBuffer->appendByte(MAPL_BYTE_LITERAL_UINT32);
                        currentBuffer->appendBytes(&(literal.uInt32Value), sizeof(literal.uInt32Value));
                        break;
                    case MaPLPrimitiveType_UInt64:
                        currentBuffer->appendByte(MAPL_BYTE_LITERAL_UINT64);
                        currentBuffer->appendBytes(&(literal.uInt64Value), sizeof(literal.uInt64Value));
                        break;
                    case MaPLPrimitiveType_Float32:
                        currentBuffer->appendByte(MAPL_BYTE_LITERAL_FLOAT32);
                        currentBuffer->appendBytes(&(literal.float32Value), sizeof(literal.float32Value));
                        break;
                    case MaPLPrimitiveType_Float64:
                        currentBuffer->appendByte(MAPL_BYTE_LITERAL_FLOAT64);
                        currentBuffer->appendBytes(&(literal.float64Value), sizeof(literal.float64Value));
                        break;
                    case MaPLPrimitiveType_String: {
                        currentBuffer->appendByte(MAPL_BYTE_LITERAL_STRING);
                        
                        // Append the string.
                        const char* cString = literal.stringValue.c_str();
                        size_t length = strlen(cString);
                        currentBuffer->appendBytes(cString, length);
                        
                        // Append the null char to terminate the string.
                        currentBuffer->appendByte(0);
                    }
                        break;
                    case MaPLPrimitiveType_Boolean:
                        currentBuffer->appendByte(literal.booleanValue ? MAPL_BYTE_LITERAL_BOOLEAN_TRUE : MAPL_BYTE_LITERAL_BOOLEAN_FALSE);
                        break;
                    case MaPLPrimitiveType_Pointer:
                        currentBuffer->appendByte(MAPL_BYTE_LITERAL_NULL);
                        break;
                    default: break;
                }
                break;
            }
            
            if (expression->keyToken) {
                size_t tokenType = expression->keyToken->getType();
                switch (tokenType) {
                    case MaPLParser::PAREN_OPEN: { // Typecast.
                        MaPLType castType = typeForTypeContext(expression->type());
                        MaPLType expressionType = dataTypeForExpression(expression->expression(0));
                        if (castType.primitiveType == expressionType.primitiveType) {
                            // The cast doesn't involve any casting between primitive types, and so is a no-op in terms of bytecode.
                            if (castType.primitiveType == MaPLPrimitiveType_Pointer &&
                                castType.pointerType != expressionType.pointerType &&
                                !inheritsFromType(this, castType.pointerType, expressionType.pointerType) &&
                                !inheritsFromType(this, expressionType.pointerType, castType.pointerType)) {
                                // Cast is between two pointer types. This only makes sense if the types have some child/ancestor relationship.
                                logError(this, expression->type()->start, "Cast attempted between incompatible pointers. The types '"+castType.pointerType+"' and '"+expressionType.pointerType+"' have no child/ancestor relationship.");
                            }
                            compileNode(expression->expression(0), expressionType, currentBuffer);
                            break;
                        }
                        // All pointer-to-pointer casts already handled above. No primitive can cast to pointer.
                        if (castType.primitiveType == MaPLPrimitiveType_Pointer) {
                            logError(this, expression->type()->start, "Cannot cast primitive "+descriptorForType(expressionType)+" type to pointer type "+descriptorForType(castType)+".");
                            break;
                        }
                        if (expressionType.primitiveType == MaPLPrimitiveType_Pointer) {
                            // Strings are the only primitive that can cast from pointer (to print the memory address of pointers).
                            if (castType.primitiveType == MaPLPrimitiveType_String) {
                                currentBuffer->appendByte(MAPL_BYTE_TYPECAST_FROM_POINTER_TO_STRING);
                                compileNode(expression->expression(0), expressionType, currentBuffer);
                                break;
                            } else {
                                logError(this, expression->type()->start, "Pointer types, like "+descriptorForType(expressionType)+", can only be cast to other pointer types with a child/ancestor relationship or 'string'.");
                                break;
                            }
                        }
                        if (isAmbiguousNumericType(expressionType.primitiveType)) {
                            if (isAssignable(this, expressionType, castType)) {
                                // This cast doesn't change types. Instead it's clarifying what to use as the expected type for an ambiguous literal.
                                compileNode(expression->expression(0), castType, currentBuffer);
                            } else {
                                logAmbiguousLiteralError(this, expressionType.primitiveType, expression->expression(0)->start);
                            }
                            break;
                        }
                        MaPL_Instruction castFrom = typecastFromInstructionForPrimitive(expressionType.primitiveType);
                        MaPL_Instruction castTo = typecastToInstructionForPrimitive(castType.primitiveType);
                        if (!castTo || !castFrom) {
                            logError(this, expression->type()->start, "Unable to cast from type "+descriptorForType(expressionType)+" to "+descriptorForType(castType)+".");
                            break;
                        }
                        currentBuffer->appendByte(castFrom);
                        currentBuffer->appendByte(castTo);
                        compileNode(expression->expression(0), expressionType, currentBuffer);
                    }
                        break;
                    case MaPLParser::LOGICAL_AND: // Intentional fallthrough.
                    case MaPLParser::LOGICAL_OR:
                        if (tokenType == MaPLParser::LOGICAL_AND) {
                            currentBuffer->appendByte(MAPL_BYTE_LOGICAL_AND);
                        } else {
                            currentBuffer->appendByte(MAPL_BYTE_LOGICAL_OR);
                        }
                        compileNode(expression->expression(0), { MaPLPrimitiveType_Boolean }, currentBuffer);
                        compileNode(expression->expression(1), { MaPLPrimitiveType_Boolean }, currentBuffer);
                        break;
                    case MaPLParser::BITWISE_AND: // Intentional fallthrough.
                    case MaPLParser::BITWISE_XOR: // Intentional fallthrough.
                    case MaPLParser::BITWISE_OR: // Intentional fallthrough.
                    case MaPLParser::BITWISE_SHIFT_LEFT: // Intentional fallthrough.
                    case MaPLParser::BITWISE_SHIFT_RIGHT:
                        switch (tokenType) {
                            case MaPLParser::BITWISE_AND:
                                currentBuffer->appendByte(MAPL_BYTE_BITWISE_AND);
                                break;
                            case MaPLParser::BITWISE_XOR:
                                currentBuffer->appendByte(MAPL_BYTE_BITWISE_XOR);
                                break;
                            case MaPLParser::BITWISE_OR:
                                currentBuffer->appendByte(MAPL_BYTE_BITWISE_OR);
                                break;
                            case MaPLParser::BITWISE_SHIFT_LEFT:
                                currentBuffer->appendByte(MAPL_BYTE_BITWISE_SHIFT_LEFT);
                                break;
                            case MaPLParser::BITWISE_SHIFT_RIGHT:
                                currentBuffer->appendByte(MAPL_BYTE_BITWISE_SHIFT_RIGHT);
                                break;
                            default: break;
                        }
                        compileNode(expression->expression(0), expectedType, currentBuffer);
                        compileNode(expression->expression(1), expectedType, currentBuffer);
                        break;
                    case MaPLParser::LOGICAL_EQUALITY: // Intentional fallthrough.
                    case MaPLParser::LOGICAL_INEQUALITY: {
                        MaPLParser::ExpressionContext *leftExpression = expression->expression(0);
                        MaPLParser::ExpressionContext *rightExpression = expression->expression(1);
                        MaPLType leftType = dataTypeForExpression(leftExpression);
                        MaPLType rightType = dataTypeForExpression(rightExpression);
                        MaPLPrimitiveType reconciledType = reconcileTypes(leftType.primitiveType,
                                                                          rightType.primitiveType,
                                                                          expression->keyToken);
                        if (reconciledType == MaPLPrimitiveType_TypeError) {
                            // If there was a type reconciliation error, it's been logged already.
                            break;
                        }
                        if (isAmbiguousNumericType(reconciledType)) {
                            logAmbiguousLiteralError(this, reconciledType, expression->keyToken);
                            break;
                        }
                        if (tokenType == MaPLParser::LOGICAL_EQUALITY) {
                            currentBuffer->appendByte(equalityInstructionForPrimitive(reconciledType));
                        } else {
                            currentBuffer->appendByte(inequalityInstructionForPrimitive(reconciledType));
                        }
                        // Pointers can compare memory addresses without need for equal #type.
                        if (reconciledType == MaPLPrimitiveType_Pointer) {
                            compileNode(leftExpression, leftType, currentBuffer);
                            compileNode(rightExpression, rightType, currentBuffer);
                        } else {
                            compileNode(leftExpression, { reconciledType }, currentBuffer);
                            compileNode(rightExpression, { reconciledType }, currentBuffer);
                        }
                    }
                        break;
                    case MaPLParser::LESS_THAN: // Intentional fallthrough.
                    case MaPLParser::LESS_THAN_EQUAL: // Intentional fallthrough.
                    case MaPLParser::GREATER_THAN: // Intentional fallthrough.
                    case MaPLParser::GREATER_THAN_EQUAL: {
                        MaPLParser::ExpressionContext *leftExpression = expression->expression(0);
                        MaPLParser::ExpressionContext *rightExpression = expression->expression(1);
                        MaPLPrimitiveType reconciledType = reconcileTypes(dataTypeForExpression(leftExpression).primitiveType,
                                                                          dataTypeForExpression(rightExpression).primitiveType,
                                                                          expression->keyToken);
                        if (reconciledType == MaPLPrimitiveType_TypeError) {
                            // If there was a type reconciliation error, it's been logged already.
                            break;
                        }
                        if (isAmbiguousNumericType(reconciledType)) {
                            logAmbiguousLiteralError(this, reconciledType, expression->keyToken);
                            break;
                        }
                        if (!isNumeric(reconciledType)) {
                            logNonNumericOperandsError(this, expression->keyToken);
                            break;
                        }
                        switch (tokenType) {
                            case MaPLParser::LESS_THAN:
                                currentBuffer->appendByte(lessThanInstructionForPrimitive(reconciledType));
                                break;
                            case MaPLParser::LESS_THAN_EQUAL:
                                currentBuffer->appendByte(lessThanOrEqualInstructionForPrimitive(reconciledType));
                                break;
                            case MaPLParser::GREATER_THAN:
                                currentBuffer->appendByte(greaterThanInstructionForPrimitive(reconciledType));
                                break;
                            case MaPLParser::GREATER_THAN_EQUAL:
                                currentBuffer->appendByte(greaterThanOrEqualInstructionForPrimitive(reconciledType));
                                break;
                            default: break;
                        }
                        compileNode(leftExpression, { reconciledType }, currentBuffer);
                        compileNode(rightExpression, { reconciledType }, currentBuffer);
                    }
                        break;
                    case MaPLParser::LOGICAL_NEGATION:
                        currentBuffer->appendByte(MAPL_BYTE_LOGICAL_NEGATION);
                        compileNode(expression->expression(0), { MaPLPrimitiveType_Boolean }, currentBuffer);
                        break;
                    case MaPLParser::BITWISE_NEGATION:
                        currentBuffer->appendByte(MAPL_BYTE_BITWISE_NEGATION);
                        compileNode(expression->expression(0), expectedType, currentBuffer);
                        break;
                    case MaPLParser::SUBTRACT: {
                        std::vector<MaPLParser::ExpressionContext *> expressions = expression->expression();
                        if (expressions.size() == 1) {
                            // This is numeric negation (instead of subtraction).
                            currentBuffer->appendByte(MAPL_BYTE_NUMERIC_NEGATION);
                            compileNode(expressions[0], expectedType, currentBuffer);
                            break;
                        }
                    }
                        // Intentional fallthrough handles the binary subtraction operator.
                    case MaPLParser::ADD: // Intentional fallthrough.
                    case MaPLParser::MOD: // Intentional fallthrough.
                    case MaPLParser::MULTIPLY: // Intentional fallthrough.
                    case MaPLParser::DIVIDE:
                        switch (tokenType) {
                            case MaPLParser::ADD:
                                if (expectedType.primitiveType == MaPLPrimitiveType_String) {
                                    currentBuffer->appendByte(MAPL_BYTE_STRING_CONCAT);
                                } else {
                                    currentBuffer->appendByte(MAPL_BYTE_NUMERIC_ADD);
                                }
                                break;
                            case MaPLParser::SUBTRACT:
                                currentBuffer->appendByte(MAPL_BYTE_NUMERIC_SUBTRACT);
                                break;
                            case MaPLParser::MOD:
                                currentBuffer->appendByte(MAPL_BYTE_NUMERIC_MODULO);
                                break;
                            case MaPLParser::MULTIPLY:
                                currentBuffer->appendByte(MAPL_BYTE_NUMERIC_MULTIPLY);
                                break;
                            case MaPLParser::DIVIDE:
                                currentBuffer->appendByte(MAPL_BYTE_NUMERIC_DIVIDE);
                                break;
                            default: break;
                        }
                        compileNode(expression->expression(0), expectedType, currentBuffer);
                        compileNode(expression->expression(1), expectedType, currentBuffer);
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
        case MaPLParser::RuleWhileLoop: {
            MaPLParser::WhileLoopContext *loop = (MaPLParser::WhileLoopContext *)node;
            MaPLParser::ExpressionContext *loopExpression = loop->expression();
            
            MaPLLiteral expressionLiteral = constantValueForExpression(loopExpression);
            if (expressionLiteral.type.primitiveType == MaPLPrimitiveType_Boolean &&
                !expressionLiteral.booleanValue) {
                // If the literal is always false, this entire loop is dead code. Append nothing.
                break;
            }
            
            // "While" loops are represented in bytecode as follows:
            // ┌ MAPL_BYTE_CONDITIONAL - Signals the start of the loop.
            // | ExpressionContext - The boolean expression at the top of the loop.
            // └ MaPL_Index - If the boolean expression is false, this is how many bytes to skip forward to exit the loop.
            //   ScopeContext - The contents of the loop.
            // ┌ MAPL_BYTE_CURSOR_MOVE_BACK - Signals the end of the loop.
            // └ MaPL_Index - The size of the backward move required to return to the top of the loop.
            MaPLBuffer scopeBuffer(10);
            compileNode(loop->scope(), { MaPLPrimitiveType_Uninitialized }, &scopeBuffer);
            
            // If the conditional is always true, this is an infinite while loop.
            bool infiniteLoop = expressionLiteral.type.primitiveType == MaPLPrimitiveType_Boolean && expressionLiteral.booleanValue;
            MaPLBuffer loopBuffer(10);
            if (!infiniteLoop) {
                // This is not an infinite loop, so the conditional must be checked on each iteration.
                loopBuffer.appendByte(MAPL_BYTE_CONDITIONAL);
                compileNode(loopExpression, { MaPLPrimitiveType_Boolean }, &loopBuffer);
                // Scope size must also include the MAPL_BYTE_CURSOR_MOVE_BACK.
                MaPL_Index scopeSize = scopeBuffer.getByteCount() + sizeof(MaPL_Instruction) + sizeof(MaPL_Index);
                loopBuffer.appendBytes(&scopeSize, sizeof(scopeSize));
            }
            loopBuffer.appendBuffer(&scopeBuffer, 0);
            loopBuffer.appendByte(MAPL_BYTE_CURSOR_MOVE_BACK);
            MaPL_Index byteDistanceToLoopTop = loopBuffer.getByteCount() + sizeof(MaPL_Index);
            loopBuffer.appendBytes(&byteDistanceToLoopTop, sizeof(byteDistanceToLoopTop));
            
            loopBuffer.resolveControlFlowAnnotations(MaPLBufferAnnotationType_Break, true);
            loopBuffer.resolveControlFlowAnnotations(MaPLBufferAnnotationType_Continue, false);
            
            currentBuffer->appendBuffer(&loopBuffer, 0);
        }
            break;
        case MaPLParser::RuleForLoop: {
            MaPLParser::ForLoopContext *loop = (MaPLParser::ForLoopContext *)node;
            
            // "For" loops typically contain a variable declaration. There's a scope in the body of the loop,
            // but we need another for the variable declared at the top of the loop.
            _variableStack->push();
            
            // "For" loops are represented in bytecode as follows:
            //   ImperativeStatementContext - The first imperative statement at the top of the loop (typically variable declaration).
            // ┌ MAPL_BYTE_CONDITIONAL - The conditional at the top of the loop.
            // | ExpressionContext - The boolean expression at the top of the loop.
            // └ MaPL_Index - If the boolean expression is false, this is how many bytes to skip forward to exit the loop.
            //   ScopeContext - The contents of the loop.
            //   ImperativeStatementContext - The last imperative statement at the top of the loop (typically variable increment).
            // ┌ MAPL_BYTE_CURSOR_MOVE_BACK - Signals the end of the loop.
            // └ MaPL_Index - The size of the backward move required to return to the top of the loop.
            MaPLParser::ForLoopControlStatementsContext *controlStatements = loop->forLoopControlStatements();
            if (controlStatements->firstStatement) {
                compileNode(controlStatements->firstStatement, { MaPLPrimitiveType_Uninitialized }, currentBuffer);
            }
            
            MaPLParser::ExpressionContext *loopExpression = controlStatements->expression();
            MaPLLiteral expressionLiteral;
            if (loopExpression) {
                expressionLiteral = constantValueForExpression(loopExpression);
            } else {
                // If the conditional was omitted, it's equivalent to "true".
                expressionLiteral = { { MaPLPrimitiveType_Boolean } };
                expressionLiteral.booleanValue = true;
            }
            if (expressionLiteral.type.primitiveType == MaPLPrimitiveType_Boolean &&
                !expressionLiteral.booleanValue) {
                // The conditional at the top of the loop is always false. Don't append anything after the first imperative statement.
                _variableStack->pop();
                break;
            }
            
            MaPLBuffer scopeBuffer(10);
            compileNode(loop->scope(), { MaPLPrimitiveType_Uninitialized }, &scopeBuffer);
            if (controlStatements->lastStatement) {
                compileNode(controlStatements->lastStatement, { MaPLPrimitiveType_Uninitialized }, &scopeBuffer);
            }
            
            // If the conditional is always true, this is an infinite for loop.
            bool infiniteLoop = expressionLiteral.type.primitiveType == MaPLPrimitiveType_Boolean && expressionLiteral.booleanValue;
            MaPLBuffer loopBuffer(10);
            if (!infiniteLoop) {
                loopBuffer.appendByte(MAPL_BYTE_CONDITIONAL);
                compileNode(loopExpression, { MaPLPrimitiveType_Boolean }, &loopBuffer);
                // Scope size must also include the MAPL_BYTE_CURSOR_MOVE_BACK.
                MaPL_Index scopeSize = scopeBuffer.getByteCount() + sizeof(MaPL_Instruction) + sizeof(MaPL_Index);
                loopBuffer.appendBytes(&scopeSize, sizeof(scopeSize));
            }
            loopBuffer.appendBuffer(&scopeBuffer, 0);
            loopBuffer.appendByte(MAPL_BYTE_CURSOR_MOVE_BACK);
            MaPL_Index byteDistanceToLoopTop = loopBuffer.getByteCount() + sizeof(MaPL_Index);
            loopBuffer.appendBytes(&byteDistanceToLoopTop, sizeof(byteDistanceToLoopTop));
            
            loopBuffer.resolveControlFlowAnnotations(MaPLBufferAnnotationType_Break, true);
            loopBuffer.resolveControlFlowAnnotations(MaPLBufferAnnotationType_Continue, false);
            
            currentBuffer->appendBuffer(&loopBuffer, 0);
            
            _variableStack->pop();
        }
            break;
        case MaPLParser::RuleDoWhileLoop: {
            MaPLParser::DoWhileLoopContext *loop = (MaPLParser::DoWhileLoopContext *)node;
            
            // "Do while" loops are represented in bytecode as follows:
            //   ScopeContext - The contents of the loop.
            // ┌ MAPL_BYTE_CONDITIONAL - The conditional at the end of the loop.
            // | ExpressionContext - The boolean expression at the end of the loop.
            // └ MaPL_Index - If the boolean expression is false, this is how many bytes to skip forward to exit the loop.
            // ┌ MAPL_BYTE_CURSOR_MOVE_BACK - Contained within the MAPL_BYTE_CONDITIONAL, loops back to the top.
            // └ MaPL_Index - The size of the backward move required to return to the top of the loop.
            MaPLBuffer loopBuffer(10);
            compileNode(loop->scope(), { MaPLPrimitiveType_Uninitialized }, &loopBuffer);
            loopBuffer.resolveControlFlowAnnotations(MaPLBufferAnnotationType_Continue, true);
            
            MaPLParser::ExpressionContext *loopExpression = loop->expression();
            MaPLLiteral expressionLiteral = constantValueForExpression(loopExpression);
            if (expressionLiteral.type.primitiveType == MaPLPrimitiveType_Boolean &&
                !expressionLiteral.booleanValue) {
                // The conditional at the end of the loop is always false. No need to ever repeat.
                loopBuffer.resolveControlFlowAnnotations(MaPLBufferAnnotationType_Break, true);
                currentBuffer->appendBuffer(&loopBuffer, 0);
                break;
            }
            
            // If the conditional is always true, this is an infinite "do while" loop.
            bool infiniteLoop = expressionLiteral.type.primitiveType == MaPLPrimitiveType_Boolean && expressionLiteral.booleanValue;
            if (!infiniteLoop) {
                loopBuffer.appendByte(MAPL_BYTE_CONDITIONAL);
                compileNode(loopExpression, { MaPLPrimitiveType_Boolean }, &loopBuffer);
                // This is already at the end of the loop, and just needs to skip the MAPL_BYTE_CURSOR_MOVE_BACK that makes the loop repeat.
                MaPL_Index cursorMoveSize = sizeof(MaPL_Instruction) + sizeof(MaPL_Index);
                loopBuffer.appendBytes(&cursorMoveSize, sizeof(cursorMoveSize));
            }
            loopBuffer.appendByte(MAPL_BYTE_CURSOR_MOVE_BACK);
            MaPL_Index loopSize = loopBuffer.getByteCount();
            loopBuffer.appendBytes(&loopSize, sizeof(loopSize));
            
            loopBuffer.resolveControlFlowAnnotations(MaPLBufferAnnotationType_Break, true);
            
            currentBuffer->appendBuffer(&loopBuffer, 0);
        }
            break;
        case MaPLParser::RuleConditional: {
            MaPLParser::ConditionalContext *conditional = (MaPLParser::ConditionalContext *)node;
            MaPLParser::ExpressionContext *conditionalExpression = conditional->expression();
            
            MaPLLiteral expressionLiteral = constantValueForExpression(conditionalExpression);
            if (expressionLiteral.type.primitiveType == MaPLPrimitiveType_Boolean) {
                // The expression evaluated by this conditional is a compile-time constant, so it's possible to strip some dead code.
                if (expressionLiteral.booleanValue) {
                    // The conditional is always true. Unconditionally include the contents of the scope and desregard any "else" logic.
                    compileNode(conditional->scope(), { MaPLPrimitiveType_Uninitialized }, currentBuffer);
                } else {
                    // The conditional is always false. Disregard the contents of the scope and unconditionally include any "else" logic (if it exists).
                    MaPLParser::ConditionalElseContext *conditionalElse = conditional->conditionalElse();
                    if (conditionalElse) {
                        compileChildNodes(conditionalElse, { MaPLPrimitiveType_Uninitialized }, currentBuffer);
                    }
                }
            } else {
                // The expression evaluated by this conditional is not compile-time constant. Compile it normally.
                // Conditionals are represented in bytecode as follows:
                // ┌ MAPL_BYTE_CONDITIONAL - Signals the start of a conditional.
                // | ExpressionContext - The boolean expression at the top of the conditional.
                // └ MaPL_Index - If the conditional is false, this is how many bytes to skip forward to exit the conditional.
                //   ScopeContext - The contents of the conditional.
                // ┌ MAPL_BYTE_CURSOR_MOVE_FORWARD - Signals the end of conditional contents (omitted if there's no "else").
                // └ MaPL_Index - After the conditional content, how far to skip past all subsequent "else" bytes (omitted if there's no "else").
                //   ConditionalElseContext - The "else" portion of the conditional (omitted if there's no "else").
                currentBuffer->appendByte(MAPL_BYTE_CONDITIONAL);
                compileNode(conditionalExpression, { MaPLPrimitiveType_Boolean }, currentBuffer);
                
                MaPLBuffer scopeBuffer(10);
                compileNode(conditional->scope(), { MaPLPrimitiveType_Uninitialized }, &scopeBuffer);
                
                MaPLParser::ConditionalElseContext *conditionalElse = conditional->conditionalElse();
                MaPLBuffer elseBuffer(10);
                if (conditionalElse) {
                    compileChildNodes(conditionalElse, { MaPLPrimitiveType_Uninitialized }, &elseBuffer);
                }
                
                MaPL_Index elseBufferSize = (MaPL_Index)elseBuffer.getByteCount();
                if (elseBufferSize > 0) {
                    scopeBuffer.appendByte(MAPL_BYTE_CURSOR_MOVE_FORWARD);
                    scopeBuffer.appendBytes(&elseBufferSize, sizeof(elseBufferSize));
                }
                
                MaPL_Index scopeSize = (MaPL_Index)scopeBuffer.getByteCount();
                currentBuffer->appendBytes(&scopeSize, sizeof(scopeSize));
                
                currentBuffer->appendBuffer(&scopeBuffer, 0);
                currentBuffer->appendBuffer(&elseBuffer, 0);
            }
        }
            break;
        case MaPLParser::RuleScope:
            _variableStack->push();
            compileChildNodes(node, { MaPLPrimitiveType_Uninitialized }, currentBuffer);
            _variableStack->pop();
            break;
        default:
            break;
    }
}

MaPLPrimitiveType MaPLFile::typeReconciliationError(MaPLPrimitiveType left,
                                                    MaPLPrimitiveType right,
                                                    antlr4::Token *errorToken) {
    if (errorToken) {
        logError(this, errorToken, "Type mismatch. Cannot combine "+descriptorForPrimitive(left)+" and "+descriptorForPrimitive(right)+" types in this way.");
    }
    return MaPLPrimitiveType_TypeError;
}

void MaPLFile::missingTypeError(antlr4::Token *errorToken, const std::string &typeName) {
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
        return typeReconciliationError(left, right, errorToken);
    }
    // This function takes a pair of types, which potentially contain ambiguity due
    // to the values being specified as literals instead of variable declarations,
    // and resolves them down to the most specific type information that can be inferred.
    // For example, the expression "3+4.5" should be interpreted as a pair of floating
    // point numbers, even though the number on the left looks like an integer literal.
    if (isAmbiguousNumericType(left) || isAmbiguousNumericType(right)) {
        // Any ambiguous number can be stored in a float.
        if (isConcreteFloat(left)) { return left; }
        if (isConcreteFloat(right)) { return right; }
        
        // Signed integers can store any ambiguous non-float numbers.
        if (isConcreteSignedInt(left)) {
            if (right == MaPLPrimitiveType_Float_AmbiguousSize) {
                return typeReconciliationError(left, right, errorToken);
            }
            return left;
        }
        if (isConcreteSignedInt(right)) {
            if (left == MaPLPrimitiveType_Float_AmbiguousSize) {
                return typeReconciliationError(left, right, errorToken);
            }
            return right;
        }
        
        // Unsigned integers can store only ambiguously-sized, unsigned numbers.
        if (isConcreteUnsignedInt(left)) {
            if (right == MaPLPrimitiveType_Int_AmbiguousSizeAndSign) {
                return left;
            }
            return typeReconciliationError(left, right, errorToken);
        }
        if (isConcreteUnsignedInt(right)) {
            if (left == MaPLPrimitiveType_Int_AmbiguousSizeAndSign) {
                return right;
            }
            return typeReconciliationError(left, right, errorToken);
        }
        
        // All pairings containing a specific type have been handled above. All remaining are some combination of ambiguous types.
        if (left == MaPLPrimitiveType_Float_AmbiguousSize || right == MaPLPrimitiveType_Float_AmbiguousSize) {
            // All ambigous integer types can convert to float.
            return MaPLPrimitiveType_Float_AmbiguousSize;
        }
        // Only permutation remaining is SignedInt_AmbiguousSize and Int_AmbiguousSizeAndSign.
        return MaPLPrimitiveType_SignedInt_AmbiguousSize;
    }
    return typeReconciliationError(left, right, errorToken);
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
        logError(this, errorToken, "The return type of this operator is ambiguous and cannot be determined. Expressions in both branches of the conditional must have a matching type. "+errorSuffix);
        return { MaPLPrimitiveType_TypeError };
    }
    return { reconciledPrimitive, type1.pointerType };
}

MaPLLiteral MaPLFile::constantValueForExpression(MaPLParser::ExpressionContext *expression) {
    if (expression->keyToken) {
        size_t tokenType = expression->keyToken->getType();
        switch (tokenType) {
            case MaPLParser::PAREN_OPEN: { // Typecast.
                MaPLLiteral literal = castLiteralToType(constantValueForExpression(expression->expression(0)),
                                                        typeForTypeContext(expression->type()));
                if (literal.type.primitiveType != MaPLPrimitiveType_TypeError) {
                    return literal;
                }
            }
            case MaPLParser::PAREN_CLOSE: // Nested expression.
                return constantValueForExpression(expression->expression(0));
            case MaPLParser::LOGICAL_AND: {
                MaPLLiteral left = constantValueForExpression(expression->expression(0));
                if (left.type.primitiveType == MaPLPrimitiveType_Boolean && !left.booleanValue) {
                    return left;
                }
                MaPLLiteral right = constantValueForExpression(expression->expression(1));
                if (right.type.primitiveType == MaPLPrimitiveType_Boolean && !right.booleanValue) {
                    return right;
                }
                if (left.type.primitiveType == MaPLPrimitiveType_Boolean &&
                    right.type.primitiveType == MaPLPrimitiveType_Boolean) {
                    MaPLLiteral literal = { { MaPLPrimitiveType_Boolean } };
                    literal.booleanValue = left.booleanValue && right.booleanValue;
                    return literal;
                }
            }
                break;
            case MaPLParser::LOGICAL_OR: {
                MaPLLiteral left = constantValueForExpression(expression->expression(0));
                if (left.type.primitiveType == MaPLPrimitiveType_Boolean && left.booleanValue) {
                    return left;
                }
                MaPLLiteral right = constantValueForExpression(expression->expression(1));
                if (right.type.primitiveType == MaPLPrimitiveType_Boolean && right.booleanValue) {
                    return right;
                }
                if (left.type.primitiveType == MaPLPrimitiveType_Boolean &&
                    right.type.primitiveType == MaPLPrimitiveType_Boolean) {
                    MaPLLiteral literal = { { MaPLPrimitiveType_Boolean } };
                    literal.booleanValue = left.booleanValue || right.booleanValue;
                    return literal;
                }
            }
                break;
            case MaPLParser::LOGICAL_EQUALITY: // Intentional fallthrough.
            case MaPLParser::LOGICAL_INEQUALITY: {
                MaPLLiteral left = constantValueForExpression(expression->expression(0));
                if (left.type.primitiveType != MaPLPrimitiveType_Boolean) { break; }
                MaPLLiteral right = constantValueForExpression(expression->expression(1));
                if (right.type.primitiveType != MaPLPrimitiveType_Boolean) { break; }
                MaPLLiteral literal = { { MaPLPrimitiveType_Boolean } };
                switch (tokenType) {
                    case MaPLParser::LOGICAL_EQUALITY:
                        literal.booleanValue = left.booleanValue == right.booleanValue;
                        return literal;
                    case MaPLParser::LOGICAL_INEQUALITY:
                        literal.booleanValue = left.booleanValue != right.booleanValue;
                        return literal;
                    default: break;
                }
            }
            case MaPLParser::LESS_THAN: // Intentional fallthrough.
            case MaPLParser::LESS_THAN_EQUAL: // Intentional fallthrough.
            case MaPLParser::GREATER_THAN: // Intentional fallthrough.
            case MaPLParser::GREATER_THAN_EQUAL: {
                MaPLLiteral left = constantValueForExpression(expression->expression(0));
                if (!isNumeric(left.type.primitiveType)) { break; }
                MaPLLiteral right = constantValueForExpression(expression->expression(1));
                if (!isNumeric(right.type.primitiveType)) { break; }
                
                // Because literals can be ambiguous, types must be reconciled and typecast.
                MaPLType reconciledType = { reconcileTypes(left.type.primitiveType, right.type.primitiveType, NULL) };
                if (!isNumeric(reconciledType.primitiveType)) { break; }
                left = castLiteralToType(left, reconciledType);
                right = castLiteralToType(right, reconciledType);
                
                MaPLLiteral returnVal = { { MaPLPrimitiveType_Boolean } };
                switch (reconciledType.primitiveType) {
                    case MaPLPrimitiveType_Int8:
                        switch (tokenType) {
                            case MaPLParser::LESS_THAN:
                                returnVal.booleanValue = left.int8Value < right.int8Value;
                                return returnVal;
                            case MaPLParser::LESS_THAN_EQUAL:
                                returnVal.booleanValue = left.int8Value <= right.int8Value;
                                return returnVal;
                            case MaPLParser::GREATER_THAN:
                                returnVal.booleanValue = left.int8Value > right.int8Value;
                                return returnVal;
                            case MaPLParser::GREATER_THAN_EQUAL:
                                returnVal.booleanValue = left.int8Value >= right.int8Value;
                                return returnVal;
                            default: break;
                        }
                        break;
                    case MaPLPrimitiveType_Int16:
                        switch (tokenType) {
                            case MaPLParser::LESS_THAN:
                                returnVal.booleanValue = left.int16Value < right.int16Value;
                                return returnVal;
                            case MaPLParser::LESS_THAN_EQUAL:
                                returnVal.booleanValue = left.int16Value <= right.int16Value;
                                return returnVal;
                            case MaPLParser::GREATER_THAN:
                                returnVal.booleanValue = left.int16Value > right.int16Value;
                                return returnVal;
                            case MaPLParser::GREATER_THAN_EQUAL:
                                returnVal.booleanValue = left.int16Value >= right.int16Value;
                                return returnVal;
                            default: break;
                        }
                        break;
                    case MaPLPrimitiveType_Int32:
                        switch (tokenType) {
                            case MaPLParser::LESS_THAN:
                                returnVal.booleanValue = left.int32Value < right.int32Value;
                                return returnVal;
                            case MaPLParser::LESS_THAN_EQUAL:
                                returnVal.booleanValue = left.int32Value <= right.int32Value;
                                return returnVal;
                            case MaPLParser::GREATER_THAN:
                                returnVal.booleanValue = left.int32Value > right.int32Value;
                                return returnVal;
                            case MaPLParser::GREATER_THAN_EQUAL:
                                returnVal.booleanValue = left.int32Value >= right.int32Value;
                                return returnVal;
                            default: break;
                        }
                        break;
                    case MaPLPrimitiveType_Int64: // Intentional fallthrough.
                    case MaPLPrimitiveType_SignedInt_AmbiguousSize:
                        switch (tokenType) {
                            case MaPLParser::LESS_THAN:
                                returnVal.booleanValue = left.int64Value < right.int64Value;
                                return returnVal;
                            case MaPLParser::LESS_THAN_EQUAL:
                                returnVal.booleanValue = left.int64Value <= right.int64Value;
                                return returnVal;
                            case MaPLParser::GREATER_THAN:
                                returnVal.booleanValue = left.int64Value > right.int64Value;
                                return returnVal;
                            case MaPLParser::GREATER_THAN_EQUAL:
                                returnVal.booleanValue = left.int64Value >= right.int64Value;
                                return returnVal;
                            default: break;
                        }
                        break;
                    case MaPLPrimitiveType_UInt8:
                        switch (tokenType) {
                            case MaPLParser::LESS_THAN:
                                returnVal.booleanValue = left.uInt8Value < right.uInt8Value;
                                return returnVal;
                            case MaPLParser::LESS_THAN_EQUAL:
                                returnVal.booleanValue = left.uInt8Value <= right.uInt8Value;
                                return returnVal;
                            case MaPLParser::GREATER_THAN:
                                returnVal.booleanValue = left.uInt8Value > right.uInt8Value;
                                return returnVal;
                            case MaPLParser::GREATER_THAN_EQUAL:
                                returnVal.booleanValue = left.uInt8Value >= right.uInt8Value;
                                return returnVal;
                            default: break;
                        }
                        break;
                    case MaPLPrimitiveType_UInt16:
                        switch (tokenType) {
                            case MaPLParser::LESS_THAN:
                                returnVal.booleanValue = left.uInt16Value < right.uInt16Value;
                                return returnVal;
                            case MaPLParser::LESS_THAN_EQUAL:
                                returnVal.booleanValue = left.uInt16Value <= right.uInt16Value;
                                return returnVal;
                            case MaPLParser::GREATER_THAN:
                                returnVal.booleanValue = left.uInt16Value > right.uInt16Value;
                                return returnVal;
                            case MaPLParser::GREATER_THAN_EQUAL:
                                returnVal.booleanValue = left.uInt16Value >= right.uInt16Value;
                                return returnVal;
                            default: break;
                        }
                        break;
                    case MaPLPrimitiveType_UInt32:
                        switch (tokenType) {
                            case MaPLParser::LESS_THAN:
                                returnVal.booleanValue = left.uInt32Value < right.uInt32Value;
                                return returnVal;
                            case MaPLParser::LESS_THAN_EQUAL:
                                returnVal.booleanValue = left.uInt32Value <= right.uInt32Value;
                                return returnVal;
                            case MaPLParser::GREATER_THAN:
                                returnVal.booleanValue = left.uInt32Value > right.uInt32Value;
                                return returnVal;
                            case MaPLParser::GREATER_THAN_EQUAL:
                                returnVal.booleanValue = left.uInt32Value >= right.uInt32Value;
                                return returnVal;
                            default: break;
                        }
                        break;
                    case MaPLPrimitiveType_UInt64: // Intentional fallthrough.
                    case MaPLPrimitiveType_Int_AmbiguousSizeAndSign:
                        switch (tokenType) {
                            case MaPLParser::LESS_THAN:
                                returnVal.booleanValue = left.uInt64Value < right.uInt64Value;
                                return returnVal;
                            case MaPLParser::LESS_THAN_EQUAL:
                                returnVal.booleanValue = left.uInt64Value <= right.uInt64Value;
                                return returnVal;
                            case MaPLParser::GREATER_THAN:
                                returnVal.booleanValue = left.uInt64Value > right.uInt64Value;
                                return returnVal;
                            case MaPLParser::GREATER_THAN_EQUAL:
                                returnVal.booleanValue = left.uInt64Value >= right.uInt64Value;
                                return returnVal;
                            default: break;
                        }
                        break;
                    case MaPLPrimitiveType_Float32:
                        switch (tokenType) {
                            case MaPLParser::LESS_THAN:
                                returnVal.booleanValue = left.float32Value < right.float32Value;
                                return returnVal;
                            case MaPLParser::LESS_THAN_EQUAL:
                                returnVal.booleanValue = left.float32Value <= right.float32Value;
                                return returnVal;
                            case MaPLParser::GREATER_THAN:
                                returnVal.booleanValue = left.float32Value > right.float32Value;
                                return returnVal;
                            case MaPLParser::GREATER_THAN_EQUAL:
                                returnVal.booleanValue = left.float32Value >= right.float32Value;
                                return returnVal;
                            default: break;
                        }
                        break;
                    case MaPLPrimitiveType_Float64: // Intentional fallthrough.
                    case MaPLPrimitiveType_Float_AmbiguousSize:
                        switch (tokenType) {
                            case MaPLParser::LESS_THAN:
                                returnVal.booleanValue = left.float64Value < right.float64Value;
                                return returnVal;
                            case MaPLParser::LESS_THAN_EQUAL:
                                returnVal.booleanValue = left.float64Value <= right.float64Value;
                                return returnVal;
                            case MaPLParser::GREATER_THAN:
                                returnVal.booleanValue = left.float64Value > right.float64Value;
                                return returnVal;
                            case MaPLParser::GREATER_THAN_EQUAL:
                                returnVal.booleanValue = left.float64Value >= right.float64Value;
                                return returnVal;
                            default: break;
                        }
                        break;
                    default: break;
                }
            }
                break;
            case MaPLParser::LOGICAL_NEGATION: {
                MaPLLiteral literal = constantValueForExpression(expression->expression(0));
                if (literal.type.primitiveType != MaPLPrimitiveType_Boolean) { break; }
                literal.booleanValue = !literal.booleanValue;
                return literal;
            }
            case MaPLParser::LITERAL_TRUE: // Intentional fallthrough.
            case MaPLParser::LITERAL_FALSE: {
                MaPLLiteral literal = { { MaPLPrimitiveType_Boolean } };
                literal.booleanValue = tokenType == MaPLParser::LITERAL_TRUE;
                return  literal;
            }
            case MaPLParser::SUBTRACT: {
                std::vector<MaPLParser::ExpressionContext *> childExpressions = expression->expression();
                if (childExpressions.size() == 1) {
                    // There's only one operand, so this is numeric negation instead of subtraction.
                    MaPLLiteral literal = constantValueForExpression(childExpressions[0]);
                    switch (literal.type.primitiveType) {
                        case MaPLPrimitiveType_Int8:
                            literal.int8Value = -literal.int8Value;
                            return literal;
                        case MaPLPrimitiveType_Int16:
                            literal.int16Value = -literal.int16Value;
                            return literal;
                        case MaPLPrimitiveType_Int32:
                            literal.int32Value = -literal.int32Value;
                            return literal;
                        case MaPLPrimitiveType_Int64: // Intentional fallthrough.
                        case MaPLPrimitiveType_SignedInt_AmbiguousSize:
                            literal.int64Value = -literal.int64Value;
                            return literal;
                        case MaPLPrimitiveType_Float32:
                            literal.float32Value = -literal.float32Value;
                            return literal;
                        case MaPLPrimitiveType_Float64:
                        case MaPLPrimitiveType_Float_AmbiguousSize:
                            literal.float64Value = -literal.float64Value;
                            return literal;
                        case MaPLPrimitiveType_Int_AmbiguousSizeAndSign:
                            literal = castLiteralToType(literal, { MaPLPrimitiveType_SignedInt_AmbiguousSize });
                            literal.int64Value = -literal.int64Value;
                            return literal;
                        default: break;
                    }
                    break;
                }
            }
                // Intentional fallthrough handles the binary subtraction operator.
            case MaPLParser::MOD: // Intentional fallthrough.
            case MaPLParser::MULTIPLY: // Intentional fallthrough.
            case MaPLParser::DIVIDE: {
                MaPLLiteral left = constantValueForExpression(expression->expression(0));
                if (!isNumeric(left.type.primitiveType)) { break; }
                MaPLLiteral right = constantValueForExpression(expression->expression(1));
                if (!isNumeric(right.type.primitiveType)) { break; }
                
                // Because literals can be ambiguous, types must be reconciled and typecast.
                MaPLType reconciledType = { reconcileTypes(left.type.primitiveType, right.type.primitiveType, NULL) };
                if (!isNumeric(reconciledType.primitiveType)) { break; }
                left = castLiteralToType(left, reconciledType);
                right = castLiteralToType(right, reconciledType);
                
                MaPLLiteral returnVal = { reconciledType };
                switch (reconciledType.primitiveType) {
                    case MaPLPrimitiveType_Int8:
                        switch (tokenType) {
                            case MaPLParser::SUBTRACT:
                                returnVal.int8Value = left.int8Value - right.int8Value;
                                return returnVal;
                            case MaPLParser::MOD:
                                returnVal.int8Value = left.int8Value % right.int8Value;
                                return returnVal;
                            case MaPLParser::MULTIPLY:
                                returnVal.int8Value = left.int8Value * right.int8Value;
                                return returnVal;
                            case MaPLParser::DIVIDE:
                                returnVal.int8Value = left.int8Value / right.int8Value;
                                return returnVal;
                            default: break;
                        }
                        break;
                    case MaPLPrimitiveType_Int16:
                        switch (tokenType) {
                            case MaPLParser::SUBTRACT:
                                returnVal.int16Value = left.int16Value - right.int16Value;
                                return returnVal;
                            case MaPLParser::MOD:
                                returnVal.int16Value = left.int16Value % right.int16Value;
                                return returnVal;
                            case MaPLParser::MULTIPLY:
                                returnVal.int16Value = left.int16Value * right.int16Value;
                                return returnVal;
                            case MaPLParser::DIVIDE:
                                returnVal.int16Value = left.int16Value / right.int16Value;
                                return returnVal;
                            default: break;
                        }
                        break;
                    case MaPLPrimitiveType_Int32:
                        switch (tokenType) {
                            case MaPLParser::SUBTRACT:
                                returnVal.int32Value = left.int32Value - right.int32Value;
                                return returnVal;
                            case MaPLParser::MOD:
                                returnVal.int32Value = left.int32Value % right.int32Value;
                                return returnVal;
                            case MaPLParser::MULTIPLY:
                                returnVal.int32Value = left.int32Value * right.int32Value;
                                return returnVal;
                            case MaPLParser::DIVIDE:
                                returnVal.int32Value = left.int32Value / right.int32Value;
                                return returnVal;
                            default: break;
                        }
                        break;
                    case MaPLPrimitiveType_Int64: // Intentional fallthrough.
                    case MaPLPrimitiveType_SignedInt_AmbiguousSize:
                        switch (tokenType) {
                            case MaPLParser::SUBTRACT:
                                returnVal.int64Value = left.int64Value - right.int64Value;
                                return returnVal;
                            case MaPLParser::MOD:
                                returnVal.int64Value = left.int64Value % right.int64Value;
                                return returnVal;
                            case MaPLParser::MULTIPLY:
                                returnVal.int64Value = left.int64Value * right.int64Value;
                                return returnVal;
                            case MaPLParser::DIVIDE:
                                returnVal.int64Value = left.int64Value / right.int64Value;
                                return returnVal;
                            default: break;
                        }
                        break;
                    case MaPLPrimitiveType_UInt8:
                        switch (tokenType) {
                            case MaPLParser::SUBTRACT:
                                returnVal.uInt8Value = left.uInt8Value - right.uInt8Value;
                                return returnVal;
                            case MaPLParser::MOD:
                                returnVal.uInt8Value = left.uInt8Value % right.uInt8Value;
                                return returnVal;
                            case MaPLParser::MULTIPLY:
                                returnVal.uInt8Value = left.uInt8Value * right.uInt8Value;
                                return returnVal;
                            case MaPLParser::DIVIDE:
                                returnVal.uInt8Value = left.uInt8Value / right.uInt8Value;
                                return returnVal;
                            default: break;
                        }
                        break;
                    case MaPLPrimitiveType_UInt16:
                        switch (tokenType) {
                            case MaPLParser::SUBTRACT:
                                returnVal.uInt16Value = left.uInt16Value - right.uInt16Value;
                                return returnVal;
                            case MaPLParser::MOD:
                                returnVal.uInt16Value = left.uInt16Value % right.uInt16Value;
                                return returnVal;
                            case MaPLParser::MULTIPLY:
                                returnVal.uInt16Value = left.uInt16Value * right.uInt16Value;
                                return returnVal;
                            case MaPLParser::DIVIDE:
                                returnVal.uInt16Value = left.uInt16Value / right.uInt16Value;
                                return returnVal;
                            default: break;
                        }
                        break;
                    case MaPLPrimitiveType_UInt32:
                        switch (tokenType) {
                            case MaPLParser::SUBTRACT:
                                returnVal.uInt32Value = left.uInt32Value - right.uInt32Value;
                                return returnVal;
                            case MaPLParser::MOD:
                                returnVal.uInt32Value = left.uInt32Value % right.uInt32Value;
                                return returnVal;
                            case MaPLParser::MULTIPLY:
                                returnVal.uInt32Value = left.uInt32Value * right.uInt32Value;
                                return returnVal;
                            case MaPLParser::DIVIDE:
                                returnVal.uInt32Value = left.uInt32Value / right.uInt32Value;
                                return returnVal;
                            default: break;
                        }
                        break;
                    case MaPLPrimitiveType_UInt64: // Intentional fallthrough.
                    case MaPLPrimitiveType_Int_AmbiguousSizeAndSign:
                        switch (tokenType) {
                            case MaPLParser::SUBTRACT:
                                returnVal.uInt64Value = left.uInt64Value - right.uInt64Value;
                                return returnVal;
                            case MaPLParser::MOD:
                                returnVal.uInt64Value = left.uInt64Value % right.uInt64Value;
                                return returnVal;
                            case MaPLParser::MULTIPLY:
                                returnVal.uInt64Value = left.uInt64Value * right.uInt64Value;
                                return returnVal;
                            case MaPLParser::DIVIDE:
                                returnVal.uInt64Value = left.uInt64Value / right.uInt64Value;
                                return returnVal;
                            default: break;
                        }
                        break;
                    case MaPLPrimitiveType_Float32:
                        switch (tokenType) {
                            case MaPLParser::SUBTRACT:
                                returnVal.float32Value = left.float32Value - right.float32Value;
                                return returnVal;
                            case MaPLParser::MOD:
                                returnVal.float32Value = fmod(left.float32Value, right.float32Value);
                                return returnVal;
                            case MaPLParser::MULTIPLY:
                                returnVal.float32Value = left.float32Value * right.float32Value;
                                return returnVal;
                            case MaPLParser::DIVIDE:
                                returnVal.float32Value = left.float32Value / right.float32Value;
                                return returnVal;
                            default: break;
                        }
                        break;
                    case MaPLPrimitiveType_Float64: // Intentional fallthrough.
                    case MaPLPrimitiveType_Float_AmbiguousSize:
                        switch (tokenType) {
                            case MaPLParser::SUBTRACT:
                                returnVal.float64Value = left.float64Value - right.float64Value;
                                return returnVal;
                            case MaPLParser::MOD:
                                returnVal.float64Value = fmod(left.float64Value, right.float64Value);
                                return returnVal;
                            case MaPLParser::MULTIPLY:
                                returnVal.float64Value = left.float64Value * right.float64Value;
                                return returnVal;
                            case MaPLParser::DIVIDE:
                                returnVal.float64Value = left.float64Value / right.float64Value;
                                return returnVal;
                            default: break;
                        }
                        break;
                    default: break;
                }
            }
                break;
            case MaPLParser::ADD: {
                MaPLLiteral left = constantValueForExpression(expression->expression(0));
                if (!isNumeric(left.type.primitiveType) && left.type.primitiveType != MaPLPrimitiveType_String) { break; }
                MaPLLiteral right = constantValueForExpression(expression->expression(1));
                if (!isNumeric(right.type.primitiveType) && right.type.primitiveType != MaPLPrimitiveType_String) { break; }
                
                // Because literals can be ambiguous, types must be reconciled and typecast.
                MaPLType reconciledType = { reconcileTypes(left.type.primitiveType, right.type.primitiveType, NULL) };
                if (!isNumeric(reconciledType.primitiveType) && reconciledType.primitiveType != MaPLPrimitiveType_String) { break; }
                left = castLiteralToType(left, reconciledType);
                right = castLiteralToType(right, reconciledType);
                
                MaPLLiteral returnVal = { reconciledType };
                switch (reconciledType.primitiveType) {
                    case MaPLPrimitiveType_Int8:
                        returnVal.int8Value = left.int8Value + right.int8Value;
                        return returnVal;
                    case MaPLPrimitiveType_Int16:
                        returnVal.int16Value = left.int16Value + right.int16Value;
                        return returnVal;
                    case MaPLPrimitiveType_Int32:
                        returnVal.int32Value = left.int32Value + right.int32Value;
                        return returnVal;
                    case MaPLPrimitiveType_Int64:
                    case MaPLPrimitiveType_SignedInt_AmbiguousSize:
                        returnVal.int64Value = left.int64Value + right.int64Value;
                        return returnVal;
                    case MaPLPrimitiveType_UInt8:
                        returnVal.uInt8Value = left.uInt8Value + right.uInt8Value;
                        return returnVal;
                    case MaPLPrimitiveType_UInt16:
                        returnVal.uInt16Value = left.uInt16Value + right.uInt16Value;
                        return returnVal;
                    case MaPLPrimitiveType_UInt32:
                        returnVal.uInt32Value = left.uInt32Value + right.uInt32Value;
                        return returnVal;
                    case MaPLPrimitiveType_UInt64:
                    case MaPLPrimitiveType_Int_AmbiguousSizeAndSign:
                        returnVal.uInt64Value = left.uInt64Value + right.uInt64Value;
                        return returnVal;
                    case MaPLPrimitiveType_Float32:
                        returnVal.float32Value = left.float32Value + right.float32Value;
                        return returnVal;
                    case MaPLPrimitiveType_Float64:
                    case MaPLPrimitiveType_Float_AmbiguousSize:
                        returnVal.float64Value = left.float64Value + right.float64Value;
                        return returnVal;
                    case MaPLPrimitiveType_String:
                        returnVal.stringValue = left.stringValue + right.stringValue;
                        return returnVal;
                    default: break;
                }
            }
                break;
            case MaPLParser::BITWISE_AND: // Intentional fallthrough.
            case MaPLParser::BITWISE_XOR: // Intentional fallthrough.
            case MaPLParser::BITWISE_OR: // Intentional fallthrough.
            case MaPLParser::BITWISE_SHIFT_LEFT: // Intentional fallthrough.
            case MaPLParser::BITWISE_SHIFT_RIGHT: {
                MaPLLiteral left = constantValueForExpression(expression->expression(0));
                if (!isIntegral(left.type.primitiveType)) { break; }
                MaPLLiteral right = constantValueForExpression(expression->expression(1));
                if (!isIntegral(right.type.primitiveType)) { break; }
                
                // Because literals can be ambiguous, types must be reconciled and typecast.
                MaPLType reconciledType = { reconcileTypes(left.type.primitiveType, right.type.primitiveType, NULL) };
                if (!isIntegral(reconciledType.primitiveType)) { break; }
                left = castLiteralToType(left, reconciledType);
                right = castLiteralToType(right, reconciledType);
                
                MaPLLiteral returnVal = { reconciledType };
                switch (reconciledType.primitiveType) {
                    case MaPLPrimitiveType_Int8:
                        switch (tokenType) {
                            case MaPLParser::BITWISE_AND:
                                returnVal.int8Value = left.int8Value & right.int8Value;
                                return returnVal;
                            case MaPLParser::BITWISE_XOR:
                                returnVal.int8Value = left.int8Value ^ right.int8Value;
                                return returnVal;
                            case MaPLParser::BITWISE_OR:
                                returnVal.int8Value = left.int8Value | right.int8Value;
                                return returnVal;
                            case MaPLParser::BITWISE_SHIFT_LEFT:
                                returnVal.int8Value = left.int8Value << right.int8Value;
                                return returnVal;
                            case MaPLParser::BITWISE_SHIFT_RIGHT:
                                returnVal.int8Value = left.int8Value >> right.int8Value;
                                return returnVal;
                            default: break;
                        }
                        break;
                    case MaPLPrimitiveType_Int16:
                        switch (tokenType) {
                            case MaPLParser::BITWISE_AND:
                                returnVal.int16Value = left.int16Value & right.int16Value;
                                return returnVal;
                            case MaPLParser::BITWISE_XOR:
                                returnVal.int16Value = left.int16Value ^ right.int16Value;
                                return returnVal;
                            case MaPLParser::BITWISE_OR:
                                returnVal.int16Value = left.int16Value | right.int16Value;
                                return returnVal;
                            case MaPLParser::BITWISE_SHIFT_LEFT:
                                returnVal.int16Value = left.int16Value << right.int16Value;
                                return returnVal;
                            case MaPLParser::BITWISE_SHIFT_RIGHT:
                                returnVal.int16Value = left.int16Value >> right.int16Value;
                                return returnVal;
                            default: break;
                        }
                        break;
                    case MaPLPrimitiveType_Int32:
                        switch (tokenType) {
                            case MaPLParser::BITWISE_AND:
                                returnVal.int32Value = left.int32Value & right.int32Value;
                                return returnVal;
                            case MaPLParser::BITWISE_XOR:
                                returnVal.int32Value = left.int32Value ^ right.int32Value;
                                return returnVal;
                            case MaPLParser::BITWISE_OR:
                                returnVal.int32Value = left.int32Value | right.int32Value;
                                return returnVal;
                            case MaPLParser::BITWISE_SHIFT_LEFT:
                                returnVal.int32Value = left.int32Value << right.int32Value;
                                return returnVal;
                            case MaPLParser::BITWISE_SHIFT_RIGHT:
                                returnVal.int32Value = left.int32Value >> right.int32Value;
                                return returnVal;
                            default: break;
                        }
                        break;
                    case MaPLPrimitiveType_Int64: // Intentional fallthrough.
                    case MaPLPrimitiveType_SignedInt_AmbiguousSize:
                        switch (tokenType) {
                            case MaPLParser::BITWISE_AND:
                                returnVal.int64Value = left.int64Value & right.int64Value;
                                return returnVal;
                            case MaPLParser::BITWISE_XOR:
                                returnVal.int64Value = left.int64Value ^ right.int64Value;
                                return returnVal;
                            case MaPLParser::BITWISE_OR:
                                returnVal.int64Value = left.int64Value | right.int64Value;
                                return returnVal;
                            case MaPLParser::BITWISE_SHIFT_LEFT:
                                returnVal.int64Value = left.int64Value << right.int64Value;
                                return returnVal;
                            case MaPLParser::BITWISE_SHIFT_RIGHT:
                                returnVal.int64Value = left.int64Value >> right.int64Value;
                                return returnVal;
                            default: break;
                        }
                        break;
                    case MaPLPrimitiveType_UInt8:
                        switch (tokenType) {
                            case MaPLParser::BITWISE_AND:
                                returnVal.uInt8Value = left.uInt8Value & right.uInt8Value;
                                return returnVal;
                            case MaPLParser::BITWISE_XOR:
                                returnVal.uInt8Value = left.uInt8Value ^ right.uInt8Value;
                                return returnVal;
                            case MaPLParser::BITWISE_OR:
                                returnVal.uInt8Value = left.uInt8Value | right.uInt8Value;
                                return returnVal;
                            case MaPLParser::BITWISE_SHIFT_LEFT:
                                returnVal.uInt8Value = left.uInt8Value << right.uInt8Value;
                                return returnVal;
                            case MaPLParser::BITWISE_SHIFT_RIGHT:
                                returnVal.uInt8Value = left.uInt8Value >> right.uInt8Value;
                                return returnVal;
                            default: break;
                        }
                        break;
                    case MaPLPrimitiveType_UInt16:
                        switch (tokenType) {
                            case MaPLParser::BITWISE_AND:
                                returnVal.uInt16Value = left.uInt16Value & right.uInt16Value;
                                return returnVal;
                            case MaPLParser::BITWISE_XOR:
                                returnVal.uInt16Value = left.uInt16Value ^ right.uInt16Value;
                                return returnVal;
                            case MaPLParser::BITWISE_OR:
                                returnVal.uInt16Value = left.uInt16Value | right.uInt16Value;
                                return returnVal;
                            case MaPLParser::BITWISE_SHIFT_LEFT:
                                returnVal.uInt16Value = left.uInt16Value << right.uInt16Value;
                                return returnVal;
                            case MaPLParser::BITWISE_SHIFT_RIGHT:
                                returnVal.uInt16Value = left.uInt16Value >> right.uInt16Value;
                                return returnVal;
                            default: break;
                        }
                        break;
                    case MaPLPrimitiveType_UInt32:
                        switch (tokenType) {
                            case MaPLParser::BITWISE_AND:
                                returnVal.uInt32Value = left.uInt32Value & right.uInt32Value;
                                return returnVal;
                            case MaPLParser::BITWISE_XOR:
                                returnVal.uInt32Value = left.uInt32Value ^ right.uInt32Value;
                                return returnVal;
                            case MaPLParser::BITWISE_OR:
                                returnVal.uInt32Value = left.uInt32Value | right.uInt32Value;
                                return returnVal;
                            case MaPLParser::BITWISE_SHIFT_LEFT:
                                returnVal.uInt32Value = left.uInt32Value << right.uInt32Value;
                                return returnVal;
                            case MaPLParser::BITWISE_SHIFT_RIGHT:
                                returnVal.uInt32Value = left.uInt32Value >> right.uInt32Value;
                                return returnVal;
                            default: break;
                        }
                        break;
                    case MaPLPrimitiveType_UInt64: // Intentional fallthrough.
                    case MaPLPrimitiveType_Int_AmbiguousSizeAndSign:
                        switch (tokenType) {
                            case MaPLParser::BITWISE_AND:
                                returnVal.uInt64Value = left.uInt64Value & right.uInt64Value;
                                return returnVal;
                            case MaPLParser::BITWISE_XOR:
                                returnVal.uInt64Value = left.uInt64Value ^ right.uInt64Value;
                                return returnVal;
                            case MaPLParser::BITWISE_OR:
                                returnVal.uInt64Value = left.uInt64Value | right.uInt64Value;
                                return returnVal;
                            case MaPLParser::BITWISE_SHIFT_LEFT:
                                returnVal.uInt64Value = left.uInt64Value << right.uInt64Value;
                                return returnVal;
                            case MaPLParser::BITWISE_SHIFT_RIGHT:
                                returnVal.uInt64Value = left.uInt64Value >> right.uInt64Value;
                                return returnVal;
                            default: break;
                        }
                        break;
                    default: break;
                }
            }
            case MaPLParser::BITWISE_NEGATION: {
                MaPLLiteral literal = constantValueForExpression(expression->expression(0));
                switch (literal.type.primitiveType) {
                    case MaPLPrimitiveType_Int8:
                        literal.int8Value = ~literal.int8Value;
                        return literal;
                    case MaPLPrimitiveType_Int16:
                        literal.int16Value = ~literal.int16Value;
                        return literal;
                    case MaPLPrimitiveType_Int32:
                        literal.int32Value = ~literal.int32Value;
                        return literal;
                    case MaPLPrimitiveType_Int64: // Intentional fallthrough.
                    case MaPLPrimitiveType_SignedInt_AmbiguousSize:
                        literal.int64Value = ~literal.int64Value;
                        return literal;
                    case MaPLPrimitiveType_UInt8:
                        literal.uInt8Value = ~literal.uInt8Value;
                        return literal;
                    case MaPLPrimitiveType_UInt16:
                        literal.uInt16Value = ~literal.uInt16Value;
                        return literal;
                    case MaPLPrimitiveType_UInt32:
                        literal.uInt32Value = ~literal.uInt32Value;
                        return literal;
                    case MaPLPrimitiveType_UInt64: // Intentional fallthrough.
                    case MaPLPrimitiveType_Int_AmbiguousSizeAndSign:
                        literal.uInt64Value = ~literal.uInt64Value;
                        return literal;
                    default: break;
                }
            }
                break;
            case MaPLParser::TERNARY_CONDITIONAL: {
                MaPLLiteral conditionalLiteral = constantValueForExpression(expression->expression(0));
                if (conditionalLiteral.type.primitiveType != MaPLPrimitiveType_Boolean) { break; }
                size_t chosenExpressionIndex = conditionalLiteral.booleanValue ? 1 : 2;
                return constantValueForExpression(expression->expression(chosenExpressionIndex));
            }
            case MaPLParser::NULL_COALESCING: {
                // It's very unlikely that NULL coalescing would be a compile-time constant,
                // though it's possible if you had an expression like "NULL ?? NULL".
                MaPLLiteral literal = constantValueForExpression(expression->expression(0));
                if (literal.type.primitiveType != MaPLPrimitiveType_Pointer) { break; }
                return constantValueForExpression(expression->expression(1));
            }
            case MaPLParser::LITERAL_INT: {
                std::string intAsString = expression->LITERAL_INT()->getText();
                MaPLLiteral literal = { { MaPLPrimitiveType_Int_AmbiguousSizeAndSign } };
                literal.uInt64Value = (u_int64_t)std::stoull(intAsString);
                return literal;
            }
            case MaPLParser::LITERAL_FLOAT: {
                std::string floatAsString = expression->LITERAL_FLOAT()->getText();
                MaPLLiteral literal = { { MaPLPrimitiveType_Float_AmbiguousSize } };
                literal.float64Value = (double_t)std::stod(floatAsString);
                return literal;
            }
            case MaPLParser::LITERAL_STRING: {
                std::string literalString = expression->LITERAL_STRING()->getText();
                // The literal text will always contain the string quotes, substring call removes them.
                std::filesystem::path stringValue = literalString.substr(1, literalString.length()-2);
                MaPLLiteral literal = { { MaPLPrimitiveType_String } };
                literal.stringValue = stringValue;
                return literal;
            }
            case MaPLParser::LITERAL_NULL: return { { MaPLPrimitiveType_Pointer } };
        }
    }
    return { { MaPLPrimitiveType_Uninitialized } };
}

MaPLType MaPLFile::dataTypeForExpression(MaPLParser::ExpressionContext *expression) {
    if (expression->keyToken) {
        switch (expression->keyToken->getType()) {
            case MaPLParser::PAREN_OPEN: {
                // This is a typecast, return the type that is specified in the cast.
                MaPLParser::TypeContext *typeContext = expression->type();
                MaPLType type = typeForTypeContext(typeContext);
                if (type.primitiveType == MaPLPrimitiveType_TypeError) {
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
                    if (isConcreteUnsignedInt(type.primitiveType)) {
                        logError(this, expression->keyToken, "Unsigned integers cannot be negated.");
                        return { MaPLPrimitiveType_TypeError };
                    }
                    if (type.primitiveType == MaPLPrimitiveType_Int_AmbiguousSizeAndSign) {
                        // If the datatype is ambiguous, but it's being negated, it must be a signed int.
                        return { MaPLPrimitiveType_SignedInt_AmbiguousSize };
                    }
                    if (isNumeric(type.primitiveType)) {
                        return type;
                    }
                    logError(this, expression->keyToken, "Numeric negation can only be applied to numeric data types.");
                    return { MaPLPrimitiveType_TypeError };
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
                logNonNumericOperandsError(this, expression->keyToken);
                return { MaPLPrimitiveType_TypeError };
            }
            case MaPLParser::ADD: {
                std::vector<MaPLParser::ExpressionContext *> childExpressions = expression->expression();
                MaPLType type1 = dataTypeForExpression(childExpressions[0]);
                MaPLType type2 = dataTypeForExpression(childExpressions[1]);
                // Plus operator could be numeric add or string concatenation.
                if (type1.primitiveType == MaPLPrimitiveType_String &&
                    type2.primitiveType == MaPLPrimitiveType_String) {
                    return { MaPLPrimitiveType_String };
                } else if (isNumeric(type1.primitiveType) && isNumeric(type2.primitiveType)) {
                    return { reconcileTypes(type1.primitiveType, type2.primitiveType, expression->keyToken) };
                }
                logError(this, expression->keyToken, "Both operands must be either string (concatenation) or numeric (addition).");
                return { MaPLPrimitiveType_TypeError };
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
                return { MaPLPrimitiveType_TypeError };
            }
            case MaPLParser::BITWISE_NEGATION: {
                    MaPLType type = dataTypeForExpression(expression->expression(0));
                    if (!isIntegral(type.primitiveType)) {
                        logError(this, expression->keyToken, "Bitwise negation can only be applied to integer data types.");
                        return { MaPLPrimitiveType_TypeError };
                    }
                    return type;
            }
            case MaPLParser::TERNARY_CONDITIONAL: {
                std::vector<MaPLParser::ExpressionContext *> childExpressions = expression->expression();
                return reconcileExpressionTypes(childExpressions[1], childExpressions[2], expression->keyToken);
            }
            case MaPLParser::NULL_COALESCING: {
                std::vector<MaPLParser::ExpressionContext *> childExpressions = expression->expression();
                MaPLType reconciledType = reconcileExpressionTypes(childExpressions[0], childExpressions[1], expression->keyToken);
                if (reconciledType.primitiveType == MaPLPrimitiveType_TypeError) {
                    return reconciledType;
                }
                if (reconciledType.primitiveType != MaPLPrimitiveType_Pointer) {
                    logError(this, expression->keyToken, "NULL coalescing operator can only be applied to expressions which return a pointer.");
                    return { MaPLPrimitiveType_TypeError };
                }
                return reconciledType;
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
    return { MaPLPrimitiveType_TypeError };
}

MaPLType MaPLFile::objectExpressionReturnType(MaPLParser::ObjectExpressionContext *expression,
                                              const std::string &invokedOnType) {
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
                    return { MaPLPrimitiveType_TypeError };
                }
                return objectExpressionReturnType(childExpressions[1], prefixType.pointerType);
            }
            case MaPLParser::SUBSCRIPT_OPEN: {
                // Subscript invocation.
                MaPLType prefixType = objectExpressionReturnType(expression->objectExpression(0), invokedOnType);
                if (prefixType.primitiveType != MaPLPrimitiveType_Pointer) {
                    logError(this, keyToken, "The subscript operator can only be used on pointers.");
                    return { MaPLPrimitiveType_TypeError };
                }
                MaPLType indexType = dataTypeForExpression(expression->expression(0));
                MaPLParser::ApiSubscriptContext *subscript = findSubscript(this, prefixType.pointerType, indexType, NULL);
                if (!subscript) {
                    logError(this, expression->keyToken, "Unable to find a subscript on type '"+prefixType.pointerType+"' with an index parameter of type "+descriptorForType(indexType)+".");
                    return { MaPLPrimitiveType_TypeError };
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
                                                                        parameterTypes,
                                                                        MaPLParameterStrategy_Flexible,
                                                                        NULL);
                if (!function) {
                    std::string functionSignature = descriptorForFunction(functionName,
                                                                          parameterTypes,
                                                                          false);
                    if (invokedOnType.empty()) {
                        logError(this, expression->identifier()->start, "Unable to find a global '"+functionSignature+"' function.");
                    } else {
                        logError(this, expression->identifier()->start, "Unable to find a '"+functionSignature+"' function on type '"+invokedOnType+"'.");
                    }
                    return { MaPLPrimitiveType_TypeError };
                }
                
                MaPLParser::ApiFunctionContext *conflictingFunction = findFunction(this,
                                                                                   invokedOnType,
                                                                                   functionName,
                                                                                   parameterTypes,
                                                                                   MaPLParameterStrategy_Flexible,
                                                                                   function);
                if (conflictingFunction) {
                    std::string errorSuffix = "This ambiguity might be resolved by renaming these APIs, or by adding a typecast to explicitly describe the type of any literal parameters.";
                    if (invokedOnType.empty()) {
                        logError(this, expression->identifier()->start, "This function invocation is ambiguous between global functions '"+descriptorForFunction(function)+"' and '"+descriptorForFunction(conflictingFunction)+"'. "+errorSuffix);
                    } else {
                        logError(this, expression->identifier()->start, "This function invocation is ambiguous between functions '"+descriptorForFunction(function)+"' and '"+descriptorForFunction(conflictingFunction)+"' in type '"+invokedOnType+"'. "+errorSuffix);
                    }
                    return { MaPLPrimitiveType_TypeError };
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
                if (variable.type.primitiveType != MaPLPrimitiveType_TypeError) {
                    return variable.type;
                }
            }
            
            // The name doesn't match a variable, check if it matches a property.
            MaPLParser::ApiPropertyContext *property = findProperty(this,
                                                                    invokedOnType,
                                                                    propertyOrVariableName,
                                                                    NULL);
            if (!property) {
                if (isInvokedOnType) {
                    logError(this, identifier->start, "Unable to find a '"+propertyOrVariableName+"' property on type '"+invokedOnType+"'.");
                } else {
                    logError(this, identifier->start, "Unable to find a variable or global property named '"+propertyOrVariableName+"'.");
                }
                return { MaPLPrimitiveType_TypeError };
            }
            return typeForTypeContext(property->type());
        }
    }
    logError(this, keyToken, "Error determining the type of this expression.");
    return { MaPLPrimitiveType_TypeError };
}

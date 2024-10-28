
// Generated from ./MaPLParser.g4 by ANTLR 4.13.2

#pragma once


#include "antlr4-runtime.h"




class  MaPLParser : public antlr4::Parser {
public:
  enum {
    ASSIGN = 1, ADD = 2, ADD_ASSIGN = 3, SUBTRACT = 4, SUBTRACT_ASSIGN = 5, 
    DIVIDE = 6, DIVIDE_ASSIGN = 7, MULTIPLY = 8, MULTIPLY_ASSIGN = 9, MOD = 10, 
    MOD_ASSIGN = 11, INCREMENT = 12, DECREMENT = 13, LOGICAL_EQUALITY = 14, 
    LOGICAL_INEQUALITY = 15, LOGICAL_AND = 16, LOGICAL_OR = 17, LOGICAL_NEGATION = 18, 
    LESS_THAN = 19, LESS_THAN_EQUAL = 20, GREATER_THAN = 21, GREATER_THAN_EQUAL = 22, 
    BITWISE_NEGATION = 23, BITWISE_AND = 24, BITWISE_AND_ASSIGN = 25, BITWISE_OR = 26, 
    BITWISE_OR_ASSIGN = 27, BITWISE_XOR = 28, BITWISE_XOR_ASSIGN = 29, BITWISE_SHIFT_LEFT = 30, 
    BITWISE_SHIFT_LEFT_ASSIGN = 31, BITWISE_SHIFT_RIGHT_ASSIGN = 32, LOOP_WHILE = 33, 
    LOOP_FOR = 34, LOOP_DO = 35, CONDITIONAL = 36, CONDITIONAL_ELSE = 37, 
    BREAK = 38, CONTINUE = 39, EXIT = 40, DECL_CHAR = 41, DECL_INT32 = 42, 
    DECL_INT64 = 43, DECL_UINT32 = 44, DECL_UINT64 = 45, DECL_FLOAT32 = 46, 
    DECL_FLOAT64 = 47, DECL_BOOL = 48, DECL_STRING = 49, PAREN_OPEN = 50, 
    PAREN_CLOSE = 51, SCOPE_OPEN = 52, SCOPE_CLOSE = 53, SUBSCRIPT_OPEN = 54, 
    SUBSCRIPT_CLOSE = 55, OBJECT_TO_MEMBER = 56, PARAM_DELIMITER = 57, COLON = 58, 
    TERNARY_CONDITIONAL = 59, NULL_COALESCING = 60, STATEMENT_DELIMITER = 61, 
    API_GLOBAL = 62, API_TYPE = 63, API_IMPORT = 64, API_READONLY = 65, 
    API_VOID = 66, API_VARIADIC_PARAMETERS = 67, LITERAL_NULL = 68, LITERAL_TRUE = 69, 
    LITERAL_FALSE = 70, LITERAL_INT = 71, LITERAL_FLOAT = 72, LITERAL_STRING = 73, 
    METADATA_OPEN = 74, IDENTIFIER = 75, BLOCK_COMMENT = 76, LINE_COMMENT = 77, 
    WHITESPACE = 78, BITWISE_SHIFT_RIGHT = 79, METADATA_INTERPOLATION = 80, 
    METADATA_CLOSE = 81, METADATA_CHAR = 82
  };

  enum {
    RuleProgram = 0, RuleStatement = 1, RuleImperativeStatement = 2, RuleMetadataStatement = 3, 
    RuleAssignStatement = 4, RuleUnaryStatement = 5, RuleExpression = 6, 
    RuleObjectExpression = 7, RuleVariableDeclaration = 8, RuleType = 9, 
    RulePointerType = 10, RuleScope = 11, RuleWhileLoop = 12, RuleForLoop = 13, 
    RuleForLoopControlStatements = 14, RuleDoWhileLoop = 15, RuleConditional = 16, 
    RuleConditionalElse = 17, RuleApiGlobal = 18, RuleApiType = 19, RuleApiInheritance = 20, 
    RuleApiFunction = 21, RuleApiFunctionParams = 22, RuleApiProperty = 23, 
    RuleApiSubscript = 24, RuleApiImport = 25, RuleIdentifier = 26, RuleBitwiseShiftRight = 27
  };

  explicit MaPLParser(antlr4::TokenStream *input);

  MaPLParser(antlr4::TokenStream *input, const antlr4::atn::ParserATNSimulatorOptions &options);

  ~MaPLParser() override;

  std::string getGrammarFileName() const override;

  const antlr4::atn::ATN& getATN() const override;

  const std::vector<std::string>& getRuleNames() const override;

  const antlr4::dfa::Vocabulary& getVocabulary() const override;

  antlr4::atn::SerializedATNView getSerializedATN() const override;


  class ProgramContext;
  class StatementContext;
  class ImperativeStatementContext;
  class MetadataStatementContext;
  class AssignStatementContext;
  class UnaryStatementContext;
  class ExpressionContext;
  class ObjectExpressionContext;
  class VariableDeclarationContext;
  class TypeContext;
  class PointerTypeContext;
  class ScopeContext;
  class WhileLoopContext;
  class ForLoopContext;
  class ForLoopControlStatementsContext;
  class DoWhileLoopContext;
  class ConditionalContext;
  class ConditionalElseContext;
  class ApiGlobalContext;
  class ApiTypeContext;
  class ApiInheritanceContext;
  class ApiFunctionContext;
  class ApiFunctionParamsContext;
  class ApiPropertyContext;
  class ApiSubscriptContext;
  class ApiImportContext;
  class IdentifierContext;
  class BitwiseShiftRightContext; 

  class  ProgramContext : public antlr4::ParserRuleContext {
  public:
    ProgramContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *EOF();
    std::vector<StatementContext *> statement();
    StatementContext* statement(size_t i);

   
  };

  ProgramContext* program();

  class  StatementContext : public antlr4::ParserRuleContext {
  public:
    StatementContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    ImperativeStatementContext *imperativeStatement();
    antlr4::tree::TerminalNode *STATEMENT_DELIMITER();
    WhileLoopContext *whileLoop();
    ForLoopContext *forLoop();
    DoWhileLoopContext *doWhileLoop();
    ConditionalContext *conditional();
    ApiGlobalContext *apiGlobal();
    ApiTypeContext *apiType();
    ApiImportContext *apiImport();
    MetadataStatementContext *metadataStatement();
    ScopeContext *scope();

   
  };

  StatementContext* statement();

  class  ImperativeStatementContext : public antlr4::ParserRuleContext {
  public:
    antlr4::Token *keyToken = nullptr;
    ImperativeStatementContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *BREAK();
    antlr4::tree::TerminalNode *CONTINUE();
    antlr4::tree::TerminalNode *EXIT();
    VariableDeclarationContext *variableDeclaration();
    AssignStatementContext *assignStatement();
    UnaryStatementContext *unaryStatement();
    ObjectExpressionContext *objectExpression();

   
  };

  ImperativeStatementContext* imperativeStatement();

  class  MetadataStatementContext : public antlr4::ParserRuleContext {
  public:
    antlr4::Token *metadata_openToken = nullptr;
    std::vector<antlr4::Token *> metadataTokens;
    antlr4::Token *metadata_interpolationToken = nullptr;
    antlr4::Token *scope_closeToken = nullptr;
    antlr4::Token *metadata_closeToken = nullptr;
    MetadataStatementContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *METADATA_OPEN();
    antlr4::tree::TerminalNode *METADATA_CLOSE();
    std::vector<ExpressionContext *> expression();
    ExpressionContext* expression(size_t i);
    std::vector<antlr4::tree::TerminalNode *> METADATA_INTERPOLATION();
    antlr4::tree::TerminalNode* METADATA_INTERPOLATION(size_t i);
    std::vector<antlr4::tree::TerminalNode *> SCOPE_CLOSE();
    antlr4::tree::TerminalNode* SCOPE_CLOSE(size_t i);

   
  };

  MetadataStatementContext* metadataStatement();

  class  AssignStatementContext : public antlr4::ParserRuleContext {
  public:
    antlr4::Token *keyToken = nullptr;
    AssignStatementContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    ObjectExpressionContext *objectExpression();
    ExpressionContext *expression();
    antlr4::tree::TerminalNode *ASSIGN();
    antlr4::tree::TerminalNode *ADD_ASSIGN();
    antlr4::tree::TerminalNode *SUBTRACT_ASSIGN();
    antlr4::tree::TerminalNode *MULTIPLY_ASSIGN();
    antlr4::tree::TerminalNode *DIVIDE_ASSIGN();
    antlr4::tree::TerminalNode *MOD_ASSIGN();
    antlr4::tree::TerminalNode *BITWISE_AND_ASSIGN();
    antlr4::tree::TerminalNode *BITWISE_OR_ASSIGN();
    antlr4::tree::TerminalNode *BITWISE_XOR_ASSIGN();
    antlr4::tree::TerminalNode *BITWISE_SHIFT_LEFT_ASSIGN();
    antlr4::tree::TerminalNode *BITWISE_SHIFT_RIGHT_ASSIGN();

   
  };

  AssignStatementContext* assignStatement();

  class  UnaryStatementContext : public antlr4::ParserRuleContext {
  public:
    antlr4::Token *keyToken = nullptr;
    UnaryStatementContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    ObjectExpressionContext *objectExpression();
    antlr4::tree::TerminalNode *INCREMENT();
    antlr4::tree::TerminalNode *DECREMENT();

   
  };

  UnaryStatementContext* unaryStatement();

  class  ExpressionContext : public antlr4::ParserRuleContext {
  public:
    antlr4::Token *keyToken = nullptr;
    ExpressionContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    TypeContext *type();
    antlr4::tree::TerminalNode *PAREN_CLOSE();
    std::vector<ExpressionContext *> expression();
    ExpressionContext* expression(size_t i);
    antlr4::tree::TerminalNode *PAREN_OPEN();
    antlr4::tree::TerminalNode *LOGICAL_NEGATION();
    antlr4::tree::TerminalNode *SUBTRACT();
    antlr4::tree::TerminalNode *BITWISE_NEGATION();
    antlr4::tree::TerminalNode *LITERAL_TRUE();
    antlr4::tree::TerminalNode *LITERAL_FALSE();
    antlr4::tree::TerminalNode *LITERAL_NULL();
    antlr4::tree::TerminalNode *LITERAL_INT();
    antlr4::tree::TerminalNode *LITERAL_FLOAT();
    antlr4::tree::TerminalNode *LITERAL_STRING();
    ObjectExpressionContext *objectExpression();
    antlr4::tree::TerminalNode *MOD();
    antlr4::tree::TerminalNode *MULTIPLY();
    antlr4::tree::TerminalNode *DIVIDE();
    antlr4::tree::TerminalNode *ADD();
    BitwiseShiftRightContext *bitwiseShiftRight();
    antlr4::tree::TerminalNode *BITWISE_SHIFT_LEFT();
    antlr4::tree::TerminalNode *BITWISE_AND();
    antlr4::tree::TerminalNode *BITWISE_XOR();
    antlr4::tree::TerminalNode *BITWISE_OR();
    antlr4::tree::TerminalNode *NULL_COALESCING();
    antlr4::tree::TerminalNode *LOGICAL_EQUALITY();
    antlr4::tree::TerminalNode *LOGICAL_INEQUALITY();
    antlr4::tree::TerminalNode *LESS_THAN();
    antlr4::tree::TerminalNode *LESS_THAN_EQUAL();
    antlr4::tree::TerminalNode *GREATER_THAN();
    antlr4::tree::TerminalNode *GREATER_THAN_EQUAL();
    antlr4::tree::TerminalNode *LOGICAL_AND();
    antlr4::tree::TerminalNode *LOGICAL_OR();
    antlr4::tree::TerminalNode *COLON();
    antlr4::tree::TerminalNode *TERNARY_CONDITIONAL();

   
  };

  ExpressionContext* expression();
  ExpressionContext* expression(int precedence);
  class  ObjectExpressionContext : public antlr4::ParserRuleContext {
  public:
    antlr4::Token *keyToken = nullptr;
    ObjectExpressionContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    IdentifierContext *identifier();
    antlr4::tree::TerminalNode *PAREN_CLOSE();
    antlr4::tree::TerminalNode *PAREN_OPEN();
    std::vector<ExpressionContext *> expression();
    ExpressionContext* expression(size_t i);
    std::vector<antlr4::tree::TerminalNode *> PARAM_DELIMITER();
    antlr4::tree::TerminalNode* PARAM_DELIMITER(size_t i);
    std::vector<ObjectExpressionContext *> objectExpression();
    ObjectExpressionContext* objectExpression(size_t i);
    antlr4::tree::TerminalNode *OBJECT_TO_MEMBER();
    antlr4::tree::TerminalNode *SUBSCRIPT_CLOSE();
    antlr4::tree::TerminalNode *SUBSCRIPT_OPEN();

   
  };

  ObjectExpressionContext* objectExpression();
  ObjectExpressionContext* objectExpression(int precedence);
  class  VariableDeclarationContext : public antlr4::ParserRuleContext {
  public:
    VariableDeclarationContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    TypeContext *type();
    IdentifierContext *identifier();
    antlr4::tree::TerminalNode *ASSIGN();
    ExpressionContext *expression();

   
  };

  VariableDeclarationContext* variableDeclaration();

  class  TypeContext : public antlr4::ParserRuleContext {
  public:
    TypeContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *DECL_CHAR();
    antlr4::tree::TerminalNode *DECL_INT32();
    antlr4::tree::TerminalNode *DECL_INT64();
    antlr4::tree::TerminalNode *DECL_UINT32();
    antlr4::tree::TerminalNode *DECL_UINT64();
    antlr4::tree::TerminalNode *DECL_FLOAT32();
    antlr4::tree::TerminalNode *DECL_FLOAT64();
    antlr4::tree::TerminalNode *DECL_BOOL();
    antlr4::tree::TerminalNode *DECL_STRING();
    PointerTypeContext *pointerType();

   
  };

  TypeContext* type();

  class  PointerTypeContext : public antlr4::ParserRuleContext {
  public:
    PointerTypeContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    IdentifierContext *identifier();
    antlr4::tree::TerminalNode *LESS_THAN();
    std::vector<TypeContext *> type();
    TypeContext* type(size_t i);
    antlr4::tree::TerminalNode *GREATER_THAN();
    std::vector<antlr4::tree::TerminalNode *> PARAM_DELIMITER();
    antlr4::tree::TerminalNode* PARAM_DELIMITER(size_t i);

   
  };

  PointerTypeContext* pointerType();

  class  ScopeContext : public antlr4::ParserRuleContext {
  public:
    ScopeContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *SCOPE_OPEN();
    antlr4::tree::TerminalNode *SCOPE_CLOSE();
    std::vector<StatementContext *> statement();
    StatementContext* statement(size_t i);

   
  };

  ScopeContext* scope();

  class  WhileLoopContext : public antlr4::ParserRuleContext {
  public:
    WhileLoopContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *LOOP_WHILE();
    ExpressionContext *expression();
    ScopeContext *scope();

   
  };

  WhileLoopContext* whileLoop();

  class  ForLoopContext : public antlr4::ParserRuleContext {
  public:
    ForLoopContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *LOOP_FOR();
    antlr4::tree::TerminalNode *PAREN_OPEN();
    ForLoopControlStatementsContext *forLoopControlStatements();
    antlr4::tree::TerminalNode *PAREN_CLOSE();
    ScopeContext *scope();

   
  };

  ForLoopContext* forLoop();

  class  ForLoopControlStatementsContext : public antlr4::ParserRuleContext {
  public:
    MaPLParser::ImperativeStatementContext *firstStatement = nullptr;
    MaPLParser::ImperativeStatementContext *lastStatement = nullptr;
    ForLoopControlStatementsContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    std::vector<antlr4::tree::TerminalNode *> STATEMENT_DELIMITER();
    antlr4::tree::TerminalNode* STATEMENT_DELIMITER(size_t i);
    ExpressionContext *expression();
    std::vector<ImperativeStatementContext *> imperativeStatement();
    ImperativeStatementContext* imperativeStatement(size_t i);

   
  };

  ForLoopControlStatementsContext* forLoopControlStatements();

  class  DoWhileLoopContext : public antlr4::ParserRuleContext {
  public:
    DoWhileLoopContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *LOOP_DO();
    ScopeContext *scope();
    antlr4::tree::TerminalNode *LOOP_WHILE();
    ExpressionContext *expression();
    antlr4::tree::TerminalNode *STATEMENT_DELIMITER();

   
  };

  DoWhileLoopContext* doWhileLoop();

  class  ConditionalContext : public antlr4::ParserRuleContext {
  public:
    ConditionalContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *CONDITIONAL();
    ExpressionContext *expression();
    ScopeContext *scope();
    ConditionalElseContext *conditionalElse();

   
  };

  ConditionalContext* conditional();

  class  ConditionalElseContext : public antlr4::ParserRuleContext {
  public:
    ConditionalElseContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *CONDITIONAL_ELSE();
    ScopeContext *scope();
    ConditionalContext *conditional();

   
  };

  ConditionalElseContext* conditionalElse();

  class  ApiGlobalContext : public antlr4::ParserRuleContext {
  public:
    ApiGlobalContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *API_GLOBAL();
    antlr4::tree::TerminalNode *STATEMENT_DELIMITER();
    ApiFunctionContext *apiFunction();
    ApiPropertyContext *apiProperty();

   
  };

  ApiGlobalContext* apiGlobal();

  class  ApiTypeContext : public antlr4::ParserRuleContext {
  public:
    MaPLParser::IdentifierContext *typeName = nullptr;
    MaPLParser::IdentifierContext *identifierContext = nullptr;
    std::vector<IdentifierContext *> generics;
    ApiTypeContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *API_TYPE();
    antlr4::tree::TerminalNode *SCOPE_OPEN();
    antlr4::tree::TerminalNode *SCOPE_CLOSE();
    std::vector<IdentifierContext *> identifier();
    IdentifierContext* identifier(size_t i);
    antlr4::tree::TerminalNode *LESS_THAN();
    antlr4::tree::TerminalNode *GREATER_THAN();
    ApiInheritanceContext *apiInheritance();
    std::vector<antlr4::tree::TerminalNode *> STATEMENT_DELIMITER();
    antlr4::tree::TerminalNode* STATEMENT_DELIMITER(size_t i);
    std::vector<ApiFunctionContext *> apiFunction();
    ApiFunctionContext* apiFunction(size_t i);
    std::vector<ApiPropertyContext *> apiProperty();
    ApiPropertyContext* apiProperty(size_t i);
    std::vector<ApiSubscriptContext *> apiSubscript();
    ApiSubscriptContext* apiSubscript(size_t i);
    std::vector<antlr4::tree::TerminalNode *> PARAM_DELIMITER();
    antlr4::tree::TerminalNode* PARAM_DELIMITER(size_t i);

   
  };

  ApiTypeContext* apiType();

  class  ApiInheritanceContext : public antlr4::ParserRuleContext {
  public:
    ApiInheritanceContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *COLON();
    std::vector<PointerTypeContext *> pointerType();
    PointerTypeContext* pointerType(size_t i);
    std::vector<antlr4::tree::TerminalNode *> PARAM_DELIMITER();
    antlr4::tree::TerminalNode* PARAM_DELIMITER(size_t i);

   
  };

  ApiInheritanceContext* apiInheritance();

  class  ApiFunctionContext : public antlr4::ParserRuleContext {
  public:
    ApiFunctionContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    IdentifierContext *identifier();
    antlr4::tree::TerminalNode *PAREN_OPEN();
    antlr4::tree::TerminalNode *PAREN_CLOSE();
    antlr4::tree::TerminalNode *API_VOID();
    TypeContext *type();
    ApiFunctionParamsContext *apiFunctionParams();

   
  };

  ApiFunctionContext* apiFunction();

  class  ApiFunctionParamsContext : public antlr4::ParserRuleContext {
  public:
    ApiFunctionParamsContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *API_VARIADIC_PARAMETERS();
    std::vector<TypeContext *> type();
    TypeContext* type(size_t i);
    std::vector<IdentifierContext *> identifier();
    IdentifierContext* identifier(size_t i);
    std::vector<antlr4::tree::TerminalNode *> PARAM_DELIMITER();
    antlr4::tree::TerminalNode* PARAM_DELIMITER(size_t i);

   
  };

  ApiFunctionParamsContext* apiFunctionParams();

  class  ApiPropertyContext : public antlr4::ParserRuleContext {
  public:
    ApiPropertyContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    TypeContext *type();
    IdentifierContext *identifier();
    antlr4::tree::TerminalNode *API_READONLY();

   
  };

  ApiPropertyContext* apiProperty();

  class  ApiSubscriptContext : public antlr4::ParserRuleContext {
  public:
    ApiSubscriptContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    std::vector<TypeContext *> type();
    TypeContext* type(size_t i);
    antlr4::tree::TerminalNode *SUBSCRIPT_OPEN();
    antlr4::tree::TerminalNode *SUBSCRIPT_CLOSE();
    antlr4::tree::TerminalNode *API_READONLY();

   
  };

  ApiSubscriptContext* apiSubscript();

  class  ApiImportContext : public antlr4::ParserRuleContext {
  public:
    ApiImportContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *API_IMPORT();
    antlr4::tree::TerminalNode *LITERAL_STRING();

   
  };

  ApiImportContext* apiImport();

  class  IdentifierContext : public antlr4::ParserRuleContext {
  public:
    IdentifierContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *LOOP_WHILE();
    antlr4::tree::TerminalNode *LOOP_FOR();
    antlr4::tree::TerminalNode *LOOP_DO();
    antlr4::tree::TerminalNode *CONDITIONAL();
    antlr4::tree::TerminalNode *CONDITIONAL_ELSE();
    antlr4::tree::TerminalNode *BREAK();
    antlr4::tree::TerminalNode *CONTINUE();
    antlr4::tree::TerminalNode *EXIT();
    antlr4::tree::TerminalNode *LITERAL_NULL();
    antlr4::tree::TerminalNode *API_READONLY();
    antlr4::tree::TerminalNode *API_VOID();
    antlr4::tree::TerminalNode *DECL_CHAR();
    antlr4::tree::TerminalNode *DECL_INT32();
    antlr4::tree::TerminalNode *DECL_INT64();
    antlr4::tree::TerminalNode *DECL_UINT32();
    antlr4::tree::TerminalNode *DECL_UINT64();
    antlr4::tree::TerminalNode *DECL_FLOAT32();
    antlr4::tree::TerminalNode *DECL_FLOAT64();
    antlr4::tree::TerminalNode *DECL_BOOL();
    antlr4::tree::TerminalNode *DECL_STRING();
    antlr4::tree::TerminalNode *LITERAL_TRUE();
    antlr4::tree::TerminalNode *LITERAL_FALSE();
    antlr4::tree::TerminalNode *IDENTIFIER();

   
  };

  IdentifierContext* identifier();

  class  BitwiseShiftRightContext : public antlr4::ParserRuleContext {
  public:
    BitwiseShiftRightContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    std::vector<antlr4::tree::TerminalNode *> GREATER_THAN();
    antlr4::tree::TerminalNode* GREATER_THAN(size_t i);

   
  };

  BitwiseShiftRightContext* bitwiseShiftRight();


  bool sempred(antlr4::RuleContext *_localctx, size_t ruleIndex, size_t predicateIndex) override;

  bool expressionSempred(ExpressionContext *_localctx, size_t predicateIndex);
  bool objectExpressionSempred(ObjectExpressionContext *_localctx, size_t predicateIndex);

  // By default the static state used to implement the parser is lazily initialized during the first
  // call to the constructor. You can call this function if you wish to initialize the static state
  // ahead of time.
  static void initialize();

private:
};


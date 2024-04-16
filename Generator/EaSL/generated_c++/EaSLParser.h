
// Generated from ./EaSL.g4 by ANTLR 4.13.1

#pragma once


#include "antlr4-runtime.h"




class  EaSLParser : public antlr4::Parser {
public:
  enum {
    DECL_CHAR = 1, DECL_INT32 = 2, DECL_INT64 = 3, DECL_UINT32 = 4, DECL_UINT64 = 5, 
    DECL_FLOAT32 = 6, DECL_FLOAT64 = 7, DECL_BOOL = 8, DECL_STRING = 9, 
    DECL_UID = 10, REFERENCE = 11, REFERENCE_OPEN = 12, REFERENCE_CLOSE = 13, 
    NAMESPACE = 14, CLASS = 15, ENUM = 16, DEFAULTS_TO = 17, DEFINITION_OPEN = 18, 
    DEFINITION_CLOSE = 19, SEQUENCE_OPEN = 20, SEQUENCE_CLOSE = 21, SEQUENCE_DELIMITER = 22, 
    SEQUENCE_WILDCARD = 23, COLON = 24, NAMESPACE_DELIMITER = 25, STATEMENT_END = 26, 
    ANNOTATION = 27, LITERAL_NULL = 28, LITERAL_TRUE = 29, LITERAL_FALSE = 30, 
    LITERAL_INT = 31, LITERAL_FLOAT = 32, LITERAL_STRING = 33, IDENTIFIER = 34, 
    BLOCK_COMMENT = 35, LINE_COMMENT = 36, WHITESPACE = 37
  };

  enum {
    RuleSchema = 0, RuleNamespace_ = 1, RuleClassDefinition = 2, RuleAttribute = 3, 
    RuleSequenceDescriptor = 4, RuleSequenceLength = 5, RuleEnumDefinition = 6, 
    RuleDefaultValue = 7, RuleLiteralValue = 8, RuleType = 9, RuleClassType = 10, 
    RuleIdentifier = 11
  };

  explicit EaSLParser(antlr4::TokenStream *input);

  EaSLParser(antlr4::TokenStream *input, const antlr4::atn::ParserATNSimulatorOptions &options);

  ~EaSLParser() override;

  std::string getGrammarFileName() const override;

  const antlr4::atn::ATN& getATN() const override;

  const std::vector<std::string>& getRuleNames() const override;

  const antlr4::dfa::Vocabulary& getVocabulary() const override;

  antlr4::atn::SerializedATNView getSerializedATN() const override;


  class SchemaContext;
  class Namespace_Context;
  class ClassDefinitionContext;
  class AttributeContext;
  class SequenceDescriptorContext;
  class SequenceLengthContext;
  class EnumDefinitionContext;
  class DefaultValueContext;
  class LiteralValueContext;
  class TypeContext;
  class ClassTypeContext;
  class IdentifierContext; 

  class  SchemaContext : public antlr4::ParserRuleContext {
  public:
    SchemaContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    Namespace_Context *namespace_();
    antlr4::tree::TerminalNode *EOF();
    std::vector<ClassDefinitionContext *> classDefinition();
    ClassDefinitionContext* classDefinition(size_t i);
    std::vector<EnumDefinitionContext *> enumDefinition();
    EnumDefinitionContext* enumDefinition(size_t i);

   
  };

  SchemaContext* schema();

  class  Namespace_Context : public antlr4::ParserRuleContext {
  public:
    Namespace_Context(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *NAMESPACE();
    IdentifierContext *identifier();
    antlr4::tree::TerminalNode *STATEMENT_END();

   
  };

  Namespace_Context* namespace_();

  class  ClassDefinitionContext : public antlr4::ParserRuleContext {
  public:
    ClassDefinitionContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *CLASS();
    IdentifierContext *identifier();
    antlr4::tree::TerminalNode *DEFINITION_OPEN();
    antlr4::tree::TerminalNode *DEFINITION_CLOSE();
    std::vector<antlr4::tree::TerminalNode *> ANNOTATION();
    antlr4::tree::TerminalNode* ANNOTATION(size_t i);
    antlr4::tree::TerminalNode *COLON();
    ClassTypeContext *classType();
    std::vector<AttributeContext *> attribute();
    AttributeContext* attribute(size_t i);

   
  };

  ClassDefinitionContext* classDefinition();

  class  AttributeContext : public antlr4::ParserRuleContext {
  public:
    AttributeContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    TypeContext *type();
    IdentifierContext *identifier();
    antlr4::tree::TerminalNode *STATEMENT_END();
    std::vector<antlr4::tree::TerminalNode *> ANNOTATION();
    antlr4::tree::TerminalNode* ANNOTATION(size_t i);
    SequenceDescriptorContext *sequenceDescriptor();
    antlr4::tree::TerminalNode *DEFAULTS_TO();
    DefaultValueContext *defaultValue();

   
  };

  AttributeContext* attribute();

  class  SequenceDescriptorContext : public antlr4::ParserRuleContext {
  public:
    SequenceDescriptorContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *SEQUENCE_OPEN();
    antlr4::tree::TerminalNode *SEQUENCE_CLOSE();
    std::vector<SequenceLengthContext *> sequenceLength();
    SequenceLengthContext* sequenceLength(size_t i);
    antlr4::tree::TerminalNode *SEQUENCE_DELIMITER();

   
  };

  SequenceDescriptorContext* sequenceDescriptor();

  class  SequenceLengthContext : public antlr4::ParserRuleContext {
  public:
    SequenceLengthContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *LITERAL_INT();
    antlr4::tree::TerminalNode *SEQUENCE_WILDCARD();

   
  };

  SequenceLengthContext* sequenceLength();

  class  EnumDefinitionContext : public antlr4::ParserRuleContext {
  public:
    EaSLParser::IdentifierContext *enumName = nullptr;
    EaSLParser::IdentifierContext *identifierContext = nullptr;
    std::vector<IdentifierContext *> enumValue;
    EnumDefinitionContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *ENUM();
    antlr4::tree::TerminalNode *DEFINITION_OPEN();
    antlr4::tree::TerminalNode *DEFINITION_CLOSE();
    std::vector<IdentifierContext *> identifier();
    IdentifierContext* identifier(size_t i);
    std::vector<antlr4::tree::TerminalNode *> ANNOTATION();
    antlr4::tree::TerminalNode* ANNOTATION(size_t i);
    std::vector<antlr4::tree::TerminalNode *> SEQUENCE_DELIMITER();
    antlr4::tree::TerminalNode* SEQUENCE_DELIMITER(size_t i);

   
  };

  EnumDefinitionContext* enumDefinition();

  class  DefaultValueContext : public antlr4::ParserRuleContext {
  public:
    DefaultValueContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    std::vector<LiteralValueContext *> literalValue();
    LiteralValueContext* literalValue(size_t i);
    std::vector<antlr4::tree::TerminalNode *> SEQUENCE_DELIMITER();
    antlr4::tree::TerminalNode* SEQUENCE_DELIMITER(size_t i);

   
  };

  DefaultValueContext* defaultValue();

  class  LiteralValueContext : public antlr4::ParserRuleContext {
  public:
    antlr4::Token *literalToken = nullptr;
    LiteralValueContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *LITERAL_NULL();
    antlr4::tree::TerminalNode *LITERAL_TRUE();
    antlr4::tree::TerminalNode *LITERAL_FALSE();
    antlr4::tree::TerminalNode *LITERAL_STRING();
    antlr4::tree::TerminalNode *LITERAL_INT();
    antlr4::tree::TerminalNode *LITERAL_FLOAT();
    IdentifierContext *identifier();

   
  };

  LiteralValueContext* literalValue();

  class  TypeContext : public antlr4::ParserRuleContext {
  public:
    antlr4::Token *typeToken = nullptr;
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
    antlr4::tree::TerminalNode *DECL_UID();
    antlr4::tree::TerminalNode *REFERENCE_OPEN();
    ClassTypeContext *classType();
    antlr4::tree::TerminalNode *REFERENCE_CLOSE();
    antlr4::tree::TerminalNode *REFERENCE();

   
  };

  TypeContext* type();

  class  ClassTypeContext : public antlr4::ParserRuleContext {
  public:
    EaSLParser::IdentifierContext *namespaceIdentifier = nullptr;
    EaSLParser::IdentifierContext *classIdentifier = nullptr;
    ClassTypeContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    std::vector<IdentifierContext *> identifier();
    IdentifierContext* identifier(size_t i);
    antlr4::tree::TerminalNode *NAMESPACE_DELIMITER();

   
  };

  ClassTypeContext* classType();

  class  IdentifierContext : public antlr4::ParserRuleContext {
  public:
    IdentifierContext(antlr4::ParserRuleContext *parent, size_t invokingState);
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
    antlr4::tree::TerminalNode *DECL_UID();
    antlr4::tree::TerminalNode *REFERENCE();
    antlr4::tree::TerminalNode *NAMESPACE();
    antlr4::tree::TerminalNode *CLASS();
    antlr4::tree::TerminalNode *ENUM();
    antlr4::tree::TerminalNode *LITERAL_NULL();
    antlr4::tree::TerminalNode *LITERAL_TRUE();
    antlr4::tree::TerminalNode *LITERAL_FALSE();
    antlr4::tree::TerminalNode *IDENTIFIER();

   
  };

  IdentifierContext* identifier();


  // By default the static state used to implement the parser is lazily initialized during the first
  // call to the constructor. You can call this function if you wish to initialize the static state
  // ahead of time.
  static void initialize();

private:
};


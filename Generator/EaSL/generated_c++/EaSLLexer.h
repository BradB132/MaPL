
// Generated from ./EaSL.g4 by ANTLR 4.13.2

#pragma once


#include "antlr4-runtime.h"




class  EaSLLexer : public antlr4::Lexer {
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
    REGEX = 35, BLOCK_COMMENT = 36, LINE_COMMENT = 37, WHITESPACE = 38
  };

  explicit EaSLLexer(antlr4::CharStream *input);

  ~EaSLLexer() override;


  std::string getGrammarFileName() const override;

  const std::vector<std::string>& getRuleNames() const override;

  const std::vector<std::string>& getChannelNames() const override;

  const std::vector<std::string>& getModeNames() const override;

  const antlr4::dfa::Vocabulary& getVocabulary() const override;

  antlr4::atn::SerializedATNView getSerializedATN() const override;

  const antlr4::atn::ATN& getATN() const override;

  // By default the static state used to implement the lexer is lazily initialized during the first
  // call to the constructor. You can call this function if you wish to initialize the static state
  // ahead of time.
  static void initialize();

private:

  // Individual action functions triggered by action() above.

  // Individual semantic predicate functions triggered by sempred() above.

};


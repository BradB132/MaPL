
// Generated from ./MaPLLexer.g4 by ANTLR 4.13.1

#pragma once


#include "antlr4-runtime.h"




class  MaPLLexer : public antlr4::Lexer {
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
    METADATA = 1
  };

  explicit MaPLLexer(antlr4::CharStream *input);

  ~MaPLLexer() override;


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


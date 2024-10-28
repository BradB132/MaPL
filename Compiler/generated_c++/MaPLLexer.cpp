
// Generated from ./MaPLLexer.g4 by ANTLR 4.13.2


#include "MaPLLexer.h"


using namespace antlr4;



using namespace antlr4;

namespace {

struct MaPLLexerStaticData final {
  MaPLLexerStaticData(std::vector<std::string> ruleNames,
                          std::vector<std::string> channelNames,
                          std::vector<std::string> modeNames,
                          std::vector<std::string> literalNames,
                          std::vector<std::string> symbolicNames)
      : ruleNames(std::move(ruleNames)), channelNames(std::move(channelNames)),
        modeNames(std::move(modeNames)), literalNames(std::move(literalNames)),
        symbolicNames(std::move(symbolicNames)),
        vocabulary(this->literalNames, this->symbolicNames) {}

  MaPLLexerStaticData(const MaPLLexerStaticData&) = delete;
  MaPLLexerStaticData(MaPLLexerStaticData&&) = delete;
  MaPLLexerStaticData& operator=(const MaPLLexerStaticData&) = delete;
  MaPLLexerStaticData& operator=(MaPLLexerStaticData&&) = delete;

  std::vector<antlr4::dfa::DFA> decisionToDFA;
  antlr4::atn::PredictionContextCache sharedContextCache;
  const std::vector<std::string> ruleNames;
  const std::vector<std::string> channelNames;
  const std::vector<std::string> modeNames;
  const std::vector<std::string> literalNames;
  const std::vector<std::string> symbolicNames;
  const antlr4::dfa::Vocabulary vocabulary;
  antlr4::atn::SerializedATNView serializedATN;
  std::unique_ptr<antlr4::atn::ATN> atn;
};

::antlr4::internal::OnceFlag mapllexerLexerOnceFlag;
#if ANTLR4_USE_THREAD_LOCAL_CACHE
static thread_local
#endif
std::unique_ptr<MaPLLexerStaticData> mapllexerLexerStaticData = nullptr;

void mapllexerLexerInitialize() {
#if ANTLR4_USE_THREAD_LOCAL_CACHE
  if (mapllexerLexerStaticData != nullptr) {
    return;
  }
#else
  assert(mapllexerLexerStaticData == nullptr);
#endif
  auto staticData = std::make_unique<MaPLLexerStaticData>(
    std::vector<std::string>{
      "ASSIGN", "ADD", "ADD_ASSIGN", "SUBTRACT", "SUBTRACT_ASSIGN", "DIVIDE", 
      "DIVIDE_ASSIGN", "MULTIPLY", "MULTIPLY_ASSIGN", "MOD", "MOD_ASSIGN", 
      "INCREMENT", "DECREMENT", "LOGICAL_EQUALITY", "LOGICAL_INEQUALITY", 
      "LOGICAL_AND", "LOGICAL_OR", "LOGICAL_NEGATION", "LESS_THAN", "LESS_THAN_EQUAL", 
      "GREATER_THAN", "GREATER_THAN_EQUAL", "BITWISE_NEGATION", "BITWISE_AND", 
      "BITWISE_AND_ASSIGN", "BITWISE_OR", "BITWISE_OR_ASSIGN", "BITWISE_XOR", 
      "BITWISE_XOR_ASSIGN", "BITWISE_SHIFT_LEFT", "BITWISE_SHIFT_LEFT_ASSIGN", 
      "BITWISE_SHIFT_RIGHT_ASSIGN", "LOOP_WHILE", "LOOP_FOR", "LOOP_DO", 
      "CONDITIONAL", "CONDITIONAL_ELSE", "BREAK", "CONTINUE", "EXIT", "DECL_CHAR", 
      "DECL_INT32", "DECL_INT64", "DECL_UINT32", "DECL_UINT64", "DECL_FLOAT32", 
      "DECL_FLOAT64", "DECL_BOOL", "DECL_STRING", "PAREN_OPEN", "PAREN_CLOSE", 
      "SCOPE_OPEN", "SCOPE_CLOSE", "SUBSCRIPT_OPEN", "SUBSCRIPT_CLOSE", 
      "OBJECT_TO_MEMBER", "PARAM_DELIMITER", "COLON", "TERNARY_CONDITIONAL", 
      "NULL_COALESCING", "STATEMENT_DELIMITER", "API_GLOBAL", "API_TYPE", 
      "API_IMPORT", "API_READONLY", "API_VOID", "API_VARIADIC_PARAMETERS", 
      "LITERAL_NULL", "LITERAL_TRUE", "LITERAL_FALSE", "LITERAL_INT", "LITERAL_FLOAT", 
      "DIGITS", "LITERAL_STRING", "STRING_ESC", "METADATA_OPEN", "IDENTIFIER", 
      "BLOCK_COMMENT", "LINE_COMMENT", "WHITESPACE", "BITWISE_SHIFT_RIGHT", 
      "METADATA_INTERPOLATION", "METADATA_CLOSE", "METADATA_CHAR"
    },
    std::vector<std::string>{
      "DEFAULT_TOKEN_CHANNEL", "HIDDEN"
    },
    std::vector<std::string>{
      "DEFAULT_MODE", "METADATA"
    },
    std::vector<std::string>{
      "", "'='", "'+'", "'+='", "'-'", "'-='", "'/'", "'/='", "'*'", "'*='", 
      "'%'", "'%='", "'++'", "'--'", "'=='", "'!='", "'&&'", "'||'", "'!'", 
      "'<'", "'<='", "'>'", "'>='", "'~'", "'&'", "'&='", "'|'", "'|='", 
      "'^'", "'^='", "'<<'", "'<<='", "'>>='", "'while'", "'for'", "'do'", 
      "'if'", "'else'", "'break'", "'continue'", "'exit'", "'char'", "'int32'", 
      "'int64'", "'uint32'", "'uint64'", "'float32'", "'float64'", "'bool'", 
      "'string'", "'('", "')'", "'{'", "'}'", "'['", "']'", "'.'", "','", 
      "':'", "'\\u003F'", "'\\u003F\\u003F'", "';'", "'#global'", "'#type'", 
      "'#import'", "'readonly'", "'void'", "'...'", "'NULL'", "'true'", 
      "'false'", "", "", "", "'<\\u003F'", "", "", "", "", "'`'", "'${'", 
      "'\\u003F>'"
    },
    std::vector<std::string>{
      "", "ASSIGN", "ADD", "ADD_ASSIGN", "SUBTRACT", "SUBTRACT_ASSIGN", 
      "DIVIDE", "DIVIDE_ASSIGN", "MULTIPLY", "MULTIPLY_ASSIGN", "MOD", "MOD_ASSIGN", 
      "INCREMENT", "DECREMENT", "LOGICAL_EQUALITY", "LOGICAL_INEQUALITY", 
      "LOGICAL_AND", "LOGICAL_OR", "LOGICAL_NEGATION", "LESS_THAN", "LESS_THAN_EQUAL", 
      "GREATER_THAN", "GREATER_THAN_EQUAL", "BITWISE_NEGATION", "BITWISE_AND", 
      "BITWISE_AND_ASSIGN", "BITWISE_OR", "BITWISE_OR_ASSIGN", "BITWISE_XOR", 
      "BITWISE_XOR_ASSIGN", "BITWISE_SHIFT_LEFT", "BITWISE_SHIFT_LEFT_ASSIGN", 
      "BITWISE_SHIFT_RIGHT_ASSIGN", "LOOP_WHILE", "LOOP_FOR", "LOOP_DO", 
      "CONDITIONAL", "CONDITIONAL_ELSE", "BREAK", "CONTINUE", "EXIT", "DECL_CHAR", 
      "DECL_INT32", "DECL_INT64", "DECL_UINT32", "DECL_UINT64", "DECL_FLOAT32", 
      "DECL_FLOAT64", "DECL_BOOL", "DECL_STRING", "PAREN_OPEN", "PAREN_CLOSE", 
      "SCOPE_OPEN", "SCOPE_CLOSE", "SUBSCRIPT_OPEN", "SUBSCRIPT_CLOSE", 
      "OBJECT_TO_MEMBER", "PARAM_DELIMITER", "COLON", "TERNARY_CONDITIONAL", 
      "NULL_COALESCING", "STATEMENT_DELIMITER", "API_GLOBAL", "API_TYPE", 
      "API_IMPORT", "API_READONLY", "API_VOID", "API_VARIADIC_PARAMETERS", 
      "LITERAL_NULL", "LITERAL_TRUE", "LITERAL_FALSE", "LITERAL_INT", "LITERAL_FLOAT", 
      "LITERAL_STRING", "METADATA_OPEN", "IDENTIFIER", "BLOCK_COMMENT", 
      "LINE_COMMENT", "WHITESPACE", "BITWISE_SHIFT_RIGHT", "METADATA_INTERPOLATION", 
      "METADATA_CLOSE", "METADATA_CHAR"
    }
  );
  static const int32_t serializedATNSegment[] = {
  	4,0,82,534,6,-1,6,-1,2,0,7,0,2,1,7,1,2,2,7,2,2,3,7,3,2,4,7,4,2,5,7,5,
  	2,6,7,6,2,7,7,7,2,8,7,8,2,9,7,9,2,10,7,10,2,11,7,11,2,12,7,12,2,13,7,
  	13,2,14,7,14,2,15,7,15,2,16,7,16,2,17,7,17,2,18,7,18,2,19,7,19,2,20,7,
  	20,2,21,7,21,2,22,7,22,2,23,7,23,2,24,7,24,2,25,7,25,2,26,7,26,2,27,7,
  	27,2,28,7,28,2,29,7,29,2,30,7,30,2,31,7,31,2,32,7,32,2,33,7,33,2,34,7,
  	34,2,35,7,35,2,36,7,36,2,37,7,37,2,38,7,38,2,39,7,39,2,40,7,40,2,41,7,
  	41,2,42,7,42,2,43,7,43,2,44,7,44,2,45,7,45,2,46,7,46,2,47,7,47,2,48,7,
  	48,2,49,7,49,2,50,7,50,2,51,7,51,2,52,7,52,2,53,7,53,2,54,7,54,2,55,7,
  	55,2,56,7,56,2,57,7,57,2,58,7,58,2,59,7,59,2,60,7,60,2,61,7,61,2,62,7,
  	62,2,63,7,63,2,64,7,64,2,65,7,65,2,66,7,66,2,67,7,67,2,68,7,68,2,69,7,
  	69,2,70,7,70,2,71,7,71,2,72,7,72,2,73,7,73,2,74,7,74,2,75,7,75,2,76,7,
  	76,2,77,7,77,2,78,7,78,2,79,7,79,2,80,7,80,2,81,7,81,2,82,7,82,2,83,7,
  	83,1,0,1,0,1,1,1,1,1,2,1,2,1,2,1,3,1,3,1,4,1,4,1,4,1,5,1,5,1,6,1,6,1,
  	6,1,7,1,7,1,8,1,8,1,8,1,9,1,9,1,10,1,10,1,10,1,11,1,11,1,11,1,12,1,12,
  	1,12,1,13,1,13,1,13,1,14,1,14,1,14,1,15,1,15,1,15,1,16,1,16,1,16,1,17,
  	1,17,1,18,1,18,1,19,1,19,1,19,1,20,1,20,1,21,1,21,1,21,1,22,1,22,1,23,
  	1,23,1,24,1,24,1,24,1,25,1,25,1,26,1,26,1,26,1,27,1,27,1,28,1,28,1,28,
  	1,29,1,29,1,29,1,30,1,30,1,30,1,30,1,31,1,31,1,31,1,31,1,32,1,32,1,32,
  	1,32,1,32,1,32,1,33,1,33,1,33,1,33,1,34,1,34,1,34,1,35,1,35,1,35,1,36,
  	1,36,1,36,1,36,1,36,1,37,1,37,1,37,1,37,1,37,1,37,1,38,1,38,1,38,1,38,
  	1,38,1,38,1,38,1,38,1,38,1,39,1,39,1,39,1,39,1,39,1,40,1,40,1,40,1,40,
  	1,40,1,41,1,41,1,41,1,41,1,41,1,41,1,42,1,42,1,42,1,42,1,42,1,42,1,43,
  	1,43,1,43,1,43,1,43,1,43,1,43,1,44,1,44,1,44,1,44,1,44,1,44,1,44,1,45,
  	1,45,1,45,1,45,1,45,1,45,1,45,1,45,1,46,1,46,1,46,1,46,1,46,1,46,1,46,
  	1,46,1,47,1,47,1,47,1,47,1,47,1,48,1,48,1,48,1,48,1,48,1,48,1,48,1,49,
  	1,49,1,50,1,50,1,51,1,51,1,51,1,51,1,52,1,52,1,52,1,52,1,53,1,53,1,54,
  	1,54,1,55,1,55,1,56,1,56,1,57,1,57,1,58,1,58,1,59,1,59,1,59,1,60,1,60,
  	1,61,1,61,1,61,1,61,1,61,1,61,1,61,1,61,1,62,1,62,1,62,1,62,1,62,1,62,
  	1,63,1,63,1,63,1,63,1,63,1,63,1,63,1,63,1,64,1,64,1,64,1,64,1,64,1,64,
  	1,64,1,64,1,64,1,65,1,65,1,65,1,65,1,65,1,66,1,66,1,66,1,66,1,67,1,67,
  	1,67,1,67,1,67,1,68,1,68,1,68,1,68,1,68,1,69,1,69,1,69,1,69,1,69,1,69,
  	1,70,1,70,1,71,3,71,444,8,71,1,71,1,71,1,71,1,72,4,72,450,8,72,11,72,
  	12,72,451,1,73,1,73,1,73,5,73,457,8,73,10,73,12,73,460,9,73,1,73,1,73,
  	1,74,1,74,1,74,1,74,3,74,468,8,74,1,75,1,75,1,75,1,75,1,75,1,76,1,76,
  	5,76,477,8,76,10,76,12,76,480,9,76,1,77,1,77,1,77,1,77,5,77,486,8,77,
  	10,77,12,77,489,9,77,1,77,1,77,1,77,1,77,1,77,1,78,1,78,1,78,1,78,5,78,
  	500,8,78,10,78,12,78,503,9,78,1,78,3,78,506,8,78,1,78,1,78,1,78,1,78,
  	1,79,4,79,513,8,79,11,79,12,79,514,1,79,1,79,1,80,1,80,1,81,1,81,1,81,
  	1,81,1,81,1,82,1,82,1,82,1,82,1,82,1,83,1,83,1,83,1,83,3,458,487,501,
  	0,84,2,1,4,2,6,3,8,4,10,5,12,6,14,7,16,8,18,9,20,10,22,11,24,12,26,13,
  	28,14,30,15,32,16,34,17,36,18,38,19,40,20,42,21,44,22,46,23,48,24,50,
  	25,52,26,54,27,56,28,58,29,60,30,62,31,64,32,66,33,68,34,70,35,72,36,
  	74,37,76,38,78,39,80,40,82,41,84,42,86,43,88,44,90,45,92,46,94,47,96,
  	48,98,49,100,50,102,51,104,52,106,53,108,54,110,55,112,56,114,57,116,
  	58,118,59,120,60,122,61,124,62,126,63,128,64,130,65,132,66,134,67,136,
  	68,138,69,140,70,142,71,144,72,146,0,148,73,150,0,152,74,154,75,156,76,
  	158,77,160,78,162,79,164,80,166,81,168,82,2,0,1,4,1,0,48,57,3,0,65,90,
  	95,95,97,122,4,0,48,57,65,90,95,95,97,122,3,0,9,10,13,13,32,32,540,0,
  	2,1,0,0,0,0,4,1,0,0,0,0,6,1,0,0,0,0,8,1,0,0,0,0,10,1,0,0,0,0,12,1,0,0,
  	0,0,14,1,0,0,0,0,16,1,0,0,0,0,18,1,0,0,0,0,20,1,0,0,0,0,22,1,0,0,0,0,
  	24,1,0,0,0,0,26,1,0,0,0,0,28,1,0,0,0,0,30,1,0,0,0,0,32,1,0,0,0,0,34,1,
  	0,0,0,0,36,1,0,0,0,0,38,1,0,0,0,0,40,1,0,0,0,0,42,1,0,0,0,0,44,1,0,0,
  	0,0,46,1,0,0,0,0,48,1,0,0,0,0,50,1,0,0,0,0,52,1,0,0,0,0,54,1,0,0,0,0,
  	56,1,0,0,0,0,58,1,0,0,0,0,60,1,0,0,0,0,62,1,0,0,0,0,64,1,0,0,0,0,66,1,
  	0,0,0,0,68,1,0,0,0,0,70,1,0,0,0,0,72,1,0,0,0,0,74,1,0,0,0,0,76,1,0,0,
  	0,0,78,1,0,0,0,0,80,1,0,0,0,0,82,1,0,0,0,0,84,1,0,0,0,0,86,1,0,0,0,0,
  	88,1,0,0,0,0,90,1,0,0,0,0,92,1,0,0,0,0,94,1,0,0,0,0,96,1,0,0,0,0,98,1,
  	0,0,0,0,100,1,0,0,0,0,102,1,0,0,0,0,104,1,0,0,0,0,106,1,0,0,0,0,108,1,
  	0,0,0,0,110,1,0,0,0,0,112,1,0,0,0,0,114,1,0,0,0,0,116,1,0,0,0,0,118,1,
  	0,0,0,0,120,1,0,0,0,0,122,1,0,0,0,0,124,1,0,0,0,0,126,1,0,0,0,0,128,1,
  	0,0,0,0,130,1,0,0,0,0,132,1,0,0,0,0,134,1,0,0,0,0,136,1,0,0,0,0,138,1,
  	0,0,0,0,140,1,0,0,0,0,142,1,0,0,0,0,144,1,0,0,0,0,148,1,0,0,0,0,152,1,
  	0,0,0,0,154,1,0,0,0,0,156,1,0,0,0,0,158,1,0,0,0,0,160,1,0,0,0,0,162,1,
  	0,0,0,1,164,1,0,0,0,1,166,1,0,0,0,1,168,1,0,0,0,2,170,1,0,0,0,4,172,1,
  	0,0,0,6,174,1,0,0,0,8,177,1,0,0,0,10,179,1,0,0,0,12,182,1,0,0,0,14,184,
  	1,0,0,0,16,187,1,0,0,0,18,189,1,0,0,0,20,192,1,0,0,0,22,194,1,0,0,0,24,
  	197,1,0,0,0,26,200,1,0,0,0,28,203,1,0,0,0,30,206,1,0,0,0,32,209,1,0,0,
  	0,34,212,1,0,0,0,36,215,1,0,0,0,38,217,1,0,0,0,40,219,1,0,0,0,42,222,
  	1,0,0,0,44,224,1,0,0,0,46,227,1,0,0,0,48,229,1,0,0,0,50,231,1,0,0,0,52,
  	234,1,0,0,0,54,236,1,0,0,0,56,239,1,0,0,0,58,241,1,0,0,0,60,244,1,0,0,
  	0,62,247,1,0,0,0,64,251,1,0,0,0,66,255,1,0,0,0,68,261,1,0,0,0,70,265,
  	1,0,0,0,72,268,1,0,0,0,74,271,1,0,0,0,76,276,1,0,0,0,78,282,1,0,0,0,80,
  	291,1,0,0,0,82,296,1,0,0,0,84,301,1,0,0,0,86,307,1,0,0,0,88,313,1,0,0,
  	0,90,320,1,0,0,0,92,327,1,0,0,0,94,335,1,0,0,0,96,343,1,0,0,0,98,348,
  	1,0,0,0,100,355,1,0,0,0,102,357,1,0,0,0,104,359,1,0,0,0,106,363,1,0,0,
  	0,108,367,1,0,0,0,110,369,1,0,0,0,112,371,1,0,0,0,114,373,1,0,0,0,116,
  	375,1,0,0,0,118,377,1,0,0,0,120,379,1,0,0,0,122,382,1,0,0,0,124,384,1,
  	0,0,0,126,392,1,0,0,0,128,398,1,0,0,0,130,406,1,0,0,0,132,415,1,0,0,0,
  	134,420,1,0,0,0,136,424,1,0,0,0,138,429,1,0,0,0,140,434,1,0,0,0,142,440,
  	1,0,0,0,144,443,1,0,0,0,146,449,1,0,0,0,148,453,1,0,0,0,150,467,1,0,0,
  	0,152,469,1,0,0,0,154,474,1,0,0,0,156,481,1,0,0,0,158,495,1,0,0,0,160,
  	512,1,0,0,0,162,518,1,0,0,0,164,520,1,0,0,0,166,525,1,0,0,0,168,530,1,
  	0,0,0,170,171,5,61,0,0,171,3,1,0,0,0,172,173,5,43,0,0,173,5,1,0,0,0,174,
  	175,5,43,0,0,175,176,5,61,0,0,176,7,1,0,0,0,177,178,5,45,0,0,178,9,1,
  	0,0,0,179,180,5,45,0,0,180,181,5,61,0,0,181,11,1,0,0,0,182,183,5,47,0,
  	0,183,13,1,0,0,0,184,185,5,47,0,0,185,186,5,61,0,0,186,15,1,0,0,0,187,
  	188,5,42,0,0,188,17,1,0,0,0,189,190,5,42,0,0,190,191,5,61,0,0,191,19,
  	1,0,0,0,192,193,5,37,0,0,193,21,1,0,0,0,194,195,5,37,0,0,195,196,5,61,
  	0,0,196,23,1,0,0,0,197,198,5,43,0,0,198,199,5,43,0,0,199,25,1,0,0,0,200,
  	201,5,45,0,0,201,202,5,45,0,0,202,27,1,0,0,0,203,204,5,61,0,0,204,205,
  	5,61,0,0,205,29,1,0,0,0,206,207,5,33,0,0,207,208,5,61,0,0,208,31,1,0,
  	0,0,209,210,5,38,0,0,210,211,5,38,0,0,211,33,1,0,0,0,212,213,5,124,0,
  	0,213,214,5,124,0,0,214,35,1,0,0,0,215,216,5,33,0,0,216,37,1,0,0,0,217,
  	218,5,60,0,0,218,39,1,0,0,0,219,220,5,60,0,0,220,221,5,61,0,0,221,41,
  	1,0,0,0,222,223,5,62,0,0,223,43,1,0,0,0,224,225,5,62,0,0,225,226,5,61,
  	0,0,226,45,1,0,0,0,227,228,5,126,0,0,228,47,1,0,0,0,229,230,5,38,0,0,
  	230,49,1,0,0,0,231,232,5,38,0,0,232,233,5,61,0,0,233,51,1,0,0,0,234,235,
  	5,124,0,0,235,53,1,0,0,0,236,237,5,124,0,0,237,238,5,61,0,0,238,55,1,
  	0,0,0,239,240,5,94,0,0,240,57,1,0,0,0,241,242,5,94,0,0,242,243,5,61,0,
  	0,243,59,1,0,0,0,244,245,5,60,0,0,245,246,5,60,0,0,246,61,1,0,0,0,247,
  	248,5,60,0,0,248,249,5,60,0,0,249,250,5,61,0,0,250,63,1,0,0,0,251,252,
  	5,62,0,0,252,253,5,62,0,0,253,254,5,61,0,0,254,65,1,0,0,0,255,256,5,119,
  	0,0,256,257,5,104,0,0,257,258,5,105,0,0,258,259,5,108,0,0,259,260,5,101,
  	0,0,260,67,1,0,0,0,261,262,5,102,0,0,262,263,5,111,0,0,263,264,5,114,
  	0,0,264,69,1,0,0,0,265,266,5,100,0,0,266,267,5,111,0,0,267,71,1,0,0,0,
  	268,269,5,105,0,0,269,270,5,102,0,0,270,73,1,0,0,0,271,272,5,101,0,0,
  	272,273,5,108,0,0,273,274,5,115,0,0,274,275,5,101,0,0,275,75,1,0,0,0,
  	276,277,5,98,0,0,277,278,5,114,0,0,278,279,5,101,0,0,279,280,5,97,0,0,
  	280,281,5,107,0,0,281,77,1,0,0,0,282,283,5,99,0,0,283,284,5,111,0,0,284,
  	285,5,110,0,0,285,286,5,116,0,0,286,287,5,105,0,0,287,288,5,110,0,0,288,
  	289,5,117,0,0,289,290,5,101,0,0,290,79,1,0,0,0,291,292,5,101,0,0,292,
  	293,5,120,0,0,293,294,5,105,0,0,294,295,5,116,0,0,295,81,1,0,0,0,296,
  	297,5,99,0,0,297,298,5,104,0,0,298,299,5,97,0,0,299,300,5,114,0,0,300,
  	83,1,0,0,0,301,302,5,105,0,0,302,303,5,110,0,0,303,304,5,116,0,0,304,
  	305,5,51,0,0,305,306,5,50,0,0,306,85,1,0,0,0,307,308,5,105,0,0,308,309,
  	5,110,0,0,309,310,5,116,0,0,310,311,5,54,0,0,311,312,5,52,0,0,312,87,
  	1,0,0,0,313,314,5,117,0,0,314,315,5,105,0,0,315,316,5,110,0,0,316,317,
  	5,116,0,0,317,318,5,51,0,0,318,319,5,50,0,0,319,89,1,0,0,0,320,321,5,
  	117,0,0,321,322,5,105,0,0,322,323,5,110,0,0,323,324,5,116,0,0,324,325,
  	5,54,0,0,325,326,5,52,0,0,326,91,1,0,0,0,327,328,5,102,0,0,328,329,5,
  	108,0,0,329,330,5,111,0,0,330,331,5,97,0,0,331,332,5,116,0,0,332,333,
  	5,51,0,0,333,334,5,50,0,0,334,93,1,0,0,0,335,336,5,102,0,0,336,337,5,
  	108,0,0,337,338,5,111,0,0,338,339,5,97,0,0,339,340,5,116,0,0,340,341,
  	5,54,0,0,341,342,5,52,0,0,342,95,1,0,0,0,343,344,5,98,0,0,344,345,5,111,
  	0,0,345,346,5,111,0,0,346,347,5,108,0,0,347,97,1,0,0,0,348,349,5,115,
  	0,0,349,350,5,116,0,0,350,351,5,114,0,0,351,352,5,105,0,0,352,353,5,110,
  	0,0,353,354,5,103,0,0,354,99,1,0,0,0,355,356,5,40,0,0,356,101,1,0,0,0,
  	357,358,5,41,0,0,358,103,1,0,0,0,359,360,5,123,0,0,360,361,1,0,0,0,361,
  	362,6,51,0,0,362,105,1,0,0,0,363,364,5,125,0,0,364,365,1,0,0,0,365,366,
  	6,52,1,0,366,107,1,0,0,0,367,368,5,91,0,0,368,109,1,0,0,0,369,370,5,93,
  	0,0,370,111,1,0,0,0,371,372,5,46,0,0,372,113,1,0,0,0,373,374,5,44,0,0,
  	374,115,1,0,0,0,375,376,5,58,0,0,376,117,1,0,0,0,377,378,5,63,0,0,378,
  	119,1,0,0,0,379,380,5,63,0,0,380,381,5,63,0,0,381,121,1,0,0,0,382,383,
  	5,59,0,0,383,123,1,0,0,0,384,385,5,35,0,0,385,386,5,103,0,0,386,387,5,
  	108,0,0,387,388,5,111,0,0,388,389,5,98,0,0,389,390,5,97,0,0,390,391,5,
  	108,0,0,391,125,1,0,0,0,392,393,5,35,0,0,393,394,5,116,0,0,394,395,5,
  	121,0,0,395,396,5,112,0,0,396,397,5,101,0,0,397,127,1,0,0,0,398,399,5,
  	35,0,0,399,400,5,105,0,0,400,401,5,109,0,0,401,402,5,112,0,0,402,403,
  	5,111,0,0,403,404,5,114,0,0,404,405,5,116,0,0,405,129,1,0,0,0,406,407,
  	5,114,0,0,407,408,5,101,0,0,408,409,5,97,0,0,409,410,5,100,0,0,410,411,
  	5,111,0,0,411,412,5,110,0,0,412,413,5,108,0,0,413,414,5,121,0,0,414,131,
  	1,0,0,0,415,416,5,118,0,0,416,417,5,111,0,0,417,418,5,105,0,0,418,419,
  	5,100,0,0,419,133,1,0,0,0,420,421,5,46,0,0,421,422,5,46,0,0,422,423,5,
  	46,0,0,423,135,1,0,0,0,424,425,5,78,0,0,425,426,5,85,0,0,426,427,5,76,
  	0,0,427,428,5,76,0,0,428,137,1,0,0,0,429,430,5,116,0,0,430,431,5,114,
  	0,0,431,432,5,117,0,0,432,433,5,101,0,0,433,139,1,0,0,0,434,435,5,102,
  	0,0,435,436,5,97,0,0,436,437,5,108,0,0,437,438,5,115,0,0,438,439,5,101,
  	0,0,439,141,1,0,0,0,440,441,3,146,72,0,441,143,1,0,0,0,442,444,3,146,
  	72,0,443,442,1,0,0,0,443,444,1,0,0,0,444,445,1,0,0,0,445,446,5,46,0,0,
  	446,447,3,146,72,0,447,145,1,0,0,0,448,450,7,0,0,0,449,448,1,0,0,0,450,
  	451,1,0,0,0,451,449,1,0,0,0,451,452,1,0,0,0,452,147,1,0,0,0,453,458,5,
  	34,0,0,454,457,3,150,74,0,455,457,9,0,0,0,456,454,1,0,0,0,456,455,1,0,
  	0,0,457,460,1,0,0,0,458,459,1,0,0,0,458,456,1,0,0,0,459,461,1,0,0,0,460,
  	458,1,0,0,0,461,462,5,34,0,0,462,149,1,0,0,0,463,464,5,92,0,0,464,468,
  	5,34,0,0,465,466,5,92,0,0,466,468,5,92,0,0,467,463,1,0,0,0,467,465,1,
  	0,0,0,468,151,1,0,0,0,469,470,5,60,0,0,470,471,5,63,0,0,471,472,1,0,0,
  	0,472,473,6,75,2,0,473,153,1,0,0,0,474,478,7,1,0,0,475,477,7,2,0,0,476,
  	475,1,0,0,0,477,480,1,0,0,0,478,476,1,0,0,0,478,479,1,0,0,0,479,155,1,
  	0,0,0,480,478,1,0,0,0,481,482,5,47,0,0,482,483,5,42,0,0,483,487,1,0,0,
  	0,484,486,9,0,0,0,485,484,1,0,0,0,486,489,1,0,0,0,487,488,1,0,0,0,487,
  	485,1,0,0,0,488,490,1,0,0,0,489,487,1,0,0,0,490,491,5,42,0,0,491,492,
  	5,47,0,0,492,493,1,0,0,0,493,494,6,77,3,0,494,157,1,0,0,0,495,496,5,47,
  	0,0,496,497,5,47,0,0,497,501,1,0,0,0,498,500,9,0,0,0,499,498,1,0,0,0,
  	500,503,1,0,0,0,501,502,1,0,0,0,501,499,1,0,0,0,502,505,1,0,0,0,503,501,
  	1,0,0,0,504,506,5,13,0,0,505,504,1,0,0,0,505,506,1,0,0,0,506,507,1,0,
  	0,0,507,508,5,10,0,0,508,509,1,0,0,0,509,510,6,78,3,0,510,159,1,0,0,0,
  	511,513,7,3,0,0,512,511,1,0,0,0,513,514,1,0,0,0,514,512,1,0,0,0,514,515,
  	1,0,0,0,515,516,1,0,0,0,516,517,6,79,3,0,517,161,1,0,0,0,518,519,5,96,
  	0,0,519,163,1,0,0,0,520,521,5,36,0,0,521,522,5,123,0,0,522,523,1,0,0,
  	0,523,524,6,81,0,0,524,165,1,0,0,0,525,526,5,63,0,0,526,527,5,62,0,0,
  	527,528,1,0,0,0,528,529,6,82,1,0,529,167,1,0,0,0,530,531,9,0,0,0,531,
  	532,1,0,0,0,532,533,6,83,3,0,533,169,1,0,0,0,12,0,1,443,451,456,458,467,
  	478,487,501,505,514,4,5,0,0,4,0,0,5,1,0,6,0,0
  };
  staticData->serializedATN = antlr4::atn::SerializedATNView(serializedATNSegment, sizeof(serializedATNSegment) / sizeof(serializedATNSegment[0]));

  antlr4::atn::ATNDeserializer deserializer;
  staticData->atn = deserializer.deserialize(staticData->serializedATN);

  const size_t count = staticData->atn->getNumberOfDecisions();
  staticData->decisionToDFA.reserve(count);
  for (size_t i = 0; i < count; i++) { 
    staticData->decisionToDFA.emplace_back(staticData->atn->getDecisionState(i), i);
  }
  mapllexerLexerStaticData = std::move(staticData);
}

}

MaPLLexer::MaPLLexer(CharStream *input) : Lexer(input) {
  MaPLLexer::initialize();
  _interpreter = new atn::LexerATNSimulator(this, *mapllexerLexerStaticData->atn, mapllexerLexerStaticData->decisionToDFA, mapllexerLexerStaticData->sharedContextCache);
}

MaPLLexer::~MaPLLexer() {
  delete _interpreter;
}

std::string MaPLLexer::getGrammarFileName() const {
  return "MaPLLexer.g4";
}

const std::vector<std::string>& MaPLLexer::getRuleNames() const {
  return mapllexerLexerStaticData->ruleNames;
}

const std::vector<std::string>& MaPLLexer::getChannelNames() const {
  return mapllexerLexerStaticData->channelNames;
}

const std::vector<std::string>& MaPLLexer::getModeNames() const {
  return mapllexerLexerStaticData->modeNames;
}

const dfa::Vocabulary& MaPLLexer::getVocabulary() const {
  return mapllexerLexerStaticData->vocabulary;
}

antlr4::atn::SerializedATNView MaPLLexer::getSerializedATN() const {
  return mapllexerLexerStaticData->serializedATN;
}

const atn::ATN& MaPLLexer::getATN() const {
  return *mapllexerLexerStaticData->atn;
}




void MaPLLexer::initialize() {
#if ANTLR4_USE_THREAD_LOCAL_CACHE
  mapllexerLexerInitialize();
#else
  ::antlr4::internal::call_once(mapllexerLexerOnceFlag, mapllexerLexerInitialize);
#endif
}

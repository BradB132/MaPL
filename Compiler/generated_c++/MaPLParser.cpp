
// Generated from ./MaPLParser.g4 by ANTLR 4.13.2



#include "MaPLParser.h"


using namespace antlrcpp;

using namespace antlr4;

namespace {

struct MaPLParserStaticData final {
  MaPLParserStaticData(std::vector<std::string> ruleNames,
                        std::vector<std::string> literalNames,
                        std::vector<std::string> symbolicNames)
      : ruleNames(std::move(ruleNames)), literalNames(std::move(literalNames)),
        symbolicNames(std::move(symbolicNames)),
        vocabulary(this->literalNames, this->symbolicNames) {}

  MaPLParserStaticData(const MaPLParserStaticData&) = delete;
  MaPLParserStaticData(MaPLParserStaticData&&) = delete;
  MaPLParserStaticData& operator=(const MaPLParserStaticData&) = delete;
  MaPLParserStaticData& operator=(MaPLParserStaticData&&) = delete;

  std::vector<antlr4::dfa::DFA> decisionToDFA;
  antlr4::atn::PredictionContextCache sharedContextCache;
  const std::vector<std::string> ruleNames;
  const std::vector<std::string> literalNames;
  const std::vector<std::string> symbolicNames;
  const antlr4::dfa::Vocabulary vocabulary;
  antlr4::atn::SerializedATNView serializedATN;
  std::unique_ptr<antlr4::atn::ATN> atn;
};

::antlr4::internal::OnceFlag maplparserParserOnceFlag;
#if ANTLR4_USE_THREAD_LOCAL_CACHE
static thread_local
#endif
std::unique_ptr<MaPLParserStaticData> maplparserParserStaticData = nullptr;

void maplparserParserInitialize() {
#if ANTLR4_USE_THREAD_LOCAL_CACHE
  if (maplparserParserStaticData != nullptr) {
    return;
  }
#else
  assert(maplparserParserStaticData == nullptr);
#endif
  auto staticData = std::make_unique<MaPLParserStaticData>(
    std::vector<std::string>{
      "program", "statement", "imperativeStatement", "metadataStatement", 
      "assignStatement", "unaryStatement", "expression", "objectExpression", 
      "variableDeclaration", "type", "pointerType", "scope", "whileLoop", 
      "forLoop", "forLoopControlStatements", "doWhileLoop", "conditional", 
      "conditionalElse", "apiGlobal", "apiType", "apiInheritance", "apiFunction", 
      "apiFunctionParams", "apiProperty", "apiSubscript", "apiImport", "identifier", 
      "bitwiseShiftRight"
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
  	4,1,82,380,2,0,7,0,2,1,7,1,2,2,7,2,2,3,7,3,2,4,7,4,2,5,7,5,2,6,7,6,2,
  	7,7,7,2,8,7,8,2,9,7,9,2,10,7,10,2,11,7,11,2,12,7,12,2,13,7,13,2,14,7,
  	14,2,15,7,15,2,16,7,16,2,17,7,17,2,18,7,18,2,19,7,19,2,20,7,20,2,21,7,
  	21,2,22,7,22,2,23,7,23,2,24,7,24,2,25,7,25,2,26,7,26,2,27,7,27,1,0,5,
  	0,58,8,0,10,0,12,0,61,9,0,1,0,1,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
  	1,1,1,1,1,1,3,1,77,8,1,1,2,1,2,1,2,1,2,1,2,1,2,1,2,3,2,86,8,2,1,3,1,3,
  	1,3,1,3,1,3,5,3,93,8,3,10,3,12,3,96,9,3,1,3,1,3,1,4,1,4,1,4,1,4,1,5,1,
  	5,1,5,1,6,1,6,1,6,1,6,1,6,1,6,1,6,1,6,1,6,1,6,1,6,1,6,1,6,1,6,1,6,1,6,
  	1,6,1,6,1,6,3,6,126,8,6,1,6,1,6,1,6,1,6,1,6,1,6,1,6,1,6,1,6,1,6,1,6,1,
  	6,3,6,140,8,6,1,6,1,6,1,6,1,6,1,6,1,6,1,6,1,6,1,6,1,6,1,6,1,6,1,6,1,6,
  	1,6,1,6,1,6,1,6,1,6,5,6,161,8,6,10,6,12,6,164,9,6,1,7,1,7,1,7,1,7,1,7,
  	1,7,5,7,172,8,7,10,7,12,7,175,9,7,3,7,177,8,7,1,7,3,7,180,8,7,1,7,1,7,
  	1,7,1,7,1,7,1,7,1,7,1,7,5,7,190,8,7,10,7,12,7,193,9,7,1,8,1,8,1,8,1,8,
  	3,8,199,8,8,1,9,1,9,1,9,1,9,1,9,1,9,1,9,1,9,1,9,1,9,3,9,211,8,9,1,10,
  	1,10,1,10,1,10,1,10,5,10,218,8,10,10,10,12,10,221,9,10,1,10,1,10,3,10,
  	225,8,10,1,11,1,11,5,11,229,8,11,10,11,12,11,232,9,11,1,11,1,11,1,12,
  	1,12,1,12,1,12,1,13,1,13,1,13,1,13,1,13,1,13,1,13,1,13,1,13,1,13,3,13,
  	250,8,13,1,14,3,14,253,8,14,1,14,1,14,3,14,257,8,14,1,14,1,14,3,14,261,
  	8,14,1,15,1,15,1,15,1,15,1,15,1,15,1,16,1,16,1,16,1,16,3,16,273,8,16,
  	1,17,1,17,1,17,3,17,278,8,17,1,18,1,18,1,18,3,18,283,8,18,1,18,1,18,1,
  	19,1,19,1,19,1,19,1,19,1,19,5,19,293,8,19,10,19,12,19,296,9,19,1,19,1,
  	19,3,19,300,8,19,1,19,3,19,303,8,19,1,19,1,19,1,19,1,19,3,19,309,8,19,
  	1,19,1,19,5,19,313,8,19,10,19,12,19,316,9,19,1,19,1,19,1,20,1,20,1,20,
  	1,20,5,20,324,8,20,10,20,12,20,327,9,20,1,21,1,21,3,21,331,8,21,1,21,
  	1,21,1,21,3,21,336,8,21,1,21,1,21,1,22,1,22,1,22,1,22,1,22,1,22,1,22,
  	5,22,347,8,22,10,22,12,22,350,9,22,1,22,1,22,3,22,354,8,22,3,22,356,8,
  	22,1,23,3,23,359,8,23,1,23,1,23,1,23,1,24,3,24,365,8,24,1,24,1,24,1,24,
  	1,24,1,24,1,25,1,25,1,25,1,26,1,26,1,27,1,27,1,27,1,27,0,2,12,14,28,0,
  	2,4,6,8,10,12,14,16,18,20,22,24,26,28,30,32,34,36,38,40,42,44,46,48,50,
  	52,54,0,9,10,0,1,1,3,3,5,5,7,7,9,9,11,11,25,25,27,27,29,29,31,32,1,0,
  	12,13,3,0,4,4,18,18,23,23,2,0,6,6,8,8,2,0,2,2,4,4,3,0,24,24,26,26,28,
  	28,2,0,14,15,19,22,1,0,16,17,4,0,33,49,65,66,68,70,75,75,426,0,59,1,0,
  	0,0,2,76,1,0,0,0,4,85,1,0,0,0,6,87,1,0,0,0,8,99,1,0,0,0,10,103,1,0,0,
  	0,12,125,1,0,0,0,14,165,1,0,0,0,16,194,1,0,0,0,18,210,1,0,0,0,20,212,
  	1,0,0,0,22,226,1,0,0,0,24,235,1,0,0,0,26,249,1,0,0,0,28,252,1,0,0,0,30,
  	262,1,0,0,0,32,268,1,0,0,0,34,274,1,0,0,0,36,279,1,0,0,0,38,286,1,0,0,
  	0,40,319,1,0,0,0,42,330,1,0,0,0,44,355,1,0,0,0,46,358,1,0,0,0,48,364,
  	1,0,0,0,50,371,1,0,0,0,52,374,1,0,0,0,54,376,1,0,0,0,56,58,3,2,1,0,57,
  	56,1,0,0,0,58,61,1,0,0,0,59,57,1,0,0,0,59,60,1,0,0,0,60,62,1,0,0,0,61,
  	59,1,0,0,0,62,63,5,0,0,1,63,1,1,0,0,0,64,65,3,4,2,0,65,66,5,61,0,0,66,
  	77,1,0,0,0,67,77,3,24,12,0,68,77,3,26,13,0,69,77,3,30,15,0,70,77,3,32,
  	16,0,71,77,3,36,18,0,72,77,3,38,19,0,73,77,3,50,25,0,74,77,3,6,3,0,75,
  	77,3,22,11,0,76,64,1,0,0,0,76,67,1,0,0,0,76,68,1,0,0,0,76,69,1,0,0,0,
  	76,70,1,0,0,0,76,71,1,0,0,0,76,72,1,0,0,0,76,73,1,0,0,0,76,74,1,0,0,0,
  	76,75,1,0,0,0,77,3,1,0,0,0,78,86,5,38,0,0,79,86,5,39,0,0,80,86,5,40,0,
  	0,81,86,3,16,8,0,82,86,3,8,4,0,83,86,3,10,5,0,84,86,3,14,7,0,85,78,1,
  	0,0,0,85,79,1,0,0,0,85,80,1,0,0,0,85,81,1,0,0,0,85,82,1,0,0,0,85,83,1,
  	0,0,0,85,84,1,0,0,0,86,5,1,0,0,0,87,94,5,74,0,0,88,89,5,80,0,0,89,90,
  	3,12,6,0,90,91,5,53,0,0,91,93,1,0,0,0,92,88,1,0,0,0,93,96,1,0,0,0,94,
  	92,1,0,0,0,94,95,1,0,0,0,95,97,1,0,0,0,96,94,1,0,0,0,97,98,5,81,0,0,98,
  	7,1,0,0,0,99,100,3,14,7,0,100,101,7,0,0,0,101,102,3,12,6,0,102,9,1,0,
  	0,0,103,104,3,14,7,0,104,105,7,1,0,0,105,11,1,0,0,0,106,107,6,6,-1,0,
  	107,108,5,50,0,0,108,109,3,18,9,0,109,110,5,51,0,0,110,111,3,12,6,19,
  	111,126,1,0,0,0,112,113,7,2,0,0,113,126,3,12,6,18,114,115,5,50,0,0,115,
  	116,3,12,6,0,116,117,5,51,0,0,117,126,1,0,0,0,118,126,5,69,0,0,119,126,
  	5,70,0,0,120,126,5,68,0,0,121,126,5,71,0,0,122,126,5,72,0,0,123,126,5,
  	73,0,0,124,126,3,14,7,0,125,106,1,0,0,0,125,112,1,0,0,0,125,114,1,0,0,
  	0,125,118,1,0,0,0,125,119,1,0,0,0,125,120,1,0,0,0,125,121,1,0,0,0,125,
  	122,1,0,0,0,125,123,1,0,0,0,125,124,1,0,0,0,126,162,1,0,0,0,127,128,10,
  	17,0,0,128,129,5,10,0,0,129,161,3,12,6,18,130,131,10,16,0,0,131,132,7,
  	3,0,0,132,161,3,12,6,17,133,134,10,15,0,0,134,135,7,4,0,0,135,161,3,12,
  	6,16,136,139,10,14,0,0,137,140,5,30,0,0,138,140,3,54,27,0,139,137,1,0,
  	0,0,139,138,1,0,0,0,140,141,1,0,0,0,141,161,3,12,6,15,142,143,10,13,0,
  	0,143,144,7,5,0,0,144,161,3,12,6,14,145,146,10,12,0,0,146,147,5,60,0,
  	0,147,161,3,12,6,12,148,149,10,11,0,0,149,150,7,6,0,0,150,161,3,12,6,
  	12,151,152,10,10,0,0,152,153,7,7,0,0,153,161,3,12,6,11,154,155,10,9,0,
  	0,155,156,5,59,0,0,156,157,3,12,6,0,157,158,5,58,0,0,158,159,3,12,6,9,
  	159,161,1,0,0,0,160,127,1,0,0,0,160,130,1,0,0,0,160,133,1,0,0,0,160,136,
  	1,0,0,0,160,142,1,0,0,0,160,145,1,0,0,0,160,148,1,0,0,0,160,151,1,0,0,
  	0,160,154,1,0,0,0,161,164,1,0,0,0,162,160,1,0,0,0,162,163,1,0,0,0,163,
  	13,1,0,0,0,164,162,1,0,0,0,165,166,6,7,-1,0,166,179,3,52,26,0,167,176,
  	5,50,0,0,168,173,3,12,6,0,169,170,5,57,0,0,170,172,3,12,6,0,171,169,1,
  	0,0,0,172,175,1,0,0,0,173,171,1,0,0,0,173,174,1,0,0,0,174,177,1,0,0,0,
  	175,173,1,0,0,0,176,168,1,0,0,0,176,177,1,0,0,0,177,178,1,0,0,0,178,180,
  	5,51,0,0,179,167,1,0,0,0,179,180,1,0,0,0,180,191,1,0,0,0,181,182,10,3,
  	0,0,182,183,5,56,0,0,183,190,3,14,7,4,184,185,10,2,0,0,185,186,5,54,0,
  	0,186,187,3,12,6,0,187,188,5,55,0,0,188,190,1,0,0,0,189,181,1,0,0,0,189,
  	184,1,0,0,0,190,193,1,0,0,0,191,189,1,0,0,0,191,192,1,0,0,0,192,15,1,
  	0,0,0,193,191,1,0,0,0,194,195,3,18,9,0,195,198,3,52,26,0,196,197,5,1,
  	0,0,197,199,3,12,6,0,198,196,1,0,0,0,198,199,1,0,0,0,199,17,1,0,0,0,200,
  	211,5,41,0,0,201,211,5,42,0,0,202,211,5,43,0,0,203,211,5,44,0,0,204,211,
  	5,45,0,0,205,211,5,46,0,0,206,211,5,47,0,0,207,211,5,48,0,0,208,211,5,
  	49,0,0,209,211,3,20,10,0,210,200,1,0,0,0,210,201,1,0,0,0,210,202,1,0,
  	0,0,210,203,1,0,0,0,210,204,1,0,0,0,210,205,1,0,0,0,210,206,1,0,0,0,210,
  	207,1,0,0,0,210,208,1,0,0,0,210,209,1,0,0,0,211,19,1,0,0,0,212,224,3,
  	52,26,0,213,214,5,19,0,0,214,219,3,18,9,0,215,216,5,57,0,0,216,218,3,
  	18,9,0,217,215,1,0,0,0,218,221,1,0,0,0,219,217,1,0,0,0,219,220,1,0,0,
  	0,220,222,1,0,0,0,221,219,1,0,0,0,222,223,5,21,0,0,223,225,1,0,0,0,224,
  	213,1,0,0,0,224,225,1,0,0,0,225,21,1,0,0,0,226,230,5,52,0,0,227,229,3,
  	2,1,0,228,227,1,0,0,0,229,232,1,0,0,0,230,228,1,0,0,0,230,231,1,0,0,0,
  	231,233,1,0,0,0,232,230,1,0,0,0,233,234,5,53,0,0,234,23,1,0,0,0,235,236,
  	5,33,0,0,236,237,3,12,6,0,237,238,3,22,11,0,238,25,1,0,0,0,239,240,5,
  	34,0,0,240,241,5,50,0,0,241,242,3,28,14,0,242,243,5,51,0,0,243,244,3,
  	22,11,0,244,250,1,0,0,0,245,246,5,34,0,0,246,247,3,28,14,0,247,248,3,
  	22,11,0,248,250,1,0,0,0,249,239,1,0,0,0,249,245,1,0,0,0,250,27,1,0,0,
  	0,251,253,3,4,2,0,252,251,1,0,0,0,252,253,1,0,0,0,253,254,1,0,0,0,254,
  	256,5,61,0,0,255,257,3,12,6,0,256,255,1,0,0,0,256,257,1,0,0,0,257,258,
  	1,0,0,0,258,260,5,61,0,0,259,261,3,4,2,0,260,259,1,0,0,0,260,261,1,0,
  	0,0,261,29,1,0,0,0,262,263,5,35,0,0,263,264,3,22,11,0,264,265,5,33,0,
  	0,265,266,3,12,6,0,266,267,5,61,0,0,267,31,1,0,0,0,268,269,5,36,0,0,269,
  	270,3,12,6,0,270,272,3,22,11,0,271,273,3,34,17,0,272,271,1,0,0,0,272,
  	273,1,0,0,0,273,33,1,0,0,0,274,277,5,37,0,0,275,278,3,22,11,0,276,278,
  	3,32,16,0,277,275,1,0,0,0,277,276,1,0,0,0,278,35,1,0,0,0,279,282,5,62,
  	0,0,280,283,3,42,21,0,281,283,3,46,23,0,282,280,1,0,0,0,282,281,1,0,0,
  	0,283,284,1,0,0,0,284,285,5,61,0,0,285,37,1,0,0,0,286,287,5,63,0,0,287,
  	299,3,52,26,0,288,289,5,19,0,0,289,294,3,52,26,0,290,291,5,57,0,0,291,
  	293,3,52,26,0,292,290,1,0,0,0,293,296,1,0,0,0,294,292,1,0,0,0,294,295,
  	1,0,0,0,295,297,1,0,0,0,296,294,1,0,0,0,297,298,5,21,0,0,298,300,1,0,
  	0,0,299,288,1,0,0,0,299,300,1,0,0,0,300,302,1,0,0,0,301,303,3,40,20,0,
  	302,301,1,0,0,0,302,303,1,0,0,0,303,304,1,0,0,0,304,314,5,52,0,0,305,
  	309,3,42,21,0,306,309,3,46,23,0,307,309,3,48,24,0,308,305,1,0,0,0,308,
  	306,1,0,0,0,308,307,1,0,0,0,309,310,1,0,0,0,310,311,5,61,0,0,311,313,
  	1,0,0,0,312,308,1,0,0,0,313,316,1,0,0,0,314,312,1,0,0,0,314,315,1,0,0,
  	0,315,317,1,0,0,0,316,314,1,0,0,0,317,318,5,53,0,0,318,39,1,0,0,0,319,
  	320,5,58,0,0,320,325,3,20,10,0,321,322,5,57,0,0,322,324,3,20,10,0,323,
  	321,1,0,0,0,324,327,1,0,0,0,325,323,1,0,0,0,325,326,1,0,0,0,326,41,1,
  	0,0,0,327,325,1,0,0,0,328,331,5,66,0,0,329,331,3,18,9,0,330,328,1,0,0,
  	0,330,329,1,0,0,0,331,332,1,0,0,0,332,333,3,52,26,0,333,335,5,50,0,0,
  	334,336,3,44,22,0,335,334,1,0,0,0,335,336,1,0,0,0,336,337,1,0,0,0,337,
  	338,5,51,0,0,338,43,1,0,0,0,339,356,5,67,0,0,340,341,3,18,9,0,341,348,
  	3,52,26,0,342,343,5,57,0,0,343,344,3,18,9,0,344,345,3,52,26,0,345,347,
  	1,0,0,0,346,342,1,0,0,0,347,350,1,0,0,0,348,346,1,0,0,0,348,349,1,0,0,
  	0,349,353,1,0,0,0,350,348,1,0,0,0,351,352,5,57,0,0,352,354,5,67,0,0,353,
  	351,1,0,0,0,353,354,1,0,0,0,354,356,1,0,0,0,355,339,1,0,0,0,355,340,1,
  	0,0,0,356,45,1,0,0,0,357,359,5,65,0,0,358,357,1,0,0,0,358,359,1,0,0,0,
  	359,360,1,0,0,0,360,361,3,18,9,0,361,362,3,52,26,0,362,47,1,0,0,0,363,
  	365,5,65,0,0,364,363,1,0,0,0,364,365,1,0,0,0,365,366,1,0,0,0,366,367,
  	3,18,9,0,367,368,5,54,0,0,368,369,3,18,9,0,369,370,5,55,0,0,370,49,1,
  	0,0,0,371,372,5,64,0,0,372,373,5,73,0,0,373,51,1,0,0,0,374,375,7,8,0,
  	0,375,53,1,0,0,0,376,377,5,21,0,0,377,378,5,21,0,0,378,55,1,0,0,0,38,
  	59,76,85,94,125,139,160,162,173,176,179,189,191,198,210,219,224,230,249,
  	252,256,260,272,277,282,294,299,302,308,314,325,330,335,348,353,355,358,
  	364
  };
  staticData->serializedATN = antlr4::atn::SerializedATNView(serializedATNSegment, sizeof(serializedATNSegment) / sizeof(serializedATNSegment[0]));

  antlr4::atn::ATNDeserializer deserializer;
  staticData->atn = deserializer.deserialize(staticData->serializedATN);

  const size_t count = staticData->atn->getNumberOfDecisions();
  staticData->decisionToDFA.reserve(count);
  for (size_t i = 0; i < count; i++) { 
    staticData->decisionToDFA.emplace_back(staticData->atn->getDecisionState(i), i);
  }
  maplparserParserStaticData = std::move(staticData);
}

}

MaPLParser::MaPLParser(TokenStream *input) : MaPLParser(input, antlr4::atn::ParserATNSimulatorOptions()) {}

MaPLParser::MaPLParser(TokenStream *input, const antlr4::atn::ParserATNSimulatorOptions &options) : Parser(input) {
  MaPLParser::initialize();
  _interpreter = new atn::ParserATNSimulator(this, *maplparserParserStaticData->atn, maplparserParserStaticData->decisionToDFA, maplparserParserStaticData->sharedContextCache, options);
}

MaPLParser::~MaPLParser() {
  delete _interpreter;
}

const atn::ATN& MaPLParser::getATN() const {
  return *maplparserParserStaticData->atn;
}

std::string MaPLParser::getGrammarFileName() const {
  return "MaPLParser.g4";
}

const std::vector<std::string>& MaPLParser::getRuleNames() const {
  return maplparserParserStaticData->ruleNames;
}

const dfa::Vocabulary& MaPLParser::getVocabulary() const {
  return maplparserParserStaticData->vocabulary;
}

antlr4::atn::SerializedATNView MaPLParser::getSerializedATN() const {
  return maplparserParserStaticData->serializedATN;
}


//----------------- ProgramContext ------------------------------------------------------------------

MaPLParser::ProgramContext::ProgramContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* MaPLParser::ProgramContext::EOF() {
  return getToken(MaPLParser::EOF, 0);
}

std::vector<MaPLParser::StatementContext *> MaPLParser::ProgramContext::statement() {
  return getRuleContexts<MaPLParser::StatementContext>();
}

MaPLParser::StatementContext* MaPLParser::ProgramContext::statement(size_t i) {
  return getRuleContext<MaPLParser::StatementContext>(i);
}


size_t MaPLParser::ProgramContext::getRuleIndex() const {
  return MaPLParser::RuleProgram;
}


MaPLParser::ProgramContext* MaPLParser::program() {
  ProgramContext *_localctx = _tracker.createInstance<ProgramContext>(_ctx, getState());
  enterRule(_localctx, 0, MaPLParser::RuleProgram);
  size_t _la = 0;

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(59);
    _errHandler->sync(this);
    _la = _input->LA(1);
    while (((((_la - 33) & ~ 0x3fULL) == 0) &&
      ((1ULL << (_la - 33)) & 6854231588863) != 0)) {
      setState(56);
      statement();
      setState(61);
      _errHandler->sync(this);
      _la = _input->LA(1);
    }
    setState(62);
    match(MaPLParser::EOF);
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- StatementContext ------------------------------------------------------------------

MaPLParser::StatementContext::StatementContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

MaPLParser::ImperativeStatementContext* MaPLParser::StatementContext::imperativeStatement() {
  return getRuleContext<MaPLParser::ImperativeStatementContext>(0);
}

tree::TerminalNode* MaPLParser::StatementContext::STATEMENT_DELIMITER() {
  return getToken(MaPLParser::STATEMENT_DELIMITER, 0);
}

MaPLParser::WhileLoopContext* MaPLParser::StatementContext::whileLoop() {
  return getRuleContext<MaPLParser::WhileLoopContext>(0);
}

MaPLParser::ForLoopContext* MaPLParser::StatementContext::forLoop() {
  return getRuleContext<MaPLParser::ForLoopContext>(0);
}

MaPLParser::DoWhileLoopContext* MaPLParser::StatementContext::doWhileLoop() {
  return getRuleContext<MaPLParser::DoWhileLoopContext>(0);
}

MaPLParser::ConditionalContext* MaPLParser::StatementContext::conditional() {
  return getRuleContext<MaPLParser::ConditionalContext>(0);
}

MaPLParser::ApiGlobalContext* MaPLParser::StatementContext::apiGlobal() {
  return getRuleContext<MaPLParser::ApiGlobalContext>(0);
}

MaPLParser::ApiTypeContext* MaPLParser::StatementContext::apiType() {
  return getRuleContext<MaPLParser::ApiTypeContext>(0);
}

MaPLParser::ApiImportContext* MaPLParser::StatementContext::apiImport() {
  return getRuleContext<MaPLParser::ApiImportContext>(0);
}

MaPLParser::MetadataStatementContext* MaPLParser::StatementContext::metadataStatement() {
  return getRuleContext<MaPLParser::MetadataStatementContext>(0);
}

MaPLParser::ScopeContext* MaPLParser::StatementContext::scope() {
  return getRuleContext<MaPLParser::ScopeContext>(0);
}


size_t MaPLParser::StatementContext::getRuleIndex() const {
  return MaPLParser::RuleStatement;
}


MaPLParser::StatementContext* MaPLParser::statement() {
  StatementContext *_localctx = _tracker.createInstance<StatementContext>(_ctx, getState());
  enterRule(_localctx, 2, MaPLParser::RuleStatement);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    setState(76);
    _errHandler->sync(this);
    switch (getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 1, _ctx)) {
    case 1: {
      enterOuterAlt(_localctx, 1);
      setState(64);
      imperativeStatement();
      setState(65);
      match(MaPLParser::STATEMENT_DELIMITER);
      break;
    }

    case 2: {
      enterOuterAlt(_localctx, 2);
      setState(67);
      whileLoop();
      break;
    }

    case 3: {
      enterOuterAlt(_localctx, 3);
      setState(68);
      forLoop();
      break;
    }

    case 4: {
      enterOuterAlt(_localctx, 4);
      setState(69);
      doWhileLoop();
      break;
    }

    case 5: {
      enterOuterAlt(_localctx, 5);
      setState(70);
      conditional();
      break;
    }

    case 6: {
      enterOuterAlt(_localctx, 6);
      setState(71);
      apiGlobal();
      break;
    }

    case 7: {
      enterOuterAlt(_localctx, 7);
      setState(72);
      apiType();
      break;
    }

    case 8: {
      enterOuterAlt(_localctx, 8);
      setState(73);
      apiImport();
      break;
    }

    case 9: {
      enterOuterAlt(_localctx, 9);
      setState(74);
      metadataStatement();
      break;
    }

    case 10: {
      enterOuterAlt(_localctx, 10);
      setState(75);
      scope();
      break;
    }

    default:
      break;
    }
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- ImperativeStatementContext ------------------------------------------------------------------

MaPLParser::ImperativeStatementContext::ImperativeStatementContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* MaPLParser::ImperativeStatementContext::BREAK() {
  return getToken(MaPLParser::BREAK, 0);
}

tree::TerminalNode* MaPLParser::ImperativeStatementContext::CONTINUE() {
  return getToken(MaPLParser::CONTINUE, 0);
}

tree::TerminalNode* MaPLParser::ImperativeStatementContext::EXIT() {
  return getToken(MaPLParser::EXIT, 0);
}

MaPLParser::VariableDeclarationContext* MaPLParser::ImperativeStatementContext::variableDeclaration() {
  return getRuleContext<MaPLParser::VariableDeclarationContext>(0);
}

MaPLParser::AssignStatementContext* MaPLParser::ImperativeStatementContext::assignStatement() {
  return getRuleContext<MaPLParser::AssignStatementContext>(0);
}

MaPLParser::UnaryStatementContext* MaPLParser::ImperativeStatementContext::unaryStatement() {
  return getRuleContext<MaPLParser::UnaryStatementContext>(0);
}

MaPLParser::ObjectExpressionContext* MaPLParser::ImperativeStatementContext::objectExpression() {
  return getRuleContext<MaPLParser::ObjectExpressionContext>(0);
}


size_t MaPLParser::ImperativeStatementContext::getRuleIndex() const {
  return MaPLParser::RuleImperativeStatement;
}


MaPLParser::ImperativeStatementContext* MaPLParser::imperativeStatement() {
  ImperativeStatementContext *_localctx = _tracker.createInstance<ImperativeStatementContext>(_ctx, getState());
  enterRule(_localctx, 4, MaPLParser::RuleImperativeStatement);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    setState(85);
    _errHandler->sync(this);
    switch (getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 2, _ctx)) {
    case 1: {
      enterOuterAlt(_localctx, 1);
      setState(78);
      antlrcpp::downCast<ImperativeStatementContext *>(_localctx)->keyToken = match(MaPLParser::BREAK);
      break;
    }

    case 2: {
      enterOuterAlt(_localctx, 2);
      setState(79);
      antlrcpp::downCast<ImperativeStatementContext *>(_localctx)->keyToken = match(MaPLParser::CONTINUE);
      break;
    }

    case 3: {
      enterOuterAlt(_localctx, 3);
      setState(80);
      antlrcpp::downCast<ImperativeStatementContext *>(_localctx)->keyToken = match(MaPLParser::EXIT);
      break;
    }

    case 4: {
      enterOuterAlt(_localctx, 4);
      setState(81);
      variableDeclaration();
      break;
    }

    case 5: {
      enterOuterAlt(_localctx, 5);
      setState(82);
      assignStatement();
      break;
    }

    case 6: {
      enterOuterAlt(_localctx, 6);
      setState(83);
      unaryStatement();
      break;
    }

    case 7: {
      enterOuterAlt(_localctx, 7);
      setState(84);
      objectExpression(0);
      break;
    }

    default:
      break;
    }
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- MetadataStatementContext ------------------------------------------------------------------

MaPLParser::MetadataStatementContext::MetadataStatementContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* MaPLParser::MetadataStatementContext::METADATA_OPEN() {
  return getToken(MaPLParser::METADATA_OPEN, 0);
}

tree::TerminalNode* MaPLParser::MetadataStatementContext::METADATA_CLOSE() {
  return getToken(MaPLParser::METADATA_CLOSE, 0);
}

std::vector<MaPLParser::ExpressionContext *> MaPLParser::MetadataStatementContext::expression() {
  return getRuleContexts<MaPLParser::ExpressionContext>();
}

MaPLParser::ExpressionContext* MaPLParser::MetadataStatementContext::expression(size_t i) {
  return getRuleContext<MaPLParser::ExpressionContext>(i);
}

std::vector<tree::TerminalNode *> MaPLParser::MetadataStatementContext::METADATA_INTERPOLATION() {
  return getTokens(MaPLParser::METADATA_INTERPOLATION);
}

tree::TerminalNode* MaPLParser::MetadataStatementContext::METADATA_INTERPOLATION(size_t i) {
  return getToken(MaPLParser::METADATA_INTERPOLATION, i);
}

std::vector<tree::TerminalNode *> MaPLParser::MetadataStatementContext::SCOPE_CLOSE() {
  return getTokens(MaPLParser::SCOPE_CLOSE);
}

tree::TerminalNode* MaPLParser::MetadataStatementContext::SCOPE_CLOSE(size_t i) {
  return getToken(MaPLParser::SCOPE_CLOSE, i);
}


size_t MaPLParser::MetadataStatementContext::getRuleIndex() const {
  return MaPLParser::RuleMetadataStatement;
}


MaPLParser::MetadataStatementContext* MaPLParser::metadataStatement() {
  MetadataStatementContext *_localctx = _tracker.createInstance<MetadataStatementContext>(_ctx, getState());
  enterRule(_localctx, 6, MaPLParser::RuleMetadataStatement);
  size_t _la = 0;

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(87);
    antlrcpp::downCast<MetadataStatementContext *>(_localctx)->metadata_openToken = match(MaPLParser::METADATA_OPEN);
    antlrcpp::downCast<MetadataStatementContext *>(_localctx)->metadataTokens.push_back(antlrcpp::downCast<MetadataStatementContext *>(_localctx)->metadata_openToken);
    setState(94);
    _errHandler->sync(this);
    _la = _input->LA(1);
    while (_la == MaPLParser::METADATA_INTERPOLATION) {
      setState(88);
      antlrcpp::downCast<MetadataStatementContext *>(_localctx)->metadata_interpolationToken = match(MaPLParser::METADATA_INTERPOLATION);
      antlrcpp::downCast<MetadataStatementContext *>(_localctx)->metadataTokens.push_back(antlrcpp::downCast<MetadataStatementContext *>(_localctx)->metadata_interpolationToken);
      setState(89);
      expression(0);
      setState(90);
      antlrcpp::downCast<MetadataStatementContext *>(_localctx)->scope_closeToken = match(MaPLParser::SCOPE_CLOSE);
      antlrcpp::downCast<MetadataStatementContext *>(_localctx)->metadataTokens.push_back(antlrcpp::downCast<MetadataStatementContext *>(_localctx)->scope_closeToken);
      setState(96);
      _errHandler->sync(this);
      _la = _input->LA(1);
    }
    setState(97);
    antlrcpp::downCast<MetadataStatementContext *>(_localctx)->metadata_closeToken = match(MaPLParser::METADATA_CLOSE);
    antlrcpp::downCast<MetadataStatementContext *>(_localctx)->metadataTokens.push_back(antlrcpp::downCast<MetadataStatementContext *>(_localctx)->metadata_closeToken);
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- AssignStatementContext ------------------------------------------------------------------

MaPLParser::AssignStatementContext::AssignStatementContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

MaPLParser::ObjectExpressionContext* MaPLParser::AssignStatementContext::objectExpression() {
  return getRuleContext<MaPLParser::ObjectExpressionContext>(0);
}

MaPLParser::ExpressionContext* MaPLParser::AssignStatementContext::expression() {
  return getRuleContext<MaPLParser::ExpressionContext>(0);
}

tree::TerminalNode* MaPLParser::AssignStatementContext::ASSIGN() {
  return getToken(MaPLParser::ASSIGN, 0);
}

tree::TerminalNode* MaPLParser::AssignStatementContext::ADD_ASSIGN() {
  return getToken(MaPLParser::ADD_ASSIGN, 0);
}

tree::TerminalNode* MaPLParser::AssignStatementContext::SUBTRACT_ASSIGN() {
  return getToken(MaPLParser::SUBTRACT_ASSIGN, 0);
}

tree::TerminalNode* MaPLParser::AssignStatementContext::MULTIPLY_ASSIGN() {
  return getToken(MaPLParser::MULTIPLY_ASSIGN, 0);
}

tree::TerminalNode* MaPLParser::AssignStatementContext::DIVIDE_ASSIGN() {
  return getToken(MaPLParser::DIVIDE_ASSIGN, 0);
}

tree::TerminalNode* MaPLParser::AssignStatementContext::MOD_ASSIGN() {
  return getToken(MaPLParser::MOD_ASSIGN, 0);
}

tree::TerminalNode* MaPLParser::AssignStatementContext::BITWISE_AND_ASSIGN() {
  return getToken(MaPLParser::BITWISE_AND_ASSIGN, 0);
}

tree::TerminalNode* MaPLParser::AssignStatementContext::BITWISE_OR_ASSIGN() {
  return getToken(MaPLParser::BITWISE_OR_ASSIGN, 0);
}

tree::TerminalNode* MaPLParser::AssignStatementContext::BITWISE_XOR_ASSIGN() {
  return getToken(MaPLParser::BITWISE_XOR_ASSIGN, 0);
}

tree::TerminalNode* MaPLParser::AssignStatementContext::BITWISE_SHIFT_LEFT_ASSIGN() {
  return getToken(MaPLParser::BITWISE_SHIFT_LEFT_ASSIGN, 0);
}

tree::TerminalNode* MaPLParser::AssignStatementContext::BITWISE_SHIFT_RIGHT_ASSIGN() {
  return getToken(MaPLParser::BITWISE_SHIFT_RIGHT_ASSIGN, 0);
}


size_t MaPLParser::AssignStatementContext::getRuleIndex() const {
  return MaPLParser::RuleAssignStatement;
}


MaPLParser::AssignStatementContext* MaPLParser::assignStatement() {
  AssignStatementContext *_localctx = _tracker.createInstance<AssignStatementContext>(_ctx, getState());
  enterRule(_localctx, 8, MaPLParser::RuleAssignStatement);
  size_t _la = 0;

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(99);
    objectExpression(0);
    setState(100);
    antlrcpp::downCast<AssignStatementContext *>(_localctx)->keyToken = _input->LT(1);
    _la = _input->LA(1);
    if (!((((_la & ~ 0x3fULL) == 0) &&
      ((1ULL << _la) & 7147096746) != 0))) {
      antlrcpp::downCast<AssignStatementContext *>(_localctx)->keyToken = _errHandler->recoverInline(this);
    }
    else {
      _errHandler->reportMatch(this);
      consume();
    }
    setState(101);
    expression(0);
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- UnaryStatementContext ------------------------------------------------------------------

MaPLParser::UnaryStatementContext::UnaryStatementContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

MaPLParser::ObjectExpressionContext* MaPLParser::UnaryStatementContext::objectExpression() {
  return getRuleContext<MaPLParser::ObjectExpressionContext>(0);
}

tree::TerminalNode* MaPLParser::UnaryStatementContext::INCREMENT() {
  return getToken(MaPLParser::INCREMENT, 0);
}

tree::TerminalNode* MaPLParser::UnaryStatementContext::DECREMENT() {
  return getToken(MaPLParser::DECREMENT, 0);
}


size_t MaPLParser::UnaryStatementContext::getRuleIndex() const {
  return MaPLParser::RuleUnaryStatement;
}


MaPLParser::UnaryStatementContext* MaPLParser::unaryStatement() {
  UnaryStatementContext *_localctx = _tracker.createInstance<UnaryStatementContext>(_ctx, getState());
  enterRule(_localctx, 10, MaPLParser::RuleUnaryStatement);
  size_t _la = 0;

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(103);
    objectExpression(0);
    setState(104);
    antlrcpp::downCast<UnaryStatementContext *>(_localctx)->keyToken = _input->LT(1);
    _la = _input->LA(1);
    if (!(_la == MaPLParser::INCREMENT

    || _la == MaPLParser::DECREMENT)) {
      antlrcpp::downCast<UnaryStatementContext *>(_localctx)->keyToken = _errHandler->recoverInline(this);
    }
    else {
      _errHandler->reportMatch(this);
      consume();
    }
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- ExpressionContext ------------------------------------------------------------------

MaPLParser::ExpressionContext::ExpressionContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

MaPLParser::TypeContext* MaPLParser::ExpressionContext::type() {
  return getRuleContext<MaPLParser::TypeContext>(0);
}

tree::TerminalNode* MaPLParser::ExpressionContext::PAREN_CLOSE() {
  return getToken(MaPLParser::PAREN_CLOSE, 0);
}

std::vector<MaPLParser::ExpressionContext *> MaPLParser::ExpressionContext::expression() {
  return getRuleContexts<MaPLParser::ExpressionContext>();
}

MaPLParser::ExpressionContext* MaPLParser::ExpressionContext::expression(size_t i) {
  return getRuleContext<MaPLParser::ExpressionContext>(i);
}

tree::TerminalNode* MaPLParser::ExpressionContext::PAREN_OPEN() {
  return getToken(MaPLParser::PAREN_OPEN, 0);
}

tree::TerminalNode* MaPLParser::ExpressionContext::LOGICAL_NEGATION() {
  return getToken(MaPLParser::LOGICAL_NEGATION, 0);
}

tree::TerminalNode* MaPLParser::ExpressionContext::SUBTRACT() {
  return getToken(MaPLParser::SUBTRACT, 0);
}

tree::TerminalNode* MaPLParser::ExpressionContext::BITWISE_NEGATION() {
  return getToken(MaPLParser::BITWISE_NEGATION, 0);
}

tree::TerminalNode* MaPLParser::ExpressionContext::LITERAL_TRUE() {
  return getToken(MaPLParser::LITERAL_TRUE, 0);
}

tree::TerminalNode* MaPLParser::ExpressionContext::LITERAL_FALSE() {
  return getToken(MaPLParser::LITERAL_FALSE, 0);
}

tree::TerminalNode* MaPLParser::ExpressionContext::LITERAL_NULL() {
  return getToken(MaPLParser::LITERAL_NULL, 0);
}

tree::TerminalNode* MaPLParser::ExpressionContext::LITERAL_INT() {
  return getToken(MaPLParser::LITERAL_INT, 0);
}

tree::TerminalNode* MaPLParser::ExpressionContext::LITERAL_FLOAT() {
  return getToken(MaPLParser::LITERAL_FLOAT, 0);
}

tree::TerminalNode* MaPLParser::ExpressionContext::LITERAL_STRING() {
  return getToken(MaPLParser::LITERAL_STRING, 0);
}

MaPLParser::ObjectExpressionContext* MaPLParser::ExpressionContext::objectExpression() {
  return getRuleContext<MaPLParser::ObjectExpressionContext>(0);
}

tree::TerminalNode* MaPLParser::ExpressionContext::MOD() {
  return getToken(MaPLParser::MOD, 0);
}

tree::TerminalNode* MaPLParser::ExpressionContext::MULTIPLY() {
  return getToken(MaPLParser::MULTIPLY, 0);
}

tree::TerminalNode* MaPLParser::ExpressionContext::DIVIDE() {
  return getToken(MaPLParser::DIVIDE, 0);
}

tree::TerminalNode* MaPLParser::ExpressionContext::ADD() {
  return getToken(MaPLParser::ADD, 0);
}

MaPLParser::BitwiseShiftRightContext* MaPLParser::ExpressionContext::bitwiseShiftRight() {
  return getRuleContext<MaPLParser::BitwiseShiftRightContext>(0);
}

tree::TerminalNode* MaPLParser::ExpressionContext::BITWISE_SHIFT_LEFT() {
  return getToken(MaPLParser::BITWISE_SHIFT_LEFT, 0);
}

tree::TerminalNode* MaPLParser::ExpressionContext::BITWISE_AND() {
  return getToken(MaPLParser::BITWISE_AND, 0);
}

tree::TerminalNode* MaPLParser::ExpressionContext::BITWISE_XOR() {
  return getToken(MaPLParser::BITWISE_XOR, 0);
}

tree::TerminalNode* MaPLParser::ExpressionContext::BITWISE_OR() {
  return getToken(MaPLParser::BITWISE_OR, 0);
}

tree::TerminalNode* MaPLParser::ExpressionContext::NULL_COALESCING() {
  return getToken(MaPLParser::NULL_COALESCING, 0);
}

tree::TerminalNode* MaPLParser::ExpressionContext::LOGICAL_EQUALITY() {
  return getToken(MaPLParser::LOGICAL_EQUALITY, 0);
}

tree::TerminalNode* MaPLParser::ExpressionContext::LOGICAL_INEQUALITY() {
  return getToken(MaPLParser::LOGICAL_INEQUALITY, 0);
}

tree::TerminalNode* MaPLParser::ExpressionContext::LESS_THAN() {
  return getToken(MaPLParser::LESS_THAN, 0);
}

tree::TerminalNode* MaPLParser::ExpressionContext::LESS_THAN_EQUAL() {
  return getToken(MaPLParser::LESS_THAN_EQUAL, 0);
}

tree::TerminalNode* MaPLParser::ExpressionContext::GREATER_THAN() {
  return getToken(MaPLParser::GREATER_THAN, 0);
}

tree::TerminalNode* MaPLParser::ExpressionContext::GREATER_THAN_EQUAL() {
  return getToken(MaPLParser::GREATER_THAN_EQUAL, 0);
}

tree::TerminalNode* MaPLParser::ExpressionContext::LOGICAL_AND() {
  return getToken(MaPLParser::LOGICAL_AND, 0);
}

tree::TerminalNode* MaPLParser::ExpressionContext::LOGICAL_OR() {
  return getToken(MaPLParser::LOGICAL_OR, 0);
}

tree::TerminalNode* MaPLParser::ExpressionContext::COLON() {
  return getToken(MaPLParser::COLON, 0);
}

tree::TerminalNode* MaPLParser::ExpressionContext::TERNARY_CONDITIONAL() {
  return getToken(MaPLParser::TERNARY_CONDITIONAL, 0);
}


size_t MaPLParser::ExpressionContext::getRuleIndex() const {
  return MaPLParser::RuleExpression;
}



MaPLParser::ExpressionContext* MaPLParser::expression() {
   return expression(0);
}

MaPLParser::ExpressionContext* MaPLParser::expression(int precedence) {
  ParserRuleContext *parentContext = _ctx;
  size_t parentState = getState();
  MaPLParser::ExpressionContext *_localctx = _tracker.createInstance<ExpressionContext>(_ctx, parentState);
  MaPLParser::ExpressionContext *previousContext = _localctx;
  (void)previousContext; // Silence compiler, in case the context is not used by generated code.
  size_t startState = 12;
  enterRecursionRule(_localctx, 12, MaPLParser::RuleExpression, precedence);

    size_t _la = 0;

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    unrollRecursionContexts(parentContext);
  });
  try {
    size_t alt;
    enterOuterAlt(_localctx, 1);
    setState(125);
    _errHandler->sync(this);
    switch (getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 4, _ctx)) {
    case 1: {
      setState(107);
      antlrcpp::downCast<ExpressionContext *>(_localctx)->keyToken = match(MaPLParser::PAREN_OPEN);
      setState(108);
      type();
      setState(109);
      match(MaPLParser::PAREN_CLOSE);
      setState(110);
      expression(19);
      break;
    }

    case 2: {
      setState(112);
      antlrcpp::downCast<ExpressionContext *>(_localctx)->keyToken = _input->LT(1);
      _la = _input->LA(1);
      if (!((((_la & ~ 0x3fULL) == 0) &&
        ((1ULL << _la) & 8650768) != 0))) {
        antlrcpp::downCast<ExpressionContext *>(_localctx)->keyToken = _errHandler->recoverInline(this);
      }
      else {
        _errHandler->reportMatch(this);
        consume();
      }
      setState(113);
      expression(18);
      break;
    }

    case 3: {
      setState(114);
      match(MaPLParser::PAREN_OPEN);
      setState(115);
      expression(0);
      setState(116);
      antlrcpp::downCast<ExpressionContext *>(_localctx)->keyToken = match(MaPLParser::PAREN_CLOSE);
      break;
    }

    case 4: {
      setState(118);
      antlrcpp::downCast<ExpressionContext *>(_localctx)->keyToken = match(MaPLParser::LITERAL_TRUE);
      break;
    }

    case 5: {
      setState(119);
      antlrcpp::downCast<ExpressionContext *>(_localctx)->keyToken = match(MaPLParser::LITERAL_FALSE);
      break;
    }

    case 6: {
      setState(120);
      antlrcpp::downCast<ExpressionContext *>(_localctx)->keyToken = match(MaPLParser::LITERAL_NULL);
      break;
    }

    case 7: {
      setState(121);
      antlrcpp::downCast<ExpressionContext *>(_localctx)->keyToken = match(MaPLParser::LITERAL_INT);
      break;
    }

    case 8: {
      setState(122);
      antlrcpp::downCast<ExpressionContext *>(_localctx)->keyToken = match(MaPLParser::LITERAL_FLOAT);
      break;
    }

    case 9: {
      setState(123);
      antlrcpp::downCast<ExpressionContext *>(_localctx)->keyToken = match(MaPLParser::LITERAL_STRING);
      break;
    }

    case 10: {
      setState(124);
      objectExpression(0);
      break;
    }

    default:
      break;
    }
    _ctx->stop = _input->LT(-1);
    setState(162);
    _errHandler->sync(this);
    alt = getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 7, _ctx);
    while (alt != 2 && alt != atn::ATN::INVALID_ALT_NUMBER) {
      if (alt == 1) {
        if (!_parseListeners.empty())
          triggerExitRuleEvent();
        previousContext = _localctx;
        setState(160);
        _errHandler->sync(this);
        switch (getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 6, _ctx)) {
        case 1: {
          _localctx = _tracker.createInstance<ExpressionContext>(parentContext, parentState);
          pushNewRecursionContext(_localctx, startState, RuleExpression);
          setState(127);

          if (!(precpred(_ctx, 17))) throw FailedPredicateException(this, "precpred(_ctx, 17)");
          setState(128);
          antlrcpp::downCast<ExpressionContext *>(_localctx)->keyToken = match(MaPLParser::MOD);
          setState(129);
          expression(18);
          break;
        }

        case 2: {
          _localctx = _tracker.createInstance<ExpressionContext>(parentContext, parentState);
          pushNewRecursionContext(_localctx, startState, RuleExpression);
          setState(130);

          if (!(precpred(_ctx, 16))) throw FailedPredicateException(this, "precpred(_ctx, 16)");
          setState(131);
          antlrcpp::downCast<ExpressionContext *>(_localctx)->keyToken = _input->LT(1);
          _la = _input->LA(1);
          if (!(_la == MaPLParser::DIVIDE

          || _la == MaPLParser::MULTIPLY)) {
            antlrcpp::downCast<ExpressionContext *>(_localctx)->keyToken = _errHandler->recoverInline(this);
          }
          else {
            _errHandler->reportMatch(this);
            consume();
          }
          setState(132);
          expression(17);
          break;
        }

        case 3: {
          _localctx = _tracker.createInstance<ExpressionContext>(parentContext, parentState);
          pushNewRecursionContext(_localctx, startState, RuleExpression);
          setState(133);

          if (!(precpred(_ctx, 15))) throw FailedPredicateException(this, "precpred(_ctx, 15)");
          setState(134);
          antlrcpp::downCast<ExpressionContext *>(_localctx)->keyToken = _input->LT(1);
          _la = _input->LA(1);
          if (!(_la == MaPLParser::ADD

          || _la == MaPLParser::SUBTRACT)) {
            antlrcpp::downCast<ExpressionContext *>(_localctx)->keyToken = _errHandler->recoverInline(this);
          }
          else {
            _errHandler->reportMatch(this);
            consume();
          }
          setState(135);
          expression(16);
          break;
        }

        case 4: {
          _localctx = _tracker.createInstance<ExpressionContext>(parentContext, parentState);
          pushNewRecursionContext(_localctx, startState, RuleExpression);
          setState(136);

          if (!(precpred(_ctx, 14))) throw FailedPredicateException(this, "precpred(_ctx, 14)");
          setState(139);
          _errHandler->sync(this);
          switch (_input->LA(1)) {
            case MaPLParser::BITWISE_SHIFT_LEFT: {
              setState(137);
              antlrcpp::downCast<ExpressionContext *>(_localctx)->keyToken = match(MaPLParser::BITWISE_SHIFT_LEFT);
              break;
            }

            case MaPLParser::GREATER_THAN: {
              setState(138);
              bitwiseShiftRight();
              break;
            }

          default:
            throw NoViableAltException(this);
          }
          setState(141);
          expression(15);
          break;
        }

        case 5: {
          _localctx = _tracker.createInstance<ExpressionContext>(parentContext, parentState);
          pushNewRecursionContext(_localctx, startState, RuleExpression);
          setState(142);

          if (!(precpred(_ctx, 13))) throw FailedPredicateException(this, "precpred(_ctx, 13)");
          setState(143);
          antlrcpp::downCast<ExpressionContext *>(_localctx)->keyToken = _input->LT(1);
          _la = _input->LA(1);
          if (!((((_la & ~ 0x3fULL) == 0) &&
            ((1ULL << _la) & 352321536) != 0))) {
            antlrcpp::downCast<ExpressionContext *>(_localctx)->keyToken = _errHandler->recoverInline(this);
          }
          else {
            _errHandler->reportMatch(this);
            consume();
          }
          setState(144);
          expression(14);
          break;
        }

        case 6: {
          _localctx = _tracker.createInstance<ExpressionContext>(parentContext, parentState);
          pushNewRecursionContext(_localctx, startState, RuleExpression);
          setState(145);

          if (!(precpred(_ctx, 12))) throw FailedPredicateException(this, "precpred(_ctx, 12)");
          setState(146);
          antlrcpp::downCast<ExpressionContext *>(_localctx)->keyToken = match(MaPLParser::NULL_COALESCING);
          setState(147);
          expression(12);
          break;
        }

        case 7: {
          _localctx = _tracker.createInstance<ExpressionContext>(parentContext, parentState);
          pushNewRecursionContext(_localctx, startState, RuleExpression);
          setState(148);

          if (!(precpred(_ctx, 11))) throw FailedPredicateException(this, "precpred(_ctx, 11)");
          setState(149);
          antlrcpp::downCast<ExpressionContext *>(_localctx)->keyToken = _input->LT(1);
          _la = _input->LA(1);
          if (!((((_la & ~ 0x3fULL) == 0) &&
            ((1ULL << _la) & 7913472) != 0))) {
            antlrcpp::downCast<ExpressionContext *>(_localctx)->keyToken = _errHandler->recoverInline(this);
          }
          else {
            _errHandler->reportMatch(this);
            consume();
          }
          setState(150);
          expression(12);
          break;
        }

        case 8: {
          _localctx = _tracker.createInstance<ExpressionContext>(parentContext, parentState);
          pushNewRecursionContext(_localctx, startState, RuleExpression);
          setState(151);

          if (!(precpred(_ctx, 10))) throw FailedPredicateException(this, "precpred(_ctx, 10)");
          setState(152);
          antlrcpp::downCast<ExpressionContext *>(_localctx)->keyToken = _input->LT(1);
          _la = _input->LA(1);
          if (!(_la == MaPLParser::LOGICAL_AND

          || _la == MaPLParser::LOGICAL_OR)) {
            antlrcpp::downCast<ExpressionContext *>(_localctx)->keyToken = _errHandler->recoverInline(this);
          }
          else {
            _errHandler->reportMatch(this);
            consume();
          }
          setState(153);
          expression(11);
          break;
        }

        case 9: {
          _localctx = _tracker.createInstance<ExpressionContext>(parentContext, parentState);
          pushNewRecursionContext(_localctx, startState, RuleExpression);
          setState(154);

          if (!(precpred(_ctx, 9))) throw FailedPredicateException(this, "precpred(_ctx, 9)");
          setState(155);
          antlrcpp::downCast<ExpressionContext *>(_localctx)->keyToken = match(MaPLParser::TERNARY_CONDITIONAL);
          setState(156);
          expression(0);
          setState(157);
          match(MaPLParser::COLON);
          setState(158);
          expression(9);
          break;
        }

        default:
          break;
        } 
      }
      setState(164);
      _errHandler->sync(this);
      alt = getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 7, _ctx);
    }
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }
  return _localctx;
}

//----------------- ObjectExpressionContext ------------------------------------------------------------------

MaPLParser::ObjectExpressionContext::ObjectExpressionContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

MaPLParser::IdentifierContext* MaPLParser::ObjectExpressionContext::identifier() {
  return getRuleContext<MaPLParser::IdentifierContext>(0);
}

tree::TerminalNode* MaPLParser::ObjectExpressionContext::PAREN_CLOSE() {
  return getToken(MaPLParser::PAREN_CLOSE, 0);
}

tree::TerminalNode* MaPLParser::ObjectExpressionContext::PAREN_OPEN() {
  return getToken(MaPLParser::PAREN_OPEN, 0);
}

std::vector<MaPLParser::ExpressionContext *> MaPLParser::ObjectExpressionContext::expression() {
  return getRuleContexts<MaPLParser::ExpressionContext>();
}

MaPLParser::ExpressionContext* MaPLParser::ObjectExpressionContext::expression(size_t i) {
  return getRuleContext<MaPLParser::ExpressionContext>(i);
}

std::vector<tree::TerminalNode *> MaPLParser::ObjectExpressionContext::PARAM_DELIMITER() {
  return getTokens(MaPLParser::PARAM_DELIMITER);
}

tree::TerminalNode* MaPLParser::ObjectExpressionContext::PARAM_DELIMITER(size_t i) {
  return getToken(MaPLParser::PARAM_DELIMITER, i);
}

std::vector<MaPLParser::ObjectExpressionContext *> MaPLParser::ObjectExpressionContext::objectExpression() {
  return getRuleContexts<MaPLParser::ObjectExpressionContext>();
}

MaPLParser::ObjectExpressionContext* MaPLParser::ObjectExpressionContext::objectExpression(size_t i) {
  return getRuleContext<MaPLParser::ObjectExpressionContext>(i);
}

tree::TerminalNode* MaPLParser::ObjectExpressionContext::OBJECT_TO_MEMBER() {
  return getToken(MaPLParser::OBJECT_TO_MEMBER, 0);
}

tree::TerminalNode* MaPLParser::ObjectExpressionContext::SUBSCRIPT_CLOSE() {
  return getToken(MaPLParser::SUBSCRIPT_CLOSE, 0);
}

tree::TerminalNode* MaPLParser::ObjectExpressionContext::SUBSCRIPT_OPEN() {
  return getToken(MaPLParser::SUBSCRIPT_OPEN, 0);
}


size_t MaPLParser::ObjectExpressionContext::getRuleIndex() const {
  return MaPLParser::RuleObjectExpression;
}



MaPLParser::ObjectExpressionContext* MaPLParser::objectExpression() {
   return objectExpression(0);
}

MaPLParser::ObjectExpressionContext* MaPLParser::objectExpression(int precedence) {
  ParserRuleContext *parentContext = _ctx;
  size_t parentState = getState();
  MaPLParser::ObjectExpressionContext *_localctx = _tracker.createInstance<ObjectExpressionContext>(_ctx, parentState);
  MaPLParser::ObjectExpressionContext *previousContext = _localctx;
  (void)previousContext; // Silence compiler, in case the context is not used by generated code.
  size_t startState = 14;
  enterRecursionRule(_localctx, 14, MaPLParser::RuleObjectExpression, precedence);

    size_t _la = 0;

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    unrollRecursionContexts(parentContext);
  });
  try {
    size_t alt;
    enterOuterAlt(_localctx, 1);
    setState(166);
    identifier();
    setState(179);
    _errHandler->sync(this);

    switch (getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 10, _ctx)) {
    case 1: {
      setState(167);
      antlrcpp::downCast<ObjectExpressionContext *>(_localctx)->keyToken = match(MaPLParser::PAREN_OPEN);
      setState(176);
      _errHandler->sync(this);

      _la = _input->LA(1);
      if ((((_la & ~ 0x3fULL) == 0) &&
        ((1ULL << _la) & 2251791232401424) != 0) || ((((_la - 65) & ~ 0x3fULL) == 0) &&
        ((1ULL << (_la - 65)) & 1531) != 0)) {
        setState(168);
        expression(0);
        setState(173);
        _errHandler->sync(this);
        _la = _input->LA(1);
        while (_la == MaPLParser::PARAM_DELIMITER) {
          setState(169);
          match(MaPLParser::PARAM_DELIMITER);
          setState(170);
          expression(0);
          setState(175);
          _errHandler->sync(this);
          _la = _input->LA(1);
        }
      }
      setState(178);
      match(MaPLParser::PAREN_CLOSE);
      break;
    }

    default:
      break;
    }
    _ctx->stop = _input->LT(-1);
    setState(191);
    _errHandler->sync(this);
    alt = getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 12, _ctx);
    while (alt != 2 && alt != atn::ATN::INVALID_ALT_NUMBER) {
      if (alt == 1) {
        if (!_parseListeners.empty())
          triggerExitRuleEvent();
        previousContext = _localctx;
        setState(189);
        _errHandler->sync(this);
        switch (getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 11, _ctx)) {
        case 1: {
          _localctx = _tracker.createInstance<ObjectExpressionContext>(parentContext, parentState);
          pushNewRecursionContext(_localctx, startState, RuleObjectExpression);
          setState(181);

          if (!(precpred(_ctx, 3))) throw FailedPredicateException(this, "precpred(_ctx, 3)");
          setState(182);
          antlrcpp::downCast<ObjectExpressionContext *>(_localctx)->keyToken = match(MaPLParser::OBJECT_TO_MEMBER);
          setState(183);
          objectExpression(4);
          break;
        }

        case 2: {
          _localctx = _tracker.createInstance<ObjectExpressionContext>(parentContext, parentState);
          pushNewRecursionContext(_localctx, startState, RuleObjectExpression);
          setState(184);

          if (!(precpred(_ctx, 2))) throw FailedPredicateException(this, "precpred(_ctx, 2)");
          setState(185);
          antlrcpp::downCast<ObjectExpressionContext *>(_localctx)->keyToken = match(MaPLParser::SUBSCRIPT_OPEN);
          setState(186);
          expression(0);
          setState(187);
          match(MaPLParser::SUBSCRIPT_CLOSE);
          break;
        }

        default:
          break;
        } 
      }
      setState(193);
      _errHandler->sync(this);
      alt = getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 12, _ctx);
    }
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }
  return _localctx;
}

//----------------- VariableDeclarationContext ------------------------------------------------------------------

MaPLParser::VariableDeclarationContext::VariableDeclarationContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

MaPLParser::TypeContext* MaPLParser::VariableDeclarationContext::type() {
  return getRuleContext<MaPLParser::TypeContext>(0);
}

MaPLParser::IdentifierContext* MaPLParser::VariableDeclarationContext::identifier() {
  return getRuleContext<MaPLParser::IdentifierContext>(0);
}

tree::TerminalNode* MaPLParser::VariableDeclarationContext::ASSIGN() {
  return getToken(MaPLParser::ASSIGN, 0);
}

MaPLParser::ExpressionContext* MaPLParser::VariableDeclarationContext::expression() {
  return getRuleContext<MaPLParser::ExpressionContext>(0);
}


size_t MaPLParser::VariableDeclarationContext::getRuleIndex() const {
  return MaPLParser::RuleVariableDeclaration;
}


MaPLParser::VariableDeclarationContext* MaPLParser::variableDeclaration() {
  VariableDeclarationContext *_localctx = _tracker.createInstance<VariableDeclarationContext>(_ctx, getState());
  enterRule(_localctx, 16, MaPLParser::RuleVariableDeclaration);
  size_t _la = 0;

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(194);
    type();
    setState(195);
    identifier();
    setState(198);
    _errHandler->sync(this);

    _la = _input->LA(1);
    if (_la == MaPLParser::ASSIGN) {
      setState(196);
      match(MaPLParser::ASSIGN);
      setState(197);
      expression(0);
    }
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- TypeContext ------------------------------------------------------------------

MaPLParser::TypeContext::TypeContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* MaPLParser::TypeContext::DECL_CHAR() {
  return getToken(MaPLParser::DECL_CHAR, 0);
}

tree::TerminalNode* MaPLParser::TypeContext::DECL_INT32() {
  return getToken(MaPLParser::DECL_INT32, 0);
}

tree::TerminalNode* MaPLParser::TypeContext::DECL_INT64() {
  return getToken(MaPLParser::DECL_INT64, 0);
}

tree::TerminalNode* MaPLParser::TypeContext::DECL_UINT32() {
  return getToken(MaPLParser::DECL_UINT32, 0);
}

tree::TerminalNode* MaPLParser::TypeContext::DECL_UINT64() {
  return getToken(MaPLParser::DECL_UINT64, 0);
}

tree::TerminalNode* MaPLParser::TypeContext::DECL_FLOAT32() {
  return getToken(MaPLParser::DECL_FLOAT32, 0);
}

tree::TerminalNode* MaPLParser::TypeContext::DECL_FLOAT64() {
  return getToken(MaPLParser::DECL_FLOAT64, 0);
}

tree::TerminalNode* MaPLParser::TypeContext::DECL_BOOL() {
  return getToken(MaPLParser::DECL_BOOL, 0);
}

tree::TerminalNode* MaPLParser::TypeContext::DECL_STRING() {
  return getToken(MaPLParser::DECL_STRING, 0);
}

MaPLParser::PointerTypeContext* MaPLParser::TypeContext::pointerType() {
  return getRuleContext<MaPLParser::PointerTypeContext>(0);
}


size_t MaPLParser::TypeContext::getRuleIndex() const {
  return MaPLParser::RuleType;
}


MaPLParser::TypeContext* MaPLParser::type() {
  TypeContext *_localctx = _tracker.createInstance<TypeContext>(_ctx, getState());
  enterRule(_localctx, 18, MaPLParser::RuleType);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    setState(210);
    _errHandler->sync(this);
    switch (getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 14, _ctx)) {
    case 1: {
      enterOuterAlt(_localctx, 1);
      setState(200);
      match(MaPLParser::DECL_CHAR);
      break;
    }

    case 2: {
      enterOuterAlt(_localctx, 2);
      setState(201);
      match(MaPLParser::DECL_INT32);
      break;
    }

    case 3: {
      enterOuterAlt(_localctx, 3);
      setState(202);
      match(MaPLParser::DECL_INT64);
      break;
    }

    case 4: {
      enterOuterAlt(_localctx, 4);
      setState(203);
      match(MaPLParser::DECL_UINT32);
      break;
    }

    case 5: {
      enterOuterAlt(_localctx, 5);
      setState(204);
      match(MaPLParser::DECL_UINT64);
      break;
    }

    case 6: {
      enterOuterAlt(_localctx, 6);
      setState(205);
      match(MaPLParser::DECL_FLOAT32);
      break;
    }

    case 7: {
      enterOuterAlt(_localctx, 7);
      setState(206);
      match(MaPLParser::DECL_FLOAT64);
      break;
    }

    case 8: {
      enterOuterAlt(_localctx, 8);
      setState(207);
      match(MaPLParser::DECL_BOOL);
      break;
    }

    case 9: {
      enterOuterAlt(_localctx, 9);
      setState(208);
      match(MaPLParser::DECL_STRING);
      break;
    }

    case 10: {
      enterOuterAlt(_localctx, 10);
      setState(209);
      pointerType();
      break;
    }

    default:
      break;
    }
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- PointerTypeContext ------------------------------------------------------------------

MaPLParser::PointerTypeContext::PointerTypeContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

MaPLParser::IdentifierContext* MaPLParser::PointerTypeContext::identifier() {
  return getRuleContext<MaPLParser::IdentifierContext>(0);
}

tree::TerminalNode* MaPLParser::PointerTypeContext::LESS_THAN() {
  return getToken(MaPLParser::LESS_THAN, 0);
}

std::vector<MaPLParser::TypeContext *> MaPLParser::PointerTypeContext::type() {
  return getRuleContexts<MaPLParser::TypeContext>();
}

MaPLParser::TypeContext* MaPLParser::PointerTypeContext::type(size_t i) {
  return getRuleContext<MaPLParser::TypeContext>(i);
}

tree::TerminalNode* MaPLParser::PointerTypeContext::GREATER_THAN() {
  return getToken(MaPLParser::GREATER_THAN, 0);
}

std::vector<tree::TerminalNode *> MaPLParser::PointerTypeContext::PARAM_DELIMITER() {
  return getTokens(MaPLParser::PARAM_DELIMITER);
}

tree::TerminalNode* MaPLParser::PointerTypeContext::PARAM_DELIMITER(size_t i) {
  return getToken(MaPLParser::PARAM_DELIMITER, i);
}


size_t MaPLParser::PointerTypeContext::getRuleIndex() const {
  return MaPLParser::RulePointerType;
}


MaPLParser::PointerTypeContext* MaPLParser::pointerType() {
  PointerTypeContext *_localctx = _tracker.createInstance<PointerTypeContext>(_ctx, getState());
  enterRule(_localctx, 20, MaPLParser::RulePointerType);
  size_t _la = 0;

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(212);
    identifier();
    setState(224);
    _errHandler->sync(this);

    _la = _input->LA(1);
    if (_la == MaPLParser::LESS_THAN) {
      setState(213);
      match(MaPLParser::LESS_THAN);
      setState(214);
      type();
      setState(219);
      _errHandler->sync(this);
      _la = _input->LA(1);
      while (_la == MaPLParser::PARAM_DELIMITER) {
        setState(215);
        match(MaPLParser::PARAM_DELIMITER);
        setState(216);
        type();
        setState(221);
        _errHandler->sync(this);
        _la = _input->LA(1);
      }
      setState(222);
      match(MaPLParser::GREATER_THAN);
    }
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- ScopeContext ------------------------------------------------------------------

MaPLParser::ScopeContext::ScopeContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* MaPLParser::ScopeContext::SCOPE_OPEN() {
  return getToken(MaPLParser::SCOPE_OPEN, 0);
}

tree::TerminalNode* MaPLParser::ScopeContext::SCOPE_CLOSE() {
  return getToken(MaPLParser::SCOPE_CLOSE, 0);
}

std::vector<MaPLParser::StatementContext *> MaPLParser::ScopeContext::statement() {
  return getRuleContexts<MaPLParser::StatementContext>();
}

MaPLParser::StatementContext* MaPLParser::ScopeContext::statement(size_t i) {
  return getRuleContext<MaPLParser::StatementContext>(i);
}


size_t MaPLParser::ScopeContext::getRuleIndex() const {
  return MaPLParser::RuleScope;
}


MaPLParser::ScopeContext* MaPLParser::scope() {
  ScopeContext *_localctx = _tracker.createInstance<ScopeContext>(_ctx, getState());
  enterRule(_localctx, 22, MaPLParser::RuleScope);
  size_t _la = 0;

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(226);
    match(MaPLParser::SCOPE_OPEN);
    setState(230);
    _errHandler->sync(this);
    _la = _input->LA(1);
    while (((((_la - 33) & ~ 0x3fULL) == 0) &&
      ((1ULL << (_la - 33)) & 6854231588863) != 0)) {
      setState(227);
      statement();
      setState(232);
      _errHandler->sync(this);
      _la = _input->LA(1);
    }
    setState(233);
    match(MaPLParser::SCOPE_CLOSE);
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- WhileLoopContext ------------------------------------------------------------------

MaPLParser::WhileLoopContext::WhileLoopContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* MaPLParser::WhileLoopContext::LOOP_WHILE() {
  return getToken(MaPLParser::LOOP_WHILE, 0);
}

MaPLParser::ExpressionContext* MaPLParser::WhileLoopContext::expression() {
  return getRuleContext<MaPLParser::ExpressionContext>(0);
}

MaPLParser::ScopeContext* MaPLParser::WhileLoopContext::scope() {
  return getRuleContext<MaPLParser::ScopeContext>(0);
}


size_t MaPLParser::WhileLoopContext::getRuleIndex() const {
  return MaPLParser::RuleWhileLoop;
}


MaPLParser::WhileLoopContext* MaPLParser::whileLoop() {
  WhileLoopContext *_localctx = _tracker.createInstance<WhileLoopContext>(_ctx, getState());
  enterRule(_localctx, 24, MaPLParser::RuleWhileLoop);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(235);
    match(MaPLParser::LOOP_WHILE);
    setState(236);
    expression(0);
    setState(237);
    scope();
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- ForLoopContext ------------------------------------------------------------------

MaPLParser::ForLoopContext::ForLoopContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* MaPLParser::ForLoopContext::LOOP_FOR() {
  return getToken(MaPLParser::LOOP_FOR, 0);
}

tree::TerminalNode* MaPLParser::ForLoopContext::PAREN_OPEN() {
  return getToken(MaPLParser::PAREN_OPEN, 0);
}

MaPLParser::ForLoopControlStatementsContext* MaPLParser::ForLoopContext::forLoopControlStatements() {
  return getRuleContext<MaPLParser::ForLoopControlStatementsContext>(0);
}

tree::TerminalNode* MaPLParser::ForLoopContext::PAREN_CLOSE() {
  return getToken(MaPLParser::PAREN_CLOSE, 0);
}

MaPLParser::ScopeContext* MaPLParser::ForLoopContext::scope() {
  return getRuleContext<MaPLParser::ScopeContext>(0);
}


size_t MaPLParser::ForLoopContext::getRuleIndex() const {
  return MaPLParser::RuleForLoop;
}


MaPLParser::ForLoopContext* MaPLParser::forLoop() {
  ForLoopContext *_localctx = _tracker.createInstance<ForLoopContext>(_ctx, getState());
  enterRule(_localctx, 26, MaPLParser::RuleForLoop);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    setState(249);
    _errHandler->sync(this);
    switch (getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 18, _ctx)) {
    case 1: {
      enterOuterAlt(_localctx, 1);
      setState(239);
      match(MaPLParser::LOOP_FOR);
      setState(240);
      match(MaPLParser::PAREN_OPEN);
      setState(241);
      forLoopControlStatements();
      setState(242);
      match(MaPLParser::PAREN_CLOSE);
      setState(243);
      scope();
      break;
    }

    case 2: {
      enterOuterAlt(_localctx, 2);
      setState(245);
      match(MaPLParser::LOOP_FOR);
      setState(246);
      forLoopControlStatements();
      setState(247);
      scope();
      break;
    }

    default:
      break;
    }
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- ForLoopControlStatementsContext ------------------------------------------------------------------

MaPLParser::ForLoopControlStatementsContext::ForLoopControlStatementsContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

std::vector<tree::TerminalNode *> MaPLParser::ForLoopControlStatementsContext::STATEMENT_DELIMITER() {
  return getTokens(MaPLParser::STATEMENT_DELIMITER);
}

tree::TerminalNode* MaPLParser::ForLoopControlStatementsContext::STATEMENT_DELIMITER(size_t i) {
  return getToken(MaPLParser::STATEMENT_DELIMITER, i);
}

MaPLParser::ExpressionContext* MaPLParser::ForLoopControlStatementsContext::expression() {
  return getRuleContext<MaPLParser::ExpressionContext>(0);
}

std::vector<MaPLParser::ImperativeStatementContext *> MaPLParser::ForLoopControlStatementsContext::imperativeStatement() {
  return getRuleContexts<MaPLParser::ImperativeStatementContext>();
}

MaPLParser::ImperativeStatementContext* MaPLParser::ForLoopControlStatementsContext::imperativeStatement(size_t i) {
  return getRuleContext<MaPLParser::ImperativeStatementContext>(i);
}


size_t MaPLParser::ForLoopControlStatementsContext::getRuleIndex() const {
  return MaPLParser::RuleForLoopControlStatements;
}


MaPLParser::ForLoopControlStatementsContext* MaPLParser::forLoopControlStatements() {
  ForLoopControlStatementsContext *_localctx = _tracker.createInstance<ForLoopControlStatementsContext>(_ctx, getState());
  enterRule(_localctx, 28, MaPLParser::RuleForLoopControlStatements);
  size_t _la = 0;

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(252);
    _errHandler->sync(this);

    _la = _input->LA(1);
    if (((((_la - 33) & ~ 0x3fULL) == 0) &&
      ((1ULL << (_la - 33)) & 4651449712639) != 0)) {
      setState(251);
      antlrcpp::downCast<ForLoopControlStatementsContext *>(_localctx)->firstStatement = imperativeStatement();
    }
    setState(254);
    match(MaPLParser::STATEMENT_DELIMITER);
    setState(256);
    _errHandler->sync(this);

    _la = _input->LA(1);
    if ((((_la & ~ 0x3fULL) == 0) &&
      ((1ULL << _la) & 2251791232401424) != 0) || ((((_la - 65) & ~ 0x3fULL) == 0) &&
      ((1ULL << (_la - 65)) & 1531) != 0)) {
      setState(255);
      expression(0);
    }
    setState(258);
    match(MaPLParser::STATEMENT_DELIMITER);
    setState(260);
    _errHandler->sync(this);

    _la = _input->LA(1);
    if (((((_la - 33) & ~ 0x3fULL) == 0) &&
      ((1ULL << (_la - 33)) & 4651449712639) != 0)) {
      setState(259);
      antlrcpp::downCast<ForLoopControlStatementsContext *>(_localctx)->lastStatement = imperativeStatement();
    }
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- DoWhileLoopContext ------------------------------------------------------------------

MaPLParser::DoWhileLoopContext::DoWhileLoopContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* MaPLParser::DoWhileLoopContext::LOOP_DO() {
  return getToken(MaPLParser::LOOP_DO, 0);
}

MaPLParser::ScopeContext* MaPLParser::DoWhileLoopContext::scope() {
  return getRuleContext<MaPLParser::ScopeContext>(0);
}

tree::TerminalNode* MaPLParser::DoWhileLoopContext::LOOP_WHILE() {
  return getToken(MaPLParser::LOOP_WHILE, 0);
}

MaPLParser::ExpressionContext* MaPLParser::DoWhileLoopContext::expression() {
  return getRuleContext<MaPLParser::ExpressionContext>(0);
}

tree::TerminalNode* MaPLParser::DoWhileLoopContext::STATEMENT_DELIMITER() {
  return getToken(MaPLParser::STATEMENT_DELIMITER, 0);
}


size_t MaPLParser::DoWhileLoopContext::getRuleIndex() const {
  return MaPLParser::RuleDoWhileLoop;
}


MaPLParser::DoWhileLoopContext* MaPLParser::doWhileLoop() {
  DoWhileLoopContext *_localctx = _tracker.createInstance<DoWhileLoopContext>(_ctx, getState());
  enterRule(_localctx, 30, MaPLParser::RuleDoWhileLoop);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(262);
    match(MaPLParser::LOOP_DO);
    setState(263);
    scope();
    setState(264);
    match(MaPLParser::LOOP_WHILE);
    setState(265);
    expression(0);
    setState(266);
    match(MaPLParser::STATEMENT_DELIMITER);
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- ConditionalContext ------------------------------------------------------------------

MaPLParser::ConditionalContext::ConditionalContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* MaPLParser::ConditionalContext::CONDITIONAL() {
  return getToken(MaPLParser::CONDITIONAL, 0);
}

MaPLParser::ExpressionContext* MaPLParser::ConditionalContext::expression() {
  return getRuleContext<MaPLParser::ExpressionContext>(0);
}

MaPLParser::ScopeContext* MaPLParser::ConditionalContext::scope() {
  return getRuleContext<MaPLParser::ScopeContext>(0);
}

MaPLParser::ConditionalElseContext* MaPLParser::ConditionalContext::conditionalElse() {
  return getRuleContext<MaPLParser::ConditionalElseContext>(0);
}


size_t MaPLParser::ConditionalContext::getRuleIndex() const {
  return MaPLParser::RuleConditional;
}


MaPLParser::ConditionalContext* MaPLParser::conditional() {
  ConditionalContext *_localctx = _tracker.createInstance<ConditionalContext>(_ctx, getState());
  enterRule(_localctx, 32, MaPLParser::RuleConditional);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(268);
    match(MaPLParser::CONDITIONAL);
    setState(269);
    expression(0);
    setState(270);
    scope();
    setState(272);
    _errHandler->sync(this);

    switch (getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 22, _ctx)) {
    case 1: {
      setState(271);
      conditionalElse();
      break;
    }

    default:
      break;
    }
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- ConditionalElseContext ------------------------------------------------------------------

MaPLParser::ConditionalElseContext::ConditionalElseContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* MaPLParser::ConditionalElseContext::CONDITIONAL_ELSE() {
  return getToken(MaPLParser::CONDITIONAL_ELSE, 0);
}

MaPLParser::ScopeContext* MaPLParser::ConditionalElseContext::scope() {
  return getRuleContext<MaPLParser::ScopeContext>(0);
}

MaPLParser::ConditionalContext* MaPLParser::ConditionalElseContext::conditional() {
  return getRuleContext<MaPLParser::ConditionalContext>(0);
}


size_t MaPLParser::ConditionalElseContext::getRuleIndex() const {
  return MaPLParser::RuleConditionalElse;
}


MaPLParser::ConditionalElseContext* MaPLParser::conditionalElse() {
  ConditionalElseContext *_localctx = _tracker.createInstance<ConditionalElseContext>(_ctx, getState());
  enterRule(_localctx, 34, MaPLParser::RuleConditionalElse);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(274);
    match(MaPLParser::CONDITIONAL_ELSE);
    setState(277);
    _errHandler->sync(this);
    switch (_input->LA(1)) {
      case MaPLParser::SCOPE_OPEN: {
        setState(275);
        scope();
        break;
      }

      case MaPLParser::CONDITIONAL: {
        setState(276);
        conditional();
        break;
      }

    default:
      throw NoViableAltException(this);
    }
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- ApiGlobalContext ------------------------------------------------------------------

MaPLParser::ApiGlobalContext::ApiGlobalContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* MaPLParser::ApiGlobalContext::API_GLOBAL() {
  return getToken(MaPLParser::API_GLOBAL, 0);
}

tree::TerminalNode* MaPLParser::ApiGlobalContext::STATEMENT_DELIMITER() {
  return getToken(MaPLParser::STATEMENT_DELIMITER, 0);
}

MaPLParser::ApiFunctionContext* MaPLParser::ApiGlobalContext::apiFunction() {
  return getRuleContext<MaPLParser::ApiFunctionContext>(0);
}

MaPLParser::ApiPropertyContext* MaPLParser::ApiGlobalContext::apiProperty() {
  return getRuleContext<MaPLParser::ApiPropertyContext>(0);
}


size_t MaPLParser::ApiGlobalContext::getRuleIndex() const {
  return MaPLParser::RuleApiGlobal;
}


MaPLParser::ApiGlobalContext* MaPLParser::apiGlobal() {
  ApiGlobalContext *_localctx = _tracker.createInstance<ApiGlobalContext>(_ctx, getState());
  enterRule(_localctx, 36, MaPLParser::RuleApiGlobal);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(279);
    match(MaPLParser::API_GLOBAL);
    setState(282);
    _errHandler->sync(this);
    switch (getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 24, _ctx)) {
    case 1: {
      setState(280);
      apiFunction();
      break;
    }

    case 2: {
      setState(281);
      apiProperty();
      break;
    }

    default:
      break;
    }
    setState(284);
    match(MaPLParser::STATEMENT_DELIMITER);
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- ApiTypeContext ------------------------------------------------------------------

MaPLParser::ApiTypeContext::ApiTypeContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* MaPLParser::ApiTypeContext::API_TYPE() {
  return getToken(MaPLParser::API_TYPE, 0);
}

tree::TerminalNode* MaPLParser::ApiTypeContext::SCOPE_OPEN() {
  return getToken(MaPLParser::SCOPE_OPEN, 0);
}

tree::TerminalNode* MaPLParser::ApiTypeContext::SCOPE_CLOSE() {
  return getToken(MaPLParser::SCOPE_CLOSE, 0);
}

std::vector<MaPLParser::IdentifierContext *> MaPLParser::ApiTypeContext::identifier() {
  return getRuleContexts<MaPLParser::IdentifierContext>();
}

MaPLParser::IdentifierContext* MaPLParser::ApiTypeContext::identifier(size_t i) {
  return getRuleContext<MaPLParser::IdentifierContext>(i);
}

tree::TerminalNode* MaPLParser::ApiTypeContext::LESS_THAN() {
  return getToken(MaPLParser::LESS_THAN, 0);
}

tree::TerminalNode* MaPLParser::ApiTypeContext::GREATER_THAN() {
  return getToken(MaPLParser::GREATER_THAN, 0);
}

MaPLParser::ApiInheritanceContext* MaPLParser::ApiTypeContext::apiInheritance() {
  return getRuleContext<MaPLParser::ApiInheritanceContext>(0);
}

std::vector<tree::TerminalNode *> MaPLParser::ApiTypeContext::STATEMENT_DELIMITER() {
  return getTokens(MaPLParser::STATEMENT_DELIMITER);
}

tree::TerminalNode* MaPLParser::ApiTypeContext::STATEMENT_DELIMITER(size_t i) {
  return getToken(MaPLParser::STATEMENT_DELIMITER, i);
}

std::vector<MaPLParser::ApiFunctionContext *> MaPLParser::ApiTypeContext::apiFunction() {
  return getRuleContexts<MaPLParser::ApiFunctionContext>();
}

MaPLParser::ApiFunctionContext* MaPLParser::ApiTypeContext::apiFunction(size_t i) {
  return getRuleContext<MaPLParser::ApiFunctionContext>(i);
}

std::vector<MaPLParser::ApiPropertyContext *> MaPLParser::ApiTypeContext::apiProperty() {
  return getRuleContexts<MaPLParser::ApiPropertyContext>();
}

MaPLParser::ApiPropertyContext* MaPLParser::ApiTypeContext::apiProperty(size_t i) {
  return getRuleContext<MaPLParser::ApiPropertyContext>(i);
}

std::vector<MaPLParser::ApiSubscriptContext *> MaPLParser::ApiTypeContext::apiSubscript() {
  return getRuleContexts<MaPLParser::ApiSubscriptContext>();
}

MaPLParser::ApiSubscriptContext* MaPLParser::ApiTypeContext::apiSubscript(size_t i) {
  return getRuleContext<MaPLParser::ApiSubscriptContext>(i);
}

std::vector<tree::TerminalNode *> MaPLParser::ApiTypeContext::PARAM_DELIMITER() {
  return getTokens(MaPLParser::PARAM_DELIMITER);
}

tree::TerminalNode* MaPLParser::ApiTypeContext::PARAM_DELIMITER(size_t i) {
  return getToken(MaPLParser::PARAM_DELIMITER, i);
}


size_t MaPLParser::ApiTypeContext::getRuleIndex() const {
  return MaPLParser::RuleApiType;
}


MaPLParser::ApiTypeContext* MaPLParser::apiType() {
  ApiTypeContext *_localctx = _tracker.createInstance<ApiTypeContext>(_ctx, getState());
  enterRule(_localctx, 38, MaPLParser::RuleApiType);
  size_t _la = 0;

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(286);
    match(MaPLParser::API_TYPE);
    setState(287);
    antlrcpp::downCast<ApiTypeContext *>(_localctx)->typeName = identifier();
    setState(299);
    _errHandler->sync(this);

    _la = _input->LA(1);
    if (_la == MaPLParser::LESS_THAN) {
      setState(288);
      match(MaPLParser::LESS_THAN);
      setState(289);
      antlrcpp::downCast<ApiTypeContext *>(_localctx)->identifierContext = identifier();
      antlrcpp::downCast<ApiTypeContext *>(_localctx)->generics.push_back(antlrcpp::downCast<ApiTypeContext *>(_localctx)->identifierContext);
      setState(294);
      _errHandler->sync(this);
      _la = _input->LA(1);
      while (_la == MaPLParser::PARAM_DELIMITER) {
        setState(290);
        match(MaPLParser::PARAM_DELIMITER);
        setState(291);
        antlrcpp::downCast<ApiTypeContext *>(_localctx)->identifierContext = identifier();
        antlrcpp::downCast<ApiTypeContext *>(_localctx)->generics.push_back(antlrcpp::downCast<ApiTypeContext *>(_localctx)->identifierContext);
        setState(296);
        _errHandler->sync(this);
        _la = _input->LA(1);
      }
      setState(297);
      match(MaPLParser::GREATER_THAN);
    }
    setState(302);
    _errHandler->sync(this);

    _la = _input->LA(1);
    if (_la == MaPLParser::COLON) {
      setState(301);
      apiInheritance();
    }
    setState(304);
    match(MaPLParser::SCOPE_OPEN);
    setState(314);
    _errHandler->sync(this);
    _la = _input->LA(1);
    while (((((_la - 33) & ~ 0x3fULL) == 0) &&
      ((1ULL << (_la - 33)) & 4651449712639) != 0)) {
      setState(308);
      _errHandler->sync(this);
      switch (getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 28, _ctx)) {
      case 1: {
        setState(305);
        apiFunction();
        break;
      }

      case 2: {
        setState(306);
        apiProperty();
        break;
      }

      case 3: {
        setState(307);
        apiSubscript();
        break;
      }

      default:
        break;
      }
      setState(310);
      match(MaPLParser::STATEMENT_DELIMITER);
      setState(316);
      _errHandler->sync(this);
      _la = _input->LA(1);
    }
    setState(317);
    match(MaPLParser::SCOPE_CLOSE);
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- ApiInheritanceContext ------------------------------------------------------------------

MaPLParser::ApiInheritanceContext::ApiInheritanceContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* MaPLParser::ApiInheritanceContext::COLON() {
  return getToken(MaPLParser::COLON, 0);
}

std::vector<MaPLParser::PointerTypeContext *> MaPLParser::ApiInheritanceContext::pointerType() {
  return getRuleContexts<MaPLParser::PointerTypeContext>();
}

MaPLParser::PointerTypeContext* MaPLParser::ApiInheritanceContext::pointerType(size_t i) {
  return getRuleContext<MaPLParser::PointerTypeContext>(i);
}

std::vector<tree::TerminalNode *> MaPLParser::ApiInheritanceContext::PARAM_DELIMITER() {
  return getTokens(MaPLParser::PARAM_DELIMITER);
}

tree::TerminalNode* MaPLParser::ApiInheritanceContext::PARAM_DELIMITER(size_t i) {
  return getToken(MaPLParser::PARAM_DELIMITER, i);
}


size_t MaPLParser::ApiInheritanceContext::getRuleIndex() const {
  return MaPLParser::RuleApiInheritance;
}


MaPLParser::ApiInheritanceContext* MaPLParser::apiInheritance() {
  ApiInheritanceContext *_localctx = _tracker.createInstance<ApiInheritanceContext>(_ctx, getState());
  enterRule(_localctx, 40, MaPLParser::RuleApiInheritance);
  size_t _la = 0;

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(319);
    match(MaPLParser::COLON);
    setState(320);
    pointerType();
    setState(325);
    _errHandler->sync(this);
    _la = _input->LA(1);
    while (_la == MaPLParser::PARAM_DELIMITER) {
      setState(321);
      match(MaPLParser::PARAM_DELIMITER);
      setState(322);
      pointerType();
      setState(327);
      _errHandler->sync(this);
      _la = _input->LA(1);
    }
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- ApiFunctionContext ------------------------------------------------------------------

MaPLParser::ApiFunctionContext::ApiFunctionContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

MaPLParser::IdentifierContext* MaPLParser::ApiFunctionContext::identifier() {
  return getRuleContext<MaPLParser::IdentifierContext>(0);
}

tree::TerminalNode* MaPLParser::ApiFunctionContext::PAREN_OPEN() {
  return getToken(MaPLParser::PAREN_OPEN, 0);
}

tree::TerminalNode* MaPLParser::ApiFunctionContext::PAREN_CLOSE() {
  return getToken(MaPLParser::PAREN_CLOSE, 0);
}

tree::TerminalNode* MaPLParser::ApiFunctionContext::API_VOID() {
  return getToken(MaPLParser::API_VOID, 0);
}

MaPLParser::TypeContext* MaPLParser::ApiFunctionContext::type() {
  return getRuleContext<MaPLParser::TypeContext>(0);
}

MaPLParser::ApiFunctionParamsContext* MaPLParser::ApiFunctionContext::apiFunctionParams() {
  return getRuleContext<MaPLParser::ApiFunctionParamsContext>(0);
}


size_t MaPLParser::ApiFunctionContext::getRuleIndex() const {
  return MaPLParser::RuleApiFunction;
}


MaPLParser::ApiFunctionContext* MaPLParser::apiFunction() {
  ApiFunctionContext *_localctx = _tracker.createInstance<ApiFunctionContext>(_ctx, getState());
  enterRule(_localctx, 42, MaPLParser::RuleApiFunction);
  size_t _la = 0;

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(330);
    _errHandler->sync(this);
    switch (getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 31, _ctx)) {
    case 1: {
      setState(328);
      match(MaPLParser::API_VOID);
      break;
    }

    case 2: {
      setState(329);
      type();
      break;
    }

    default:
      break;
    }
    setState(332);
    identifier();
    setState(333);
    match(MaPLParser::PAREN_OPEN);
    setState(335);
    _errHandler->sync(this);

    _la = _input->LA(1);
    if (((((_la - 33) & ~ 0x3fULL) == 0) &&
      ((1ULL << (_la - 33)) & 4668629581823) != 0)) {
      setState(334);
      apiFunctionParams();
    }
    setState(337);
    match(MaPLParser::PAREN_CLOSE);
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- ApiFunctionParamsContext ------------------------------------------------------------------

MaPLParser::ApiFunctionParamsContext::ApiFunctionParamsContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* MaPLParser::ApiFunctionParamsContext::API_VARIADIC_PARAMETERS() {
  return getToken(MaPLParser::API_VARIADIC_PARAMETERS, 0);
}

std::vector<MaPLParser::TypeContext *> MaPLParser::ApiFunctionParamsContext::type() {
  return getRuleContexts<MaPLParser::TypeContext>();
}

MaPLParser::TypeContext* MaPLParser::ApiFunctionParamsContext::type(size_t i) {
  return getRuleContext<MaPLParser::TypeContext>(i);
}

std::vector<MaPLParser::IdentifierContext *> MaPLParser::ApiFunctionParamsContext::identifier() {
  return getRuleContexts<MaPLParser::IdentifierContext>();
}

MaPLParser::IdentifierContext* MaPLParser::ApiFunctionParamsContext::identifier(size_t i) {
  return getRuleContext<MaPLParser::IdentifierContext>(i);
}

std::vector<tree::TerminalNode *> MaPLParser::ApiFunctionParamsContext::PARAM_DELIMITER() {
  return getTokens(MaPLParser::PARAM_DELIMITER);
}

tree::TerminalNode* MaPLParser::ApiFunctionParamsContext::PARAM_DELIMITER(size_t i) {
  return getToken(MaPLParser::PARAM_DELIMITER, i);
}


size_t MaPLParser::ApiFunctionParamsContext::getRuleIndex() const {
  return MaPLParser::RuleApiFunctionParams;
}


MaPLParser::ApiFunctionParamsContext* MaPLParser::apiFunctionParams() {
  ApiFunctionParamsContext *_localctx = _tracker.createInstance<ApiFunctionParamsContext>(_ctx, getState());
  enterRule(_localctx, 44, MaPLParser::RuleApiFunctionParams);
  size_t _la = 0;

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    size_t alt;
    setState(355);
    _errHandler->sync(this);
    switch (_input->LA(1)) {
      case MaPLParser::API_VARIADIC_PARAMETERS: {
        enterOuterAlt(_localctx, 1);
        setState(339);
        match(MaPLParser::API_VARIADIC_PARAMETERS);
        break;
      }

      case MaPLParser::LOOP_WHILE:
      case MaPLParser::LOOP_FOR:
      case MaPLParser::LOOP_DO:
      case MaPLParser::CONDITIONAL:
      case MaPLParser::CONDITIONAL_ELSE:
      case MaPLParser::BREAK:
      case MaPLParser::CONTINUE:
      case MaPLParser::EXIT:
      case MaPLParser::DECL_CHAR:
      case MaPLParser::DECL_INT32:
      case MaPLParser::DECL_INT64:
      case MaPLParser::DECL_UINT32:
      case MaPLParser::DECL_UINT64:
      case MaPLParser::DECL_FLOAT32:
      case MaPLParser::DECL_FLOAT64:
      case MaPLParser::DECL_BOOL:
      case MaPLParser::DECL_STRING:
      case MaPLParser::API_READONLY:
      case MaPLParser::API_VOID:
      case MaPLParser::LITERAL_NULL:
      case MaPLParser::LITERAL_TRUE:
      case MaPLParser::LITERAL_FALSE:
      case MaPLParser::IDENTIFIER: {
        enterOuterAlt(_localctx, 2);
        setState(340);
        type();
        setState(341);
        identifier();
        setState(348);
        _errHandler->sync(this);
        alt = getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 33, _ctx);
        while (alt != 2 && alt != atn::ATN::INVALID_ALT_NUMBER) {
          if (alt == 1) {
            setState(342);
            match(MaPLParser::PARAM_DELIMITER);
            setState(343);
            type();
            setState(344);
            identifier(); 
          }
          setState(350);
          _errHandler->sync(this);
          alt = getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 33, _ctx);
        }
        setState(353);
        _errHandler->sync(this);

        _la = _input->LA(1);
        if (_la == MaPLParser::PARAM_DELIMITER) {
          setState(351);
          match(MaPLParser::PARAM_DELIMITER);
          setState(352);
          match(MaPLParser::API_VARIADIC_PARAMETERS);
        }
        break;
      }

    default:
      throw NoViableAltException(this);
    }
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- ApiPropertyContext ------------------------------------------------------------------

MaPLParser::ApiPropertyContext::ApiPropertyContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

MaPLParser::TypeContext* MaPLParser::ApiPropertyContext::type() {
  return getRuleContext<MaPLParser::TypeContext>(0);
}

MaPLParser::IdentifierContext* MaPLParser::ApiPropertyContext::identifier() {
  return getRuleContext<MaPLParser::IdentifierContext>(0);
}

tree::TerminalNode* MaPLParser::ApiPropertyContext::API_READONLY() {
  return getToken(MaPLParser::API_READONLY, 0);
}


size_t MaPLParser::ApiPropertyContext::getRuleIndex() const {
  return MaPLParser::RuleApiProperty;
}


MaPLParser::ApiPropertyContext* MaPLParser::apiProperty() {
  ApiPropertyContext *_localctx = _tracker.createInstance<ApiPropertyContext>(_ctx, getState());
  enterRule(_localctx, 46, MaPLParser::RuleApiProperty);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(358);
    _errHandler->sync(this);

    switch (getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 36, _ctx)) {
    case 1: {
      setState(357);
      match(MaPLParser::API_READONLY);
      break;
    }

    default:
      break;
    }
    setState(360);
    type();
    setState(361);
    identifier();
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- ApiSubscriptContext ------------------------------------------------------------------

MaPLParser::ApiSubscriptContext::ApiSubscriptContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

std::vector<MaPLParser::TypeContext *> MaPLParser::ApiSubscriptContext::type() {
  return getRuleContexts<MaPLParser::TypeContext>();
}

MaPLParser::TypeContext* MaPLParser::ApiSubscriptContext::type(size_t i) {
  return getRuleContext<MaPLParser::TypeContext>(i);
}

tree::TerminalNode* MaPLParser::ApiSubscriptContext::SUBSCRIPT_OPEN() {
  return getToken(MaPLParser::SUBSCRIPT_OPEN, 0);
}

tree::TerminalNode* MaPLParser::ApiSubscriptContext::SUBSCRIPT_CLOSE() {
  return getToken(MaPLParser::SUBSCRIPT_CLOSE, 0);
}

tree::TerminalNode* MaPLParser::ApiSubscriptContext::API_READONLY() {
  return getToken(MaPLParser::API_READONLY, 0);
}


size_t MaPLParser::ApiSubscriptContext::getRuleIndex() const {
  return MaPLParser::RuleApiSubscript;
}


MaPLParser::ApiSubscriptContext* MaPLParser::apiSubscript() {
  ApiSubscriptContext *_localctx = _tracker.createInstance<ApiSubscriptContext>(_ctx, getState());
  enterRule(_localctx, 48, MaPLParser::RuleApiSubscript);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(364);
    _errHandler->sync(this);

    switch (getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 37, _ctx)) {
    case 1: {
      setState(363);
      match(MaPLParser::API_READONLY);
      break;
    }

    default:
      break;
    }
    setState(366);
    type();
    setState(367);
    match(MaPLParser::SUBSCRIPT_OPEN);
    setState(368);
    type();
    setState(369);
    match(MaPLParser::SUBSCRIPT_CLOSE);
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- ApiImportContext ------------------------------------------------------------------

MaPLParser::ApiImportContext::ApiImportContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* MaPLParser::ApiImportContext::API_IMPORT() {
  return getToken(MaPLParser::API_IMPORT, 0);
}

tree::TerminalNode* MaPLParser::ApiImportContext::LITERAL_STRING() {
  return getToken(MaPLParser::LITERAL_STRING, 0);
}


size_t MaPLParser::ApiImportContext::getRuleIndex() const {
  return MaPLParser::RuleApiImport;
}


MaPLParser::ApiImportContext* MaPLParser::apiImport() {
  ApiImportContext *_localctx = _tracker.createInstance<ApiImportContext>(_ctx, getState());
  enterRule(_localctx, 50, MaPLParser::RuleApiImport);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(371);
    match(MaPLParser::API_IMPORT);
    setState(372);
    match(MaPLParser::LITERAL_STRING);
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- IdentifierContext ------------------------------------------------------------------

MaPLParser::IdentifierContext::IdentifierContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* MaPLParser::IdentifierContext::LOOP_WHILE() {
  return getToken(MaPLParser::LOOP_WHILE, 0);
}

tree::TerminalNode* MaPLParser::IdentifierContext::LOOP_FOR() {
  return getToken(MaPLParser::LOOP_FOR, 0);
}

tree::TerminalNode* MaPLParser::IdentifierContext::LOOP_DO() {
  return getToken(MaPLParser::LOOP_DO, 0);
}

tree::TerminalNode* MaPLParser::IdentifierContext::CONDITIONAL() {
  return getToken(MaPLParser::CONDITIONAL, 0);
}

tree::TerminalNode* MaPLParser::IdentifierContext::CONDITIONAL_ELSE() {
  return getToken(MaPLParser::CONDITIONAL_ELSE, 0);
}

tree::TerminalNode* MaPLParser::IdentifierContext::BREAK() {
  return getToken(MaPLParser::BREAK, 0);
}

tree::TerminalNode* MaPLParser::IdentifierContext::CONTINUE() {
  return getToken(MaPLParser::CONTINUE, 0);
}

tree::TerminalNode* MaPLParser::IdentifierContext::EXIT() {
  return getToken(MaPLParser::EXIT, 0);
}

tree::TerminalNode* MaPLParser::IdentifierContext::LITERAL_NULL() {
  return getToken(MaPLParser::LITERAL_NULL, 0);
}

tree::TerminalNode* MaPLParser::IdentifierContext::API_READONLY() {
  return getToken(MaPLParser::API_READONLY, 0);
}

tree::TerminalNode* MaPLParser::IdentifierContext::API_VOID() {
  return getToken(MaPLParser::API_VOID, 0);
}

tree::TerminalNode* MaPLParser::IdentifierContext::DECL_CHAR() {
  return getToken(MaPLParser::DECL_CHAR, 0);
}

tree::TerminalNode* MaPLParser::IdentifierContext::DECL_INT32() {
  return getToken(MaPLParser::DECL_INT32, 0);
}

tree::TerminalNode* MaPLParser::IdentifierContext::DECL_INT64() {
  return getToken(MaPLParser::DECL_INT64, 0);
}

tree::TerminalNode* MaPLParser::IdentifierContext::DECL_UINT32() {
  return getToken(MaPLParser::DECL_UINT32, 0);
}

tree::TerminalNode* MaPLParser::IdentifierContext::DECL_UINT64() {
  return getToken(MaPLParser::DECL_UINT64, 0);
}

tree::TerminalNode* MaPLParser::IdentifierContext::DECL_FLOAT32() {
  return getToken(MaPLParser::DECL_FLOAT32, 0);
}

tree::TerminalNode* MaPLParser::IdentifierContext::DECL_FLOAT64() {
  return getToken(MaPLParser::DECL_FLOAT64, 0);
}

tree::TerminalNode* MaPLParser::IdentifierContext::DECL_BOOL() {
  return getToken(MaPLParser::DECL_BOOL, 0);
}

tree::TerminalNode* MaPLParser::IdentifierContext::DECL_STRING() {
  return getToken(MaPLParser::DECL_STRING, 0);
}

tree::TerminalNode* MaPLParser::IdentifierContext::LITERAL_TRUE() {
  return getToken(MaPLParser::LITERAL_TRUE, 0);
}

tree::TerminalNode* MaPLParser::IdentifierContext::LITERAL_FALSE() {
  return getToken(MaPLParser::LITERAL_FALSE, 0);
}

tree::TerminalNode* MaPLParser::IdentifierContext::IDENTIFIER() {
  return getToken(MaPLParser::IDENTIFIER, 0);
}


size_t MaPLParser::IdentifierContext::getRuleIndex() const {
  return MaPLParser::RuleIdentifier;
}


MaPLParser::IdentifierContext* MaPLParser::identifier() {
  IdentifierContext *_localctx = _tracker.createInstance<IdentifierContext>(_ctx, getState());
  enterRule(_localctx, 52, MaPLParser::RuleIdentifier);
  size_t _la = 0;

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(374);
    _la = _input->LA(1);
    if (!(((((_la - 33) & ~ 0x3fULL) == 0) &&
      ((1ULL << (_la - 33)) & 4651449712639) != 0))) {
    _errHandler->recoverInline(this);
    }
    else {
      _errHandler->reportMatch(this);
      consume();
    }
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- BitwiseShiftRightContext ------------------------------------------------------------------

MaPLParser::BitwiseShiftRightContext::BitwiseShiftRightContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

std::vector<tree::TerminalNode *> MaPLParser::BitwiseShiftRightContext::GREATER_THAN() {
  return getTokens(MaPLParser::GREATER_THAN);
}

tree::TerminalNode* MaPLParser::BitwiseShiftRightContext::GREATER_THAN(size_t i) {
  return getToken(MaPLParser::GREATER_THAN, i);
}


size_t MaPLParser::BitwiseShiftRightContext::getRuleIndex() const {
  return MaPLParser::RuleBitwiseShiftRight;
}


MaPLParser::BitwiseShiftRightContext* MaPLParser::bitwiseShiftRight() {
  BitwiseShiftRightContext *_localctx = _tracker.createInstance<BitwiseShiftRightContext>(_ctx, getState());
  enterRule(_localctx, 54, MaPLParser::RuleBitwiseShiftRight);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(376);
    match(MaPLParser::GREATER_THAN);
    setState(377);
    match(MaPLParser::GREATER_THAN);
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

bool MaPLParser::sempred(RuleContext *context, size_t ruleIndex, size_t predicateIndex) {
  switch (ruleIndex) {
    case 6: return expressionSempred(antlrcpp::downCast<ExpressionContext *>(context), predicateIndex);
    case 7: return objectExpressionSempred(antlrcpp::downCast<ObjectExpressionContext *>(context), predicateIndex);

  default:
    break;
  }
  return true;
}

bool MaPLParser::expressionSempred(ExpressionContext *_localctx, size_t predicateIndex) {
  switch (predicateIndex) {
    case 0: return precpred(_ctx, 17);
    case 1: return precpred(_ctx, 16);
    case 2: return precpred(_ctx, 15);
    case 3: return precpred(_ctx, 14);
    case 4: return precpred(_ctx, 13);
    case 5: return precpred(_ctx, 12);
    case 6: return precpred(_ctx, 11);
    case 7: return precpred(_ctx, 10);
    case 8: return precpred(_ctx, 9);

  default:
    break;
  }
  return true;
}

bool MaPLParser::objectExpressionSempred(ObjectExpressionContext *_localctx, size_t predicateIndex) {
  switch (predicateIndex) {
    case 9: return precpred(_ctx, 3);
    case 10: return precpred(_ctx, 2);

  default:
    break;
  }
  return true;
}

void MaPLParser::initialize() {
#if ANTLR4_USE_THREAD_LOCAL_CACHE
  maplparserParserInitialize();
#else
  ::antlr4::internal::call_once(maplparserParserOnceFlag, maplparserParserInitialize);
#endif
}

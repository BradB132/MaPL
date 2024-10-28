
// Generated from ./EaSL.g4 by ANTLR 4.13.2


#include "EaSLLexer.h"


using namespace antlr4;



using namespace antlr4;

namespace {

struct EaSLLexerStaticData final {
  EaSLLexerStaticData(std::vector<std::string> ruleNames,
                          std::vector<std::string> channelNames,
                          std::vector<std::string> modeNames,
                          std::vector<std::string> literalNames,
                          std::vector<std::string> symbolicNames)
      : ruleNames(std::move(ruleNames)), channelNames(std::move(channelNames)),
        modeNames(std::move(modeNames)), literalNames(std::move(literalNames)),
        symbolicNames(std::move(symbolicNames)),
        vocabulary(this->literalNames, this->symbolicNames) {}

  EaSLLexerStaticData(const EaSLLexerStaticData&) = delete;
  EaSLLexerStaticData(EaSLLexerStaticData&&) = delete;
  EaSLLexerStaticData& operator=(const EaSLLexerStaticData&) = delete;
  EaSLLexerStaticData& operator=(EaSLLexerStaticData&&) = delete;

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

::antlr4::internal::OnceFlag easllexerLexerOnceFlag;
#if ANTLR4_USE_THREAD_LOCAL_CACHE
static thread_local
#endif
std::unique_ptr<EaSLLexerStaticData> easllexerLexerStaticData = nullptr;

void easllexerLexerInitialize() {
#if ANTLR4_USE_THREAD_LOCAL_CACHE
  if (easllexerLexerStaticData != nullptr) {
    return;
  }
#else
  assert(easllexerLexerStaticData == nullptr);
#endif
  auto staticData = std::make_unique<EaSLLexerStaticData>(
    std::vector<std::string>{
      "DECL_CHAR", "DECL_INT32", "DECL_INT64", "DECL_UINT32", "DECL_UINT64", 
      "DECL_FLOAT32", "DECL_FLOAT64", "DECL_BOOL", "DECL_STRING", "DECL_UID", 
      "REFERENCE", "REFERENCE_OPEN", "REFERENCE_CLOSE", "NAMESPACE", "CLASS", 
      "ENUM", "DEFAULTS_TO", "DEFINITION_OPEN", "DEFINITION_CLOSE", "SEQUENCE_OPEN", 
      "SEQUENCE_CLOSE", "SEQUENCE_DELIMITER", "SEQUENCE_WILDCARD", "COLON", 
      "NAMESPACE_DELIMITER", "STATEMENT_END", "ANNOTATION", "LITERAL_NULL", 
      "LITERAL_TRUE", "LITERAL_FALSE", "LITERAL_INT", "LITERAL_FLOAT", "DIGITS", 
      "LITERAL_STRING", "STRING_ESC", "IDENTIFIER", "IDENTIFIER_FRAGMENT", 
      "REGEX", "BLOCK_COMMENT", "LINE_COMMENT", "WHITESPACE"
    },
    std::vector<std::string>{
      "DEFAULT_TOKEN_CHANNEL", "HIDDEN"
    },
    std::vector<std::string>{
      "DEFAULT_MODE"
    },
    std::vector<std::string>{
      "", "'char'", "'int32'", "'int64'", "'uint32'", "'uint64'", "'float32'", 
      "'float64'", "'bool'", "'string'", "'UID'", "'reference'", "'<'", 
      "'>'", "'namespace'", "'class'", "'enum'", "'='", "'{'", "'}'", "'['", 
      "']'", "','", "'*'", "':'", "'::'", "';'", "", "'NULL'", "'true'", 
      "'false'"
    },
    std::vector<std::string>{
      "", "DECL_CHAR", "DECL_INT32", "DECL_INT64", "DECL_UINT32", "DECL_UINT64", 
      "DECL_FLOAT32", "DECL_FLOAT64", "DECL_BOOL", "DECL_STRING", "DECL_UID", 
      "REFERENCE", "REFERENCE_OPEN", "REFERENCE_CLOSE", "NAMESPACE", "CLASS", 
      "ENUM", "DEFAULTS_TO", "DEFINITION_OPEN", "DEFINITION_CLOSE", "SEQUENCE_OPEN", 
      "SEQUENCE_CLOSE", "SEQUENCE_DELIMITER", "SEQUENCE_WILDCARD", "COLON", 
      "NAMESPACE_DELIMITER", "STATEMENT_END", "ANNOTATION", "LITERAL_NULL", 
      "LITERAL_TRUE", "LITERAL_FALSE", "LITERAL_INT", "LITERAL_FLOAT", "LITERAL_STRING", 
      "IDENTIFIER", "REGEX", "BLOCK_COMMENT", "LINE_COMMENT", "WHITESPACE"
    }
  );
  static const int32_t serializedATNSegment[] = {
  	4,0,38,319,6,-1,2,0,7,0,2,1,7,1,2,2,7,2,2,3,7,3,2,4,7,4,2,5,7,5,2,6,7,
  	6,2,7,7,7,2,8,7,8,2,9,7,9,2,10,7,10,2,11,7,11,2,12,7,12,2,13,7,13,2,14,
  	7,14,2,15,7,15,2,16,7,16,2,17,7,17,2,18,7,18,2,19,7,19,2,20,7,20,2,21,
  	7,21,2,22,7,22,2,23,7,23,2,24,7,24,2,25,7,25,2,26,7,26,2,27,7,27,2,28,
  	7,28,2,29,7,29,2,30,7,30,2,31,7,31,2,32,7,32,2,33,7,33,2,34,7,34,2,35,
  	7,35,2,36,7,36,2,37,7,37,2,38,7,38,2,39,7,39,2,40,7,40,1,0,1,0,1,0,1,
  	0,1,0,1,1,1,1,1,1,1,1,1,1,1,1,1,2,1,2,1,2,1,2,1,2,1,2,1,3,1,3,1,3,1,3,
  	1,3,1,3,1,3,1,4,1,4,1,4,1,4,1,4,1,4,1,4,1,5,1,5,1,5,1,5,1,5,1,5,1,5,1,
  	5,1,6,1,6,1,6,1,6,1,6,1,6,1,6,1,6,1,7,1,7,1,7,1,7,1,7,1,8,1,8,1,8,1,8,
  	1,8,1,8,1,8,1,9,1,9,1,9,1,9,1,10,1,10,1,10,1,10,1,10,1,10,1,10,1,10,1,
  	10,1,10,1,11,1,11,1,12,1,12,1,13,1,13,1,13,1,13,1,13,1,13,1,13,1,13,1,
  	13,1,13,1,14,1,14,1,14,1,14,1,14,1,14,1,15,1,15,1,15,1,15,1,15,1,16,1,
  	16,1,17,1,17,1,18,1,18,1,19,1,19,1,20,1,20,1,21,1,21,1,22,1,22,1,23,1,
  	23,1,24,1,24,1,24,1,25,1,25,1,26,1,26,1,26,1,26,4,26,207,8,26,11,26,12,
  	26,208,3,26,211,8,26,1,27,1,27,1,27,1,27,1,27,1,28,1,28,1,28,1,28,1,28,
  	1,29,1,29,1,29,1,29,1,29,1,29,1,30,3,30,230,8,30,1,30,1,30,1,31,3,31,
  	235,8,31,1,31,3,31,238,8,31,1,31,1,31,1,31,1,32,4,32,244,8,32,11,32,12,
  	32,245,1,33,1,33,1,33,5,33,251,8,33,10,33,12,33,254,9,33,1,33,1,33,1,
  	34,1,34,1,34,1,34,3,34,262,8,34,1,35,1,35,1,36,1,36,5,36,268,8,36,10,
  	36,12,36,271,9,36,1,37,1,37,1,37,5,37,276,8,37,10,37,12,37,279,9,37,1,
  	37,1,37,1,38,1,38,1,38,1,38,5,38,287,8,38,10,38,12,38,290,9,38,1,38,1,
  	38,1,38,1,38,1,38,1,39,1,39,1,39,1,39,5,39,301,8,39,10,39,12,39,304,9,
  	39,1,39,3,39,307,8,39,1,39,1,39,1,39,1,39,1,40,4,40,314,8,40,11,40,12,
  	40,315,1,40,1,40,4,252,277,288,302,0,41,1,1,3,2,5,3,7,4,9,5,11,6,13,7,
  	15,8,17,9,19,10,21,11,23,12,25,13,27,14,29,15,31,16,33,17,35,18,37,19,
  	39,20,41,21,43,22,45,23,47,24,49,25,51,26,53,27,55,28,57,29,59,30,61,
  	31,63,32,65,0,67,33,69,0,71,34,73,0,75,35,77,36,79,37,81,38,1,0,5,4,0,
  	48,57,65,90,95,95,97,122,1,0,48,57,3,0,65,90,95,95,97,122,2,0,42,42,47,
  	47,3,0,9,10,13,13,32,32,330,0,1,1,0,0,0,0,3,1,0,0,0,0,5,1,0,0,0,0,7,1,
  	0,0,0,0,9,1,0,0,0,0,11,1,0,0,0,0,13,1,0,0,0,0,15,1,0,0,0,0,17,1,0,0,0,
  	0,19,1,0,0,0,0,21,1,0,0,0,0,23,1,0,0,0,0,25,1,0,0,0,0,27,1,0,0,0,0,29,
  	1,0,0,0,0,31,1,0,0,0,0,33,1,0,0,0,0,35,1,0,0,0,0,37,1,0,0,0,0,39,1,0,
  	0,0,0,41,1,0,0,0,0,43,1,0,0,0,0,45,1,0,0,0,0,47,1,0,0,0,0,49,1,0,0,0,
  	0,51,1,0,0,0,0,53,1,0,0,0,0,55,1,0,0,0,0,57,1,0,0,0,0,59,1,0,0,0,0,61,
  	1,0,0,0,0,63,1,0,0,0,0,67,1,0,0,0,0,71,1,0,0,0,0,75,1,0,0,0,0,77,1,0,
  	0,0,0,79,1,0,0,0,0,81,1,0,0,0,1,83,1,0,0,0,3,88,1,0,0,0,5,94,1,0,0,0,
  	7,100,1,0,0,0,9,107,1,0,0,0,11,114,1,0,0,0,13,122,1,0,0,0,15,130,1,0,
  	0,0,17,135,1,0,0,0,19,142,1,0,0,0,21,146,1,0,0,0,23,156,1,0,0,0,25,158,
  	1,0,0,0,27,160,1,0,0,0,29,170,1,0,0,0,31,176,1,0,0,0,33,181,1,0,0,0,35,
  	183,1,0,0,0,37,185,1,0,0,0,39,187,1,0,0,0,41,189,1,0,0,0,43,191,1,0,0,
  	0,45,193,1,0,0,0,47,195,1,0,0,0,49,197,1,0,0,0,51,200,1,0,0,0,53,202,
  	1,0,0,0,55,212,1,0,0,0,57,217,1,0,0,0,59,222,1,0,0,0,61,229,1,0,0,0,63,
  	234,1,0,0,0,65,243,1,0,0,0,67,247,1,0,0,0,69,261,1,0,0,0,71,263,1,0,0,
  	0,73,265,1,0,0,0,75,272,1,0,0,0,77,282,1,0,0,0,79,296,1,0,0,0,81,313,
  	1,0,0,0,83,84,5,99,0,0,84,85,5,104,0,0,85,86,5,97,0,0,86,87,5,114,0,0,
  	87,2,1,0,0,0,88,89,5,105,0,0,89,90,5,110,0,0,90,91,5,116,0,0,91,92,5,
  	51,0,0,92,93,5,50,0,0,93,4,1,0,0,0,94,95,5,105,0,0,95,96,5,110,0,0,96,
  	97,5,116,0,0,97,98,5,54,0,0,98,99,5,52,0,0,99,6,1,0,0,0,100,101,5,117,
  	0,0,101,102,5,105,0,0,102,103,5,110,0,0,103,104,5,116,0,0,104,105,5,51,
  	0,0,105,106,5,50,0,0,106,8,1,0,0,0,107,108,5,117,0,0,108,109,5,105,0,
  	0,109,110,5,110,0,0,110,111,5,116,0,0,111,112,5,54,0,0,112,113,5,52,0,
  	0,113,10,1,0,0,0,114,115,5,102,0,0,115,116,5,108,0,0,116,117,5,111,0,
  	0,117,118,5,97,0,0,118,119,5,116,0,0,119,120,5,51,0,0,120,121,5,50,0,
  	0,121,12,1,0,0,0,122,123,5,102,0,0,123,124,5,108,0,0,124,125,5,111,0,
  	0,125,126,5,97,0,0,126,127,5,116,0,0,127,128,5,54,0,0,128,129,5,52,0,
  	0,129,14,1,0,0,0,130,131,5,98,0,0,131,132,5,111,0,0,132,133,5,111,0,0,
  	133,134,5,108,0,0,134,16,1,0,0,0,135,136,5,115,0,0,136,137,5,116,0,0,
  	137,138,5,114,0,0,138,139,5,105,0,0,139,140,5,110,0,0,140,141,5,103,0,
  	0,141,18,1,0,0,0,142,143,5,85,0,0,143,144,5,73,0,0,144,145,5,68,0,0,145,
  	20,1,0,0,0,146,147,5,114,0,0,147,148,5,101,0,0,148,149,5,102,0,0,149,
  	150,5,101,0,0,150,151,5,114,0,0,151,152,5,101,0,0,152,153,5,110,0,0,153,
  	154,5,99,0,0,154,155,5,101,0,0,155,22,1,0,0,0,156,157,5,60,0,0,157,24,
  	1,0,0,0,158,159,5,62,0,0,159,26,1,0,0,0,160,161,5,110,0,0,161,162,5,97,
  	0,0,162,163,5,109,0,0,163,164,5,101,0,0,164,165,5,115,0,0,165,166,5,112,
  	0,0,166,167,5,97,0,0,167,168,5,99,0,0,168,169,5,101,0,0,169,28,1,0,0,
  	0,170,171,5,99,0,0,171,172,5,108,0,0,172,173,5,97,0,0,173,174,5,115,0,
  	0,174,175,5,115,0,0,175,30,1,0,0,0,176,177,5,101,0,0,177,178,5,110,0,
  	0,178,179,5,117,0,0,179,180,5,109,0,0,180,32,1,0,0,0,181,182,5,61,0,0,
  	182,34,1,0,0,0,183,184,5,123,0,0,184,36,1,0,0,0,185,186,5,125,0,0,186,
  	38,1,0,0,0,187,188,5,91,0,0,188,40,1,0,0,0,189,190,5,93,0,0,190,42,1,
  	0,0,0,191,192,5,44,0,0,192,44,1,0,0,0,193,194,5,42,0,0,194,46,1,0,0,0,
  	195,196,5,58,0,0,196,48,1,0,0,0,197,198,5,58,0,0,198,199,5,58,0,0,199,
  	50,1,0,0,0,200,201,5,59,0,0,201,52,1,0,0,0,202,203,5,64,0,0,203,210,3,
  	73,36,0,204,206,5,61,0,0,205,207,7,0,0,0,206,205,1,0,0,0,207,208,1,0,
  	0,0,208,206,1,0,0,0,208,209,1,0,0,0,209,211,1,0,0,0,210,204,1,0,0,0,210,
  	211,1,0,0,0,211,54,1,0,0,0,212,213,5,78,0,0,213,214,5,85,0,0,214,215,
  	5,76,0,0,215,216,5,76,0,0,216,56,1,0,0,0,217,218,5,116,0,0,218,219,5,
  	114,0,0,219,220,5,117,0,0,220,221,5,101,0,0,221,58,1,0,0,0,222,223,5,
  	102,0,0,223,224,5,97,0,0,224,225,5,108,0,0,225,226,5,115,0,0,226,227,
  	5,101,0,0,227,60,1,0,0,0,228,230,5,45,0,0,229,228,1,0,0,0,229,230,1,0,
  	0,0,230,231,1,0,0,0,231,232,3,65,32,0,232,62,1,0,0,0,233,235,5,45,0,0,
  	234,233,1,0,0,0,234,235,1,0,0,0,235,237,1,0,0,0,236,238,3,65,32,0,237,
  	236,1,0,0,0,237,238,1,0,0,0,238,239,1,0,0,0,239,240,5,46,0,0,240,241,
  	3,65,32,0,241,64,1,0,0,0,242,244,7,1,0,0,243,242,1,0,0,0,244,245,1,0,
  	0,0,245,243,1,0,0,0,245,246,1,0,0,0,246,66,1,0,0,0,247,252,5,34,0,0,248,
  	251,3,69,34,0,249,251,9,0,0,0,250,248,1,0,0,0,250,249,1,0,0,0,251,254,
  	1,0,0,0,252,253,1,0,0,0,252,250,1,0,0,0,253,255,1,0,0,0,254,252,1,0,0,
  	0,255,256,5,34,0,0,256,68,1,0,0,0,257,258,5,92,0,0,258,262,5,34,0,0,259,
  	260,5,92,0,0,260,262,5,92,0,0,261,257,1,0,0,0,261,259,1,0,0,0,262,70,
  	1,0,0,0,263,264,3,73,36,0,264,72,1,0,0,0,265,269,7,2,0,0,266,268,7,0,
  	0,0,267,266,1,0,0,0,268,271,1,0,0,0,269,267,1,0,0,0,269,270,1,0,0,0,270,
  	74,1,0,0,0,271,269,1,0,0,0,272,273,5,47,0,0,273,277,8,3,0,0,274,276,9,
  	0,0,0,275,274,1,0,0,0,276,279,1,0,0,0,277,278,1,0,0,0,277,275,1,0,0,0,
  	278,280,1,0,0,0,279,277,1,0,0,0,280,281,5,47,0,0,281,76,1,0,0,0,282,283,
  	5,47,0,0,283,284,5,42,0,0,284,288,1,0,0,0,285,287,9,0,0,0,286,285,1,0,
  	0,0,287,290,1,0,0,0,288,289,1,0,0,0,288,286,1,0,0,0,289,291,1,0,0,0,290,
  	288,1,0,0,0,291,292,5,42,0,0,292,293,5,47,0,0,293,294,1,0,0,0,294,295,
  	6,38,0,0,295,78,1,0,0,0,296,297,5,47,0,0,297,298,5,47,0,0,298,302,1,0,
  	0,0,299,301,9,0,0,0,300,299,1,0,0,0,301,304,1,0,0,0,302,303,1,0,0,0,302,
  	300,1,0,0,0,303,306,1,0,0,0,304,302,1,0,0,0,305,307,5,13,0,0,306,305,
  	1,0,0,0,306,307,1,0,0,0,307,308,1,0,0,0,308,309,5,10,0,0,309,310,1,0,
  	0,0,310,311,6,39,0,0,311,80,1,0,0,0,312,314,7,4,0,0,313,312,1,0,0,0,314,
  	315,1,0,0,0,315,313,1,0,0,0,315,316,1,0,0,0,316,317,1,0,0,0,317,318,6,
  	40,0,0,318,82,1,0,0,0,16,0,208,210,229,234,237,245,250,252,261,269,277,
  	288,302,306,315,1,6,0,0
  };
  staticData->serializedATN = antlr4::atn::SerializedATNView(serializedATNSegment, sizeof(serializedATNSegment) / sizeof(serializedATNSegment[0]));

  antlr4::atn::ATNDeserializer deserializer;
  staticData->atn = deserializer.deserialize(staticData->serializedATN);

  const size_t count = staticData->atn->getNumberOfDecisions();
  staticData->decisionToDFA.reserve(count);
  for (size_t i = 0; i < count; i++) { 
    staticData->decisionToDFA.emplace_back(staticData->atn->getDecisionState(i), i);
  }
  easllexerLexerStaticData = std::move(staticData);
}

}

EaSLLexer::EaSLLexer(CharStream *input) : Lexer(input) {
  EaSLLexer::initialize();
  _interpreter = new atn::LexerATNSimulator(this, *easllexerLexerStaticData->atn, easllexerLexerStaticData->decisionToDFA, easllexerLexerStaticData->sharedContextCache);
}

EaSLLexer::~EaSLLexer() {
  delete _interpreter;
}

std::string EaSLLexer::getGrammarFileName() const {
  return "EaSL.g4";
}

const std::vector<std::string>& EaSLLexer::getRuleNames() const {
  return easllexerLexerStaticData->ruleNames;
}

const std::vector<std::string>& EaSLLexer::getChannelNames() const {
  return easllexerLexerStaticData->channelNames;
}

const std::vector<std::string>& EaSLLexer::getModeNames() const {
  return easllexerLexerStaticData->modeNames;
}

const dfa::Vocabulary& EaSLLexer::getVocabulary() const {
  return easllexerLexerStaticData->vocabulary;
}

antlr4::atn::SerializedATNView EaSLLexer::getSerializedATN() const {
  return easllexerLexerStaticData->serializedATN;
}

const atn::ATN& EaSLLexer::getATN() const {
  return *easllexerLexerStaticData->atn;
}




void EaSLLexer::initialize() {
#if ANTLR4_USE_THREAD_LOCAL_CACHE
  easllexerLexerInitialize();
#else
  ::antlr4::internal::call_once(easllexerLexerOnceFlag, easllexerLexerInitialize);
#endif
}

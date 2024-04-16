
// Generated from ./EaSL.g4 by ANTLR 4.13.1



#include "EaSLParser.h"


using namespace antlrcpp;

using namespace antlr4;

namespace {

struct EaSLParserStaticData final {
  EaSLParserStaticData(std::vector<std::string> ruleNames,
                        std::vector<std::string> literalNames,
                        std::vector<std::string> symbolicNames)
      : ruleNames(std::move(ruleNames)), literalNames(std::move(literalNames)),
        symbolicNames(std::move(symbolicNames)),
        vocabulary(this->literalNames, this->symbolicNames) {}

  EaSLParserStaticData(const EaSLParserStaticData&) = delete;
  EaSLParserStaticData(EaSLParserStaticData&&) = delete;
  EaSLParserStaticData& operator=(const EaSLParserStaticData&) = delete;
  EaSLParserStaticData& operator=(EaSLParserStaticData&&) = delete;

  std::vector<antlr4::dfa::DFA> decisionToDFA;
  antlr4::atn::PredictionContextCache sharedContextCache;
  const std::vector<std::string> ruleNames;
  const std::vector<std::string> literalNames;
  const std::vector<std::string> symbolicNames;
  const antlr4::dfa::Vocabulary vocabulary;
  antlr4::atn::SerializedATNView serializedATN;
  std::unique_ptr<antlr4::atn::ATN> atn;
};

::antlr4::internal::OnceFlag easlParserOnceFlag;
#if ANTLR4_USE_THREAD_LOCAL_CACHE
static thread_local
#endif
EaSLParserStaticData *easlParserStaticData = nullptr;

void easlParserInitialize() {
#if ANTLR4_USE_THREAD_LOCAL_CACHE
  if (easlParserStaticData != nullptr) {
    return;
  }
#else
  assert(easlParserStaticData == nullptr);
#endif
  auto staticData = std::make_unique<EaSLParserStaticData>(
    std::vector<std::string>{
      "schema", "namespace_", "classDefinition", "attribute", "sequenceDescriptor", 
      "sequenceLength", "enumDefinition", "defaultValue", "literalValue", 
      "type", "classType", "identifier"
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
      "IDENTIFIER", "BLOCK_COMMENT", "LINE_COMMENT", "WHITESPACE"
    }
  );
  static const int32_t serializedATNSegment[] = {
  	4,1,37,156,2,0,7,0,2,1,7,1,2,2,7,2,2,3,7,3,2,4,7,4,2,5,7,5,2,6,7,6,2,
  	7,7,7,2,8,7,8,2,9,7,9,2,10,7,10,2,11,7,11,1,0,1,0,1,0,5,0,28,8,0,10,0,
  	12,0,31,9,0,1,0,1,0,1,1,1,1,1,1,1,1,1,2,5,2,40,8,2,10,2,12,2,43,9,2,1,
  	2,1,2,1,2,1,2,3,2,49,8,2,1,2,1,2,5,2,53,8,2,10,2,12,2,56,9,2,1,2,1,2,
  	1,3,5,3,61,8,3,10,3,12,3,64,9,3,1,3,1,3,1,3,3,3,69,8,3,1,3,1,3,3,3,73,
  	8,3,1,3,1,3,1,4,1,4,1,4,1,4,3,4,81,8,4,3,4,83,8,4,1,4,1,4,1,5,1,5,1,6,
  	5,6,90,8,6,10,6,12,6,93,9,6,1,6,1,6,1,6,1,6,1,6,1,6,5,6,101,8,6,10,6,
  	12,6,104,9,6,1,6,1,6,3,6,108,8,6,1,6,1,6,1,7,1,7,1,7,5,7,115,8,7,10,7,
  	12,7,118,9,7,1,8,1,8,1,8,1,8,1,8,1,8,1,8,3,8,127,8,8,1,9,1,9,1,9,1,9,
  	1,9,1,9,1,9,1,9,1,9,1,9,1,9,1,9,1,9,1,9,1,9,1,9,3,9,145,8,9,1,10,1,10,
  	1,10,3,10,150,8,10,1,10,1,10,1,11,1,11,1,11,0,0,12,0,2,4,6,8,10,12,14,
  	16,18,20,22,0,2,2,0,23,23,31,31,4,0,1,11,14,16,28,30,34,34,175,0,24,1,
  	0,0,0,2,34,1,0,0,0,4,41,1,0,0,0,6,62,1,0,0,0,8,76,1,0,0,0,10,86,1,0,0,
  	0,12,91,1,0,0,0,14,111,1,0,0,0,16,126,1,0,0,0,18,144,1,0,0,0,20,149,1,
  	0,0,0,22,153,1,0,0,0,24,29,3,2,1,0,25,28,3,4,2,0,26,28,3,12,6,0,27,25,
  	1,0,0,0,27,26,1,0,0,0,28,31,1,0,0,0,29,27,1,0,0,0,29,30,1,0,0,0,30,32,
  	1,0,0,0,31,29,1,0,0,0,32,33,5,0,0,1,33,1,1,0,0,0,34,35,5,14,0,0,35,36,
  	3,22,11,0,36,37,5,26,0,0,37,3,1,0,0,0,38,40,5,27,0,0,39,38,1,0,0,0,40,
  	43,1,0,0,0,41,39,1,0,0,0,41,42,1,0,0,0,42,44,1,0,0,0,43,41,1,0,0,0,44,
  	45,5,15,0,0,45,48,3,22,11,0,46,47,5,24,0,0,47,49,3,20,10,0,48,46,1,0,
  	0,0,48,49,1,0,0,0,49,50,1,0,0,0,50,54,5,18,0,0,51,53,3,6,3,0,52,51,1,
  	0,0,0,53,56,1,0,0,0,54,52,1,0,0,0,54,55,1,0,0,0,55,57,1,0,0,0,56,54,1,
  	0,0,0,57,58,5,19,0,0,58,5,1,0,0,0,59,61,5,27,0,0,60,59,1,0,0,0,61,64,
  	1,0,0,0,62,60,1,0,0,0,62,63,1,0,0,0,63,65,1,0,0,0,64,62,1,0,0,0,65,66,
  	3,18,9,0,66,68,3,22,11,0,67,69,3,8,4,0,68,67,1,0,0,0,68,69,1,0,0,0,69,
  	72,1,0,0,0,70,71,5,17,0,0,71,73,3,14,7,0,72,70,1,0,0,0,72,73,1,0,0,0,
  	73,74,1,0,0,0,74,75,5,26,0,0,75,7,1,0,0,0,76,82,5,20,0,0,77,80,3,10,5,
  	0,78,79,5,22,0,0,79,81,3,10,5,0,80,78,1,0,0,0,80,81,1,0,0,0,81,83,1,0,
  	0,0,82,77,1,0,0,0,82,83,1,0,0,0,83,84,1,0,0,0,84,85,5,21,0,0,85,9,1,0,
  	0,0,86,87,7,0,0,0,87,11,1,0,0,0,88,90,5,27,0,0,89,88,1,0,0,0,90,93,1,
  	0,0,0,91,89,1,0,0,0,91,92,1,0,0,0,92,94,1,0,0,0,93,91,1,0,0,0,94,95,5,
  	16,0,0,95,96,3,22,11,0,96,102,5,18,0,0,97,98,3,22,11,0,98,99,5,22,0,0,
  	99,101,1,0,0,0,100,97,1,0,0,0,101,104,1,0,0,0,102,100,1,0,0,0,102,103,
  	1,0,0,0,103,105,1,0,0,0,104,102,1,0,0,0,105,107,3,22,11,0,106,108,5,22,
  	0,0,107,106,1,0,0,0,107,108,1,0,0,0,108,109,1,0,0,0,109,110,5,19,0,0,
  	110,13,1,0,0,0,111,116,3,16,8,0,112,113,5,22,0,0,113,115,3,16,8,0,114,
  	112,1,0,0,0,115,118,1,0,0,0,116,114,1,0,0,0,116,117,1,0,0,0,117,15,1,
  	0,0,0,118,116,1,0,0,0,119,127,5,28,0,0,120,127,5,29,0,0,121,127,5,30,
  	0,0,122,127,5,33,0,0,123,127,5,31,0,0,124,127,5,32,0,0,125,127,3,22,11,
  	0,126,119,1,0,0,0,126,120,1,0,0,0,126,121,1,0,0,0,126,122,1,0,0,0,126,
  	123,1,0,0,0,126,124,1,0,0,0,126,125,1,0,0,0,127,17,1,0,0,0,128,145,5,
  	1,0,0,129,145,5,2,0,0,130,145,5,3,0,0,131,145,5,4,0,0,132,145,5,5,0,0,
  	133,145,5,6,0,0,134,145,5,7,0,0,135,145,5,8,0,0,136,145,5,9,0,0,137,145,
  	5,10,0,0,138,139,5,11,0,0,139,140,5,12,0,0,140,141,3,20,10,0,141,142,
  	5,13,0,0,142,145,1,0,0,0,143,145,3,20,10,0,144,128,1,0,0,0,144,129,1,
  	0,0,0,144,130,1,0,0,0,144,131,1,0,0,0,144,132,1,0,0,0,144,133,1,0,0,0,
  	144,134,1,0,0,0,144,135,1,0,0,0,144,136,1,0,0,0,144,137,1,0,0,0,144,138,
  	1,0,0,0,144,143,1,0,0,0,145,19,1,0,0,0,146,147,3,22,11,0,147,148,5,25,
  	0,0,148,150,1,0,0,0,149,146,1,0,0,0,149,150,1,0,0,0,150,151,1,0,0,0,151,
  	152,3,22,11,0,152,21,1,0,0,0,153,154,7,1,0,0,154,23,1,0,0,0,17,27,29,
  	41,48,54,62,68,72,80,82,91,102,107,116,126,144,149
  };
  staticData->serializedATN = antlr4::atn::SerializedATNView(serializedATNSegment, sizeof(serializedATNSegment) / sizeof(serializedATNSegment[0]));

  antlr4::atn::ATNDeserializer deserializer;
  staticData->atn = deserializer.deserialize(staticData->serializedATN);

  const size_t count = staticData->atn->getNumberOfDecisions();
  staticData->decisionToDFA.reserve(count);
  for (size_t i = 0; i < count; i++) { 
    staticData->decisionToDFA.emplace_back(staticData->atn->getDecisionState(i), i);
  }
  easlParserStaticData = staticData.release();
}

}

EaSLParser::EaSLParser(TokenStream *input) : EaSLParser(input, antlr4::atn::ParserATNSimulatorOptions()) {}

EaSLParser::EaSLParser(TokenStream *input, const antlr4::atn::ParserATNSimulatorOptions &options) : Parser(input) {
  EaSLParser::initialize();
  _interpreter = new atn::ParserATNSimulator(this, *easlParserStaticData->atn, easlParserStaticData->decisionToDFA, easlParserStaticData->sharedContextCache, options);
}

EaSLParser::~EaSLParser() {
  delete _interpreter;
}

const atn::ATN& EaSLParser::getATN() const {
  return *easlParserStaticData->atn;
}

std::string EaSLParser::getGrammarFileName() const {
  return "EaSL.g4";
}

const std::vector<std::string>& EaSLParser::getRuleNames() const {
  return easlParserStaticData->ruleNames;
}

const dfa::Vocabulary& EaSLParser::getVocabulary() const {
  return easlParserStaticData->vocabulary;
}

antlr4::atn::SerializedATNView EaSLParser::getSerializedATN() const {
  return easlParserStaticData->serializedATN;
}


//----------------- SchemaContext ------------------------------------------------------------------

EaSLParser::SchemaContext::SchemaContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

EaSLParser::Namespace_Context* EaSLParser::SchemaContext::namespace_() {
  return getRuleContext<EaSLParser::Namespace_Context>(0);
}

tree::TerminalNode* EaSLParser::SchemaContext::EOF() {
  return getToken(EaSLParser::EOF, 0);
}

std::vector<EaSLParser::ClassDefinitionContext *> EaSLParser::SchemaContext::classDefinition() {
  return getRuleContexts<EaSLParser::ClassDefinitionContext>();
}

EaSLParser::ClassDefinitionContext* EaSLParser::SchemaContext::classDefinition(size_t i) {
  return getRuleContext<EaSLParser::ClassDefinitionContext>(i);
}

std::vector<EaSLParser::EnumDefinitionContext *> EaSLParser::SchemaContext::enumDefinition() {
  return getRuleContexts<EaSLParser::EnumDefinitionContext>();
}

EaSLParser::EnumDefinitionContext* EaSLParser::SchemaContext::enumDefinition(size_t i) {
  return getRuleContext<EaSLParser::EnumDefinitionContext>(i);
}


size_t EaSLParser::SchemaContext::getRuleIndex() const {
  return EaSLParser::RuleSchema;
}


EaSLParser::SchemaContext* EaSLParser::schema() {
  SchemaContext *_localctx = _tracker.createInstance<SchemaContext>(_ctx, getState());
  enterRule(_localctx, 0, EaSLParser::RuleSchema);
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
    setState(24);
    namespace_();
    setState(29);
    _errHandler->sync(this);
    _la = _input->LA(1);
    while ((((_la & ~ 0x3fULL) == 0) &&
      ((1ULL << _la) & 134316032) != 0)) {
      setState(27);
      _errHandler->sync(this);
      switch (getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 0, _ctx)) {
      case 1: {
        setState(25);
        classDefinition();
        break;
      }

      case 2: {
        setState(26);
        enumDefinition();
        break;
      }

      default:
        break;
      }
      setState(31);
      _errHandler->sync(this);
      _la = _input->LA(1);
    }
    setState(32);
    match(EaSLParser::EOF);
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- Namespace_Context ------------------------------------------------------------------

EaSLParser::Namespace_Context::Namespace_Context(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* EaSLParser::Namespace_Context::NAMESPACE() {
  return getToken(EaSLParser::NAMESPACE, 0);
}

EaSLParser::IdentifierContext* EaSLParser::Namespace_Context::identifier() {
  return getRuleContext<EaSLParser::IdentifierContext>(0);
}

tree::TerminalNode* EaSLParser::Namespace_Context::STATEMENT_END() {
  return getToken(EaSLParser::STATEMENT_END, 0);
}


size_t EaSLParser::Namespace_Context::getRuleIndex() const {
  return EaSLParser::RuleNamespace_;
}


EaSLParser::Namespace_Context* EaSLParser::namespace_() {
  Namespace_Context *_localctx = _tracker.createInstance<Namespace_Context>(_ctx, getState());
  enterRule(_localctx, 2, EaSLParser::RuleNamespace_);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(34);
    match(EaSLParser::NAMESPACE);
    setState(35);
    identifier();
    setState(36);
    match(EaSLParser::STATEMENT_END);
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- ClassDefinitionContext ------------------------------------------------------------------

EaSLParser::ClassDefinitionContext::ClassDefinitionContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* EaSLParser::ClassDefinitionContext::CLASS() {
  return getToken(EaSLParser::CLASS, 0);
}

EaSLParser::IdentifierContext* EaSLParser::ClassDefinitionContext::identifier() {
  return getRuleContext<EaSLParser::IdentifierContext>(0);
}

tree::TerminalNode* EaSLParser::ClassDefinitionContext::DEFINITION_OPEN() {
  return getToken(EaSLParser::DEFINITION_OPEN, 0);
}

tree::TerminalNode* EaSLParser::ClassDefinitionContext::DEFINITION_CLOSE() {
  return getToken(EaSLParser::DEFINITION_CLOSE, 0);
}

std::vector<tree::TerminalNode *> EaSLParser::ClassDefinitionContext::ANNOTATION() {
  return getTokens(EaSLParser::ANNOTATION);
}

tree::TerminalNode* EaSLParser::ClassDefinitionContext::ANNOTATION(size_t i) {
  return getToken(EaSLParser::ANNOTATION, i);
}

tree::TerminalNode* EaSLParser::ClassDefinitionContext::COLON() {
  return getToken(EaSLParser::COLON, 0);
}

EaSLParser::ClassTypeContext* EaSLParser::ClassDefinitionContext::classType() {
  return getRuleContext<EaSLParser::ClassTypeContext>(0);
}

std::vector<EaSLParser::AttributeContext *> EaSLParser::ClassDefinitionContext::attribute() {
  return getRuleContexts<EaSLParser::AttributeContext>();
}

EaSLParser::AttributeContext* EaSLParser::ClassDefinitionContext::attribute(size_t i) {
  return getRuleContext<EaSLParser::AttributeContext>(i);
}


size_t EaSLParser::ClassDefinitionContext::getRuleIndex() const {
  return EaSLParser::RuleClassDefinition;
}


EaSLParser::ClassDefinitionContext* EaSLParser::classDefinition() {
  ClassDefinitionContext *_localctx = _tracker.createInstance<ClassDefinitionContext>(_ctx, getState());
  enterRule(_localctx, 4, EaSLParser::RuleClassDefinition);
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
    setState(41);
    _errHandler->sync(this);
    _la = _input->LA(1);
    while (_la == EaSLParser::ANNOTATION) {
      setState(38);
      match(EaSLParser::ANNOTATION);
      setState(43);
      _errHandler->sync(this);
      _la = _input->LA(1);
    }
    setState(44);
    match(EaSLParser::CLASS);
    setState(45);
    identifier();
    setState(48);
    _errHandler->sync(this);

    _la = _input->LA(1);
    if (_la == EaSLParser::COLON) {
      setState(46);
      match(EaSLParser::COLON);
      setState(47);
      classType();
    }
    setState(50);
    match(EaSLParser::DEFINITION_OPEN);
    setState(54);
    _errHandler->sync(this);
    _la = _input->LA(1);
    while ((((_la & ~ 0x3fULL) == 0) &&
      ((1ULL << _la) & 19193253886) != 0)) {
      setState(51);
      attribute();
      setState(56);
      _errHandler->sync(this);
      _la = _input->LA(1);
    }
    setState(57);
    match(EaSLParser::DEFINITION_CLOSE);
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- AttributeContext ------------------------------------------------------------------

EaSLParser::AttributeContext::AttributeContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

EaSLParser::TypeContext* EaSLParser::AttributeContext::type() {
  return getRuleContext<EaSLParser::TypeContext>(0);
}

EaSLParser::IdentifierContext* EaSLParser::AttributeContext::identifier() {
  return getRuleContext<EaSLParser::IdentifierContext>(0);
}

tree::TerminalNode* EaSLParser::AttributeContext::STATEMENT_END() {
  return getToken(EaSLParser::STATEMENT_END, 0);
}

std::vector<tree::TerminalNode *> EaSLParser::AttributeContext::ANNOTATION() {
  return getTokens(EaSLParser::ANNOTATION);
}

tree::TerminalNode* EaSLParser::AttributeContext::ANNOTATION(size_t i) {
  return getToken(EaSLParser::ANNOTATION, i);
}

EaSLParser::SequenceDescriptorContext* EaSLParser::AttributeContext::sequenceDescriptor() {
  return getRuleContext<EaSLParser::SequenceDescriptorContext>(0);
}

tree::TerminalNode* EaSLParser::AttributeContext::DEFAULTS_TO() {
  return getToken(EaSLParser::DEFAULTS_TO, 0);
}

EaSLParser::DefaultValueContext* EaSLParser::AttributeContext::defaultValue() {
  return getRuleContext<EaSLParser::DefaultValueContext>(0);
}


size_t EaSLParser::AttributeContext::getRuleIndex() const {
  return EaSLParser::RuleAttribute;
}


EaSLParser::AttributeContext* EaSLParser::attribute() {
  AttributeContext *_localctx = _tracker.createInstance<AttributeContext>(_ctx, getState());
  enterRule(_localctx, 6, EaSLParser::RuleAttribute);
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
    setState(62);
    _errHandler->sync(this);
    _la = _input->LA(1);
    while (_la == EaSLParser::ANNOTATION) {
      setState(59);
      match(EaSLParser::ANNOTATION);
      setState(64);
      _errHandler->sync(this);
      _la = _input->LA(1);
    }
    setState(65);
    type();
    setState(66);
    identifier();
    setState(68);
    _errHandler->sync(this);

    _la = _input->LA(1);
    if (_la == EaSLParser::SEQUENCE_OPEN) {
      setState(67);
      sequenceDescriptor();
    }
    setState(72);
    _errHandler->sync(this);

    _la = _input->LA(1);
    if (_la == EaSLParser::DEFAULTS_TO) {
      setState(70);
      match(EaSLParser::DEFAULTS_TO);
      setState(71);
      defaultValue();
    }
    setState(74);
    match(EaSLParser::STATEMENT_END);
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- SequenceDescriptorContext ------------------------------------------------------------------

EaSLParser::SequenceDescriptorContext::SequenceDescriptorContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* EaSLParser::SequenceDescriptorContext::SEQUENCE_OPEN() {
  return getToken(EaSLParser::SEQUENCE_OPEN, 0);
}

tree::TerminalNode* EaSLParser::SequenceDescriptorContext::SEQUENCE_CLOSE() {
  return getToken(EaSLParser::SEQUENCE_CLOSE, 0);
}

std::vector<EaSLParser::SequenceLengthContext *> EaSLParser::SequenceDescriptorContext::sequenceLength() {
  return getRuleContexts<EaSLParser::SequenceLengthContext>();
}

EaSLParser::SequenceLengthContext* EaSLParser::SequenceDescriptorContext::sequenceLength(size_t i) {
  return getRuleContext<EaSLParser::SequenceLengthContext>(i);
}

tree::TerminalNode* EaSLParser::SequenceDescriptorContext::SEQUENCE_DELIMITER() {
  return getToken(EaSLParser::SEQUENCE_DELIMITER, 0);
}


size_t EaSLParser::SequenceDescriptorContext::getRuleIndex() const {
  return EaSLParser::RuleSequenceDescriptor;
}


EaSLParser::SequenceDescriptorContext* EaSLParser::sequenceDescriptor() {
  SequenceDescriptorContext *_localctx = _tracker.createInstance<SequenceDescriptorContext>(_ctx, getState());
  enterRule(_localctx, 8, EaSLParser::RuleSequenceDescriptor);
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
    setState(76);
    match(EaSLParser::SEQUENCE_OPEN);
    setState(82);
    _errHandler->sync(this);

    _la = _input->LA(1);
    if (_la == EaSLParser::SEQUENCE_WILDCARD

    || _la == EaSLParser::LITERAL_INT) {
      setState(77);
      sequenceLength();
      setState(80);
      _errHandler->sync(this);

      _la = _input->LA(1);
      if (_la == EaSLParser::SEQUENCE_DELIMITER) {
        setState(78);
        match(EaSLParser::SEQUENCE_DELIMITER);
        setState(79);
        sequenceLength();
      }
    }
    setState(84);
    match(EaSLParser::SEQUENCE_CLOSE);
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- SequenceLengthContext ------------------------------------------------------------------

EaSLParser::SequenceLengthContext::SequenceLengthContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* EaSLParser::SequenceLengthContext::LITERAL_INT() {
  return getToken(EaSLParser::LITERAL_INT, 0);
}

tree::TerminalNode* EaSLParser::SequenceLengthContext::SEQUENCE_WILDCARD() {
  return getToken(EaSLParser::SEQUENCE_WILDCARD, 0);
}


size_t EaSLParser::SequenceLengthContext::getRuleIndex() const {
  return EaSLParser::RuleSequenceLength;
}


EaSLParser::SequenceLengthContext* EaSLParser::sequenceLength() {
  SequenceLengthContext *_localctx = _tracker.createInstance<SequenceLengthContext>(_ctx, getState());
  enterRule(_localctx, 10, EaSLParser::RuleSequenceLength);
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
    setState(86);
    _la = _input->LA(1);
    if (!(_la == EaSLParser::SEQUENCE_WILDCARD

    || _la == EaSLParser::LITERAL_INT)) {
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

//----------------- EnumDefinitionContext ------------------------------------------------------------------

EaSLParser::EnumDefinitionContext::EnumDefinitionContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* EaSLParser::EnumDefinitionContext::ENUM() {
  return getToken(EaSLParser::ENUM, 0);
}

tree::TerminalNode* EaSLParser::EnumDefinitionContext::DEFINITION_OPEN() {
  return getToken(EaSLParser::DEFINITION_OPEN, 0);
}

tree::TerminalNode* EaSLParser::EnumDefinitionContext::DEFINITION_CLOSE() {
  return getToken(EaSLParser::DEFINITION_CLOSE, 0);
}

std::vector<EaSLParser::IdentifierContext *> EaSLParser::EnumDefinitionContext::identifier() {
  return getRuleContexts<EaSLParser::IdentifierContext>();
}

EaSLParser::IdentifierContext* EaSLParser::EnumDefinitionContext::identifier(size_t i) {
  return getRuleContext<EaSLParser::IdentifierContext>(i);
}

std::vector<tree::TerminalNode *> EaSLParser::EnumDefinitionContext::ANNOTATION() {
  return getTokens(EaSLParser::ANNOTATION);
}

tree::TerminalNode* EaSLParser::EnumDefinitionContext::ANNOTATION(size_t i) {
  return getToken(EaSLParser::ANNOTATION, i);
}

std::vector<tree::TerminalNode *> EaSLParser::EnumDefinitionContext::SEQUENCE_DELIMITER() {
  return getTokens(EaSLParser::SEQUENCE_DELIMITER);
}

tree::TerminalNode* EaSLParser::EnumDefinitionContext::SEQUENCE_DELIMITER(size_t i) {
  return getToken(EaSLParser::SEQUENCE_DELIMITER, i);
}


size_t EaSLParser::EnumDefinitionContext::getRuleIndex() const {
  return EaSLParser::RuleEnumDefinition;
}


EaSLParser::EnumDefinitionContext* EaSLParser::enumDefinition() {
  EnumDefinitionContext *_localctx = _tracker.createInstance<EnumDefinitionContext>(_ctx, getState());
  enterRule(_localctx, 12, EaSLParser::RuleEnumDefinition);
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
    enterOuterAlt(_localctx, 1);
    setState(91);
    _errHandler->sync(this);
    _la = _input->LA(1);
    while (_la == EaSLParser::ANNOTATION) {
      setState(88);
      match(EaSLParser::ANNOTATION);
      setState(93);
      _errHandler->sync(this);
      _la = _input->LA(1);
    }
    setState(94);
    match(EaSLParser::ENUM);
    setState(95);
    antlrcpp::downCast<EnumDefinitionContext *>(_localctx)->enumName = identifier();
    setState(96);
    match(EaSLParser::DEFINITION_OPEN);
    setState(102);
    _errHandler->sync(this);
    alt = getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 11, _ctx);
    while (alt != 2 && alt != atn::ATN::INVALID_ALT_NUMBER) {
      if (alt == 1) {
        setState(97);
        antlrcpp::downCast<EnumDefinitionContext *>(_localctx)->identifierContext = identifier();
        antlrcpp::downCast<EnumDefinitionContext *>(_localctx)->enumValue.push_back(antlrcpp::downCast<EnumDefinitionContext *>(_localctx)->identifierContext);
        setState(98);
        match(EaSLParser::SEQUENCE_DELIMITER); 
      }
      setState(104);
      _errHandler->sync(this);
      alt = getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 11, _ctx);
    }
    setState(105);
    antlrcpp::downCast<EnumDefinitionContext *>(_localctx)->identifierContext = identifier();
    antlrcpp::downCast<EnumDefinitionContext *>(_localctx)->enumValue.push_back(antlrcpp::downCast<EnumDefinitionContext *>(_localctx)->identifierContext);
    setState(107);
    _errHandler->sync(this);

    _la = _input->LA(1);
    if (_la == EaSLParser::SEQUENCE_DELIMITER) {
      setState(106);
      match(EaSLParser::SEQUENCE_DELIMITER);
    }
    setState(109);
    match(EaSLParser::DEFINITION_CLOSE);
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- DefaultValueContext ------------------------------------------------------------------

EaSLParser::DefaultValueContext::DefaultValueContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

std::vector<EaSLParser::LiteralValueContext *> EaSLParser::DefaultValueContext::literalValue() {
  return getRuleContexts<EaSLParser::LiteralValueContext>();
}

EaSLParser::LiteralValueContext* EaSLParser::DefaultValueContext::literalValue(size_t i) {
  return getRuleContext<EaSLParser::LiteralValueContext>(i);
}

std::vector<tree::TerminalNode *> EaSLParser::DefaultValueContext::SEQUENCE_DELIMITER() {
  return getTokens(EaSLParser::SEQUENCE_DELIMITER);
}

tree::TerminalNode* EaSLParser::DefaultValueContext::SEQUENCE_DELIMITER(size_t i) {
  return getToken(EaSLParser::SEQUENCE_DELIMITER, i);
}


size_t EaSLParser::DefaultValueContext::getRuleIndex() const {
  return EaSLParser::RuleDefaultValue;
}


EaSLParser::DefaultValueContext* EaSLParser::defaultValue() {
  DefaultValueContext *_localctx = _tracker.createInstance<DefaultValueContext>(_ctx, getState());
  enterRule(_localctx, 14, EaSLParser::RuleDefaultValue);
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
    setState(111);
    literalValue();
    setState(116);
    _errHandler->sync(this);
    _la = _input->LA(1);
    while (_la == EaSLParser::SEQUENCE_DELIMITER) {
      setState(112);
      match(EaSLParser::SEQUENCE_DELIMITER);
      setState(113);
      literalValue();
      setState(118);
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

//----------------- LiteralValueContext ------------------------------------------------------------------

EaSLParser::LiteralValueContext::LiteralValueContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* EaSLParser::LiteralValueContext::LITERAL_NULL() {
  return getToken(EaSLParser::LITERAL_NULL, 0);
}

tree::TerminalNode* EaSLParser::LiteralValueContext::LITERAL_TRUE() {
  return getToken(EaSLParser::LITERAL_TRUE, 0);
}

tree::TerminalNode* EaSLParser::LiteralValueContext::LITERAL_FALSE() {
  return getToken(EaSLParser::LITERAL_FALSE, 0);
}

tree::TerminalNode* EaSLParser::LiteralValueContext::LITERAL_STRING() {
  return getToken(EaSLParser::LITERAL_STRING, 0);
}

tree::TerminalNode* EaSLParser::LiteralValueContext::LITERAL_INT() {
  return getToken(EaSLParser::LITERAL_INT, 0);
}

tree::TerminalNode* EaSLParser::LiteralValueContext::LITERAL_FLOAT() {
  return getToken(EaSLParser::LITERAL_FLOAT, 0);
}

EaSLParser::IdentifierContext* EaSLParser::LiteralValueContext::identifier() {
  return getRuleContext<EaSLParser::IdentifierContext>(0);
}


size_t EaSLParser::LiteralValueContext::getRuleIndex() const {
  return EaSLParser::RuleLiteralValue;
}


EaSLParser::LiteralValueContext* EaSLParser::literalValue() {
  LiteralValueContext *_localctx = _tracker.createInstance<LiteralValueContext>(_ctx, getState());
  enterRule(_localctx, 16, EaSLParser::RuleLiteralValue);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    setState(126);
    _errHandler->sync(this);
    switch (getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 14, _ctx)) {
    case 1: {
      enterOuterAlt(_localctx, 1);
      setState(119);
      antlrcpp::downCast<LiteralValueContext *>(_localctx)->literalToken = match(EaSLParser::LITERAL_NULL);
      break;
    }

    case 2: {
      enterOuterAlt(_localctx, 2);
      setState(120);
      antlrcpp::downCast<LiteralValueContext *>(_localctx)->literalToken = match(EaSLParser::LITERAL_TRUE);
      break;
    }

    case 3: {
      enterOuterAlt(_localctx, 3);
      setState(121);
      antlrcpp::downCast<LiteralValueContext *>(_localctx)->literalToken = match(EaSLParser::LITERAL_FALSE);
      break;
    }

    case 4: {
      enterOuterAlt(_localctx, 4);
      setState(122);
      antlrcpp::downCast<LiteralValueContext *>(_localctx)->literalToken = match(EaSLParser::LITERAL_STRING);
      break;
    }

    case 5: {
      enterOuterAlt(_localctx, 5);
      setState(123);
      antlrcpp::downCast<LiteralValueContext *>(_localctx)->literalToken = match(EaSLParser::LITERAL_INT);
      break;
    }

    case 6: {
      enterOuterAlt(_localctx, 6);
      setState(124);
      antlrcpp::downCast<LiteralValueContext *>(_localctx)->literalToken = match(EaSLParser::LITERAL_FLOAT);
      break;
    }

    case 7: {
      enterOuterAlt(_localctx, 7);
      setState(125);
      identifier();
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

//----------------- TypeContext ------------------------------------------------------------------

EaSLParser::TypeContext::TypeContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* EaSLParser::TypeContext::DECL_CHAR() {
  return getToken(EaSLParser::DECL_CHAR, 0);
}

tree::TerminalNode* EaSLParser::TypeContext::DECL_INT32() {
  return getToken(EaSLParser::DECL_INT32, 0);
}

tree::TerminalNode* EaSLParser::TypeContext::DECL_INT64() {
  return getToken(EaSLParser::DECL_INT64, 0);
}

tree::TerminalNode* EaSLParser::TypeContext::DECL_UINT32() {
  return getToken(EaSLParser::DECL_UINT32, 0);
}

tree::TerminalNode* EaSLParser::TypeContext::DECL_UINT64() {
  return getToken(EaSLParser::DECL_UINT64, 0);
}

tree::TerminalNode* EaSLParser::TypeContext::DECL_FLOAT32() {
  return getToken(EaSLParser::DECL_FLOAT32, 0);
}

tree::TerminalNode* EaSLParser::TypeContext::DECL_FLOAT64() {
  return getToken(EaSLParser::DECL_FLOAT64, 0);
}

tree::TerminalNode* EaSLParser::TypeContext::DECL_BOOL() {
  return getToken(EaSLParser::DECL_BOOL, 0);
}

tree::TerminalNode* EaSLParser::TypeContext::DECL_STRING() {
  return getToken(EaSLParser::DECL_STRING, 0);
}

tree::TerminalNode* EaSLParser::TypeContext::DECL_UID() {
  return getToken(EaSLParser::DECL_UID, 0);
}

tree::TerminalNode* EaSLParser::TypeContext::REFERENCE_OPEN() {
  return getToken(EaSLParser::REFERENCE_OPEN, 0);
}

EaSLParser::ClassTypeContext* EaSLParser::TypeContext::classType() {
  return getRuleContext<EaSLParser::ClassTypeContext>(0);
}

tree::TerminalNode* EaSLParser::TypeContext::REFERENCE_CLOSE() {
  return getToken(EaSLParser::REFERENCE_CLOSE, 0);
}

tree::TerminalNode* EaSLParser::TypeContext::REFERENCE() {
  return getToken(EaSLParser::REFERENCE, 0);
}


size_t EaSLParser::TypeContext::getRuleIndex() const {
  return EaSLParser::RuleType;
}


EaSLParser::TypeContext* EaSLParser::type() {
  TypeContext *_localctx = _tracker.createInstance<TypeContext>(_ctx, getState());
  enterRule(_localctx, 18, EaSLParser::RuleType);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    setState(144);
    _errHandler->sync(this);
    switch (getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 15, _ctx)) {
    case 1: {
      enterOuterAlt(_localctx, 1);
      setState(128);
      antlrcpp::downCast<TypeContext *>(_localctx)->typeToken = match(EaSLParser::DECL_CHAR);
      break;
    }

    case 2: {
      enterOuterAlt(_localctx, 2);
      setState(129);
      antlrcpp::downCast<TypeContext *>(_localctx)->typeToken = match(EaSLParser::DECL_INT32);
      break;
    }

    case 3: {
      enterOuterAlt(_localctx, 3);
      setState(130);
      antlrcpp::downCast<TypeContext *>(_localctx)->typeToken = match(EaSLParser::DECL_INT64);
      break;
    }

    case 4: {
      enterOuterAlt(_localctx, 4);
      setState(131);
      antlrcpp::downCast<TypeContext *>(_localctx)->typeToken = match(EaSLParser::DECL_UINT32);
      break;
    }

    case 5: {
      enterOuterAlt(_localctx, 5);
      setState(132);
      antlrcpp::downCast<TypeContext *>(_localctx)->typeToken = match(EaSLParser::DECL_UINT64);
      break;
    }

    case 6: {
      enterOuterAlt(_localctx, 6);
      setState(133);
      antlrcpp::downCast<TypeContext *>(_localctx)->typeToken = match(EaSLParser::DECL_FLOAT32);
      break;
    }

    case 7: {
      enterOuterAlt(_localctx, 7);
      setState(134);
      antlrcpp::downCast<TypeContext *>(_localctx)->typeToken = match(EaSLParser::DECL_FLOAT64);
      break;
    }

    case 8: {
      enterOuterAlt(_localctx, 8);
      setState(135);
      antlrcpp::downCast<TypeContext *>(_localctx)->typeToken = match(EaSLParser::DECL_BOOL);
      break;
    }

    case 9: {
      enterOuterAlt(_localctx, 9);
      setState(136);
      antlrcpp::downCast<TypeContext *>(_localctx)->typeToken = match(EaSLParser::DECL_STRING);
      break;
    }

    case 10: {
      enterOuterAlt(_localctx, 10);
      setState(137);
      antlrcpp::downCast<TypeContext *>(_localctx)->typeToken = match(EaSLParser::DECL_UID);
      break;
    }

    case 11: {
      enterOuterAlt(_localctx, 11);
      setState(138);
      antlrcpp::downCast<TypeContext *>(_localctx)->typeToken = match(EaSLParser::REFERENCE);
      setState(139);
      match(EaSLParser::REFERENCE_OPEN);
      setState(140);
      classType();
      setState(141);
      match(EaSLParser::REFERENCE_CLOSE);
      break;
    }

    case 12: {
      enterOuterAlt(_localctx, 12);
      setState(143);
      classType();
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

//----------------- ClassTypeContext ------------------------------------------------------------------

EaSLParser::ClassTypeContext::ClassTypeContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

std::vector<EaSLParser::IdentifierContext *> EaSLParser::ClassTypeContext::identifier() {
  return getRuleContexts<EaSLParser::IdentifierContext>();
}

EaSLParser::IdentifierContext* EaSLParser::ClassTypeContext::identifier(size_t i) {
  return getRuleContext<EaSLParser::IdentifierContext>(i);
}

tree::TerminalNode* EaSLParser::ClassTypeContext::NAMESPACE_DELIMITER() {
  return getToken(EaSLParser::NAMESPACE_DELIMITER, 0);
}


size_t EaSLParser::ClassTypeContext::getRuleIndex() const {
  return EaSLParser::RuleClassType;
}


EaSLParser::ClassTypeContext* EaSLParser::classType() {
  ClassTypeContext *_localctx = _tracker.createInstance<ClassTypeContext>(_ctx, getState());
  enterRule(_localctx, 20, EaSLParser::RuleClassType);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(149);
    _errHandler->sync(this);

    switch (getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 16, _ctx)) {
    case 1: {
      setState(146);
      antlrcpp::downCast<ClassTypeContext *>(_localctx)->namespaceIdentifier = identifier();
      setState(147);
      match(EaSLParser::NAMESPACE_DELIMITER);
      break;
    }

    default:
      break;
    }
    setState(151);
    antlrcpp::downCast<ClassTypeContext *>(_localctx)->classIdentifier = identifier();
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- IdentifierContext ------------------------------------------------------------------

EaSLParser::IdentifierContext::IdentifierContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* EaSLParser::IdentifierContext::DECL_CHAR() {
  return getToken(EaSLParser::DECL_CHAR, 0);
}

tree::TerminalNode* EaSLParser::IdentifierContext::DECL_INT32() {
  return getToken(EaSLParser::DECL_INT32, 0);
}

tree::TerminalNode* EaSLParser::IdentifierContext::DECL_INT64() {
  return getToken(EaSLParser::DECL_INT64, 0);
}

tree::TerminalNode* EaSLParser::IdentifierContext::DECL_UINT32() {
  return getToken(EaSLParser::DECL_UINT32, 0);
}

tree::TerminalNode* EaSLParser::IdentifierContext::DECL_UINT64() {
  return getToken(EaSLParser::DECL_UINT64, 0);
}

tree::TerminalNode* EaSLParser::IdentifierContext::DECL_FLOAT32() {
  return getToken(EaSLParser::DECL_FLOAT32, 0);
}

tree::TerminalNode* EaSLParser::IdentifierContext::DECL_FLOAT64() {
  return getToken(EaSLParser::DECL_FLOAT64, 0);
}

tree::TerminalNode* EaSLParser::IdentifierContext::DECL_BOOL() {
  return getToken(EaSLParser::DECL_BOOL, 0);
}

tree::TerminalNode* EaSLParser::IdentifierContext::DECL_STRING() {
  return getToken(EaSLParser::DECL_STRING, 0);
}

tree::TerminalNode* EaSLParser::IdentifierContext::DECL_UID() {
  return getToken(EaSLParser::DECL_UID, 0);
}

tree::TerminalNode* EaSLParser::IdentifierContext::REFERENCE() {
  return getToken(EaSLParser::REFERENCE, 0);
}

tree::TerminalNode* EaSLParser::IdentifierContext::NAMESPACE() {
  return getToken(EaSLParser::NAMESPACE, 0);
}

tree::TerminalNode* EaSLParser::IdentifierContext::CLASS() {
  return getToken(EaSLParser::CLASS, 0);
}

tree::TerminalNode* EaSLParser::IdentifierContext::ENUM() {
  return getToken(EaSLParser::ENUM, 0);
}

tree::TerminalNode* EaSLParser::IdentifierContext::LITERAL_NULL() {
  return getToken(EaSLParser::LITERAL_NULL, 0);
}

tree::TerminalNode* EaSLParser::IdentifierContext::LITERAL_TRUE() {
  return getToken(EaSLParser::LITERAL_TRUE, 0);
}

tree::TerminalNode* EaSLParser::IdentifierContext::LITERAL_FALSE() {
  return getToken(EaSLParser::LITERAL_FALSE, 0);
}

tree::TerminalNode* EaSLParser::IdentifierContext::IDENTIFIER() {
  return getToken(EaSLParser::IDENTIFIER, 0);
}


size_t EaSLParser::IdentifierContext::getRuleIndex() const {
  return EaSLParser::RuleIdentifier;
}


EaSLParser::IdentifierContext* EaSLParser::identifier() {
  IdentifierContext *_localctx = _tracker.createInstance<IdentifierContext>(_ctx, getState());
  enterRule(_localctx, 22, EaSLParser::RuleIdentifier);
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
    setState(153);
    _la = _input->LA(1);
    if (!((((_la & ~ 0x3fULL) == 0) &&
      ((1ULL << _la) & 19059036158) != 0))) {
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

void EaSLParser::initialize() {
#if ANTLR4_USE_THREAD_LOCAL_CACHE
  easlParserInitialize();
#else
  ::antlr4::internal::call_once(easlParserOnceFlag, easlParserInitialize);
#endif
}

cmake_minimum_required(VERSION 3.0)

# Project name
project(Compiler)

# Set C++ standard to 17
set(CMAKE_CXX_STANDARD 17)

# Include directories
include_directories("${CMAKE_SOURCE_DIR}/Headers")
include_directories("${CMAKE_SOURCE_DIR}/generated_c++")
include_directories("${CMAKE_SOURCE_DIR}/ANTLR4/runtime/Cpp/runtime/src")
include_directories("${CMAKE_SOURCE_DIR}/../Shared")
include_directories("${CMAKE_SOURCE_DIR}/../Shared/Headers")

# List of source files (modify the file names)
set(SOURCE_FILES
    ANTLR4/runtime/Cpp/runtime/src/Exceptions.cpp
    ANTLR4/runtime/Cpp/runtime/src/misc/InterpreterDataReader.cpp
    ANTLR4/runtime/Cpp/runtime/src/misc/IntervalSet.cpp
    ANTLR4/runtime/Cpp/runtime/src/misc/Predicate.cpp
    ANTLR4/runtime/Cpp/runtime/src/misc/MurmurHash.cpp
    ANTLR4/runtime/Cpp/runtime/src/misc/Interval.cpp
    ANTLR4/runtime/Cpp/runtime/src/tree/TerminalNodeImpl.cpp
    ANTLR4/runtime/Cpp/runtime/src/tree/xpath/XPathLexerErrorListener.cpp
    ANTLR4/runtime/Cpp/runtime/src/tree/xpath/XPathRuleElement.cpp
    ANTLR4/runtime/Cpp/runtime/src/tree/xpath/XPathElement.cpp
    ANTLR4/runtime/Cpp/runtime/src/tree/xpath/XPathWildcardAnywhereElement.cpp
    ANTLR4/runtime/Cpp/runtime/src/tree/xpath/XPathRuleAnywhereElement.cpp
    ANTLR4/runtime/Cpp/runtime/src/tree/xpath/XPathWildcardElement.cpp
    ANTLR4/runtime/Cpp/runtime/src/tree/xpath/XPathLexer.cpp
    ANTLR4/runtime/Cpp/runtime/src/tree/xpath/XPath.cpp
    ANTLR4/runtime/Cpp/runtime/src/tree/xpath/XPathTokenAnywhereElement.cpp
    ANTLR4/runtime/Cpp/runtime/src/tree/xpath/XPathTokenElement.cpp
    ANTLR4/runtime/Cpp/runtime/src/tree/Trees.cpp
    ANTLR4/runtime/Cpp/runtime/src/tree/ParseTreeListener.cpp
    ANTLR4/runtime/Cpp/runtime/src/tree/ParseTree.cpp
    ANTLR4/runtime/Cpp/runtime/src/tree/ParseTreeVisitor.cpp
    ANTLR4/runtime/Cpp/runtime/src/tree/ErrorNodeImpl.cpp
    ANTLR4/runtime/Cpp/runtime/src/tree/ParseTreeWalker.cpp
    ANTLR4/runtime/Cpp/runtime/src/tree/IterativeParseTreeWalker.cpp
    ANTLR4/runtime/Cpp/runtime/src/tree/pattern/RuleTagToken.cpp
    ANTLR4/runtime/Cpp/runtime/src/tree/pattern/ParseTreeMatch.cpp
    ANTLR4/runtime/Cpp/runtime/src/tree/pattern/TokenTagToken.cpp
    ANTLR4/runtime/Cpp/runtime/src/tree/pattern/Chunk.cpp
    ANTLR4/runtime/Cpp/runtime/src/tree/pattern/ParseTreePatternMatcher.cpp
    ANTLR4/runtime/Cpp/runtime/src/tree/pattern/TextChunk.cpp
    ANTLR4/runtime/Cpp/runtime/src/tree/pattern/TagChunk.cpp
    ANTLR4/runtime/Cpp/runtime/src/tree/pattern/ParseTreePattern.cpp
    ANTLR4/runtime/Cpp/runtime/src/DiagnosticErrorListener.cpp
    ANTLR4/runtime/Cpp/runtime/src/FailedPredicateException.cpp
    ANTLR4/runtime/Cpp/runtime/src/ConsoleErrorListener.cpp
    ANTLR4/runtime/Cpp/runtime/src/UnbufferedCharStream.cpp
    ANTLR4/runtime/Cpp/runtime/src/TokenStream.cpp
    ANTLR4/runtime/Cpp/runtime/src/UnbufferedTokenStream.cpp
    ANTLR4/runtime/Cpp/runtime/src/BaseErrorListener.cpp
    ANTLR4/runtime/Cpp/runtime/src/RuntimeMetaData.cpp
    ANTLR4/runtime/Cpp/runtime/src/InputMismatchException.cpp
    ANTLR4/runtime/Cpp/runtime/src/RuleContextWithAltNum.cpp
    ANTLR4/runtime/Cpp/runtime/src/dfa/DFAState.cpp
    ANTLR4/runtime/Cpp/runtime/src/dfa/DFASerializer.cpp
    ANTLR4/runtime/Cpp/runtime/src/dfa/DFA.cpp
    ANTLR4/runtime/Cpp/runtime/src/dfa/LexerDFASerializer.cpp
    ANTLR4/runtime/Cpp/runtime/src/Vocabulary.cpp
    ANTLR4/runtime/Cpp/runtime/src/ListTokenSource.cpp
    ANTLR4/runtime/Cpp/runtime/src/ANTLRErrorListener.cpp
    ANTLR4/runtime/Cpp/runtime/src/CommonTokenFactory.cpp
    ANTLR4/runtime/Cpp/runtime/src/TokenStreamRewriter.cpp
    ANTLR4/runtime/Cpp/runtime/src/CharStream.cpp
    ANTLR4/runtime/Cpp/runtime/src/ANTLRFileStream.cpp
    ANTLR4/runtime/Cpp/runtime/src/internal/Synchronization.cpp
    ANTLR4/runtime/Cpp/runtime/src/Lexer.cpp
    ANTLR4/runtime/Cpp/runtime/src/ProxyErrorListener.cpp
    ANTLR4/runtime/Cpp/runtime/src/ANTLRErrorStrategy.cpp
    ANTLR4/runtime/Cpp/runtime/src/RuleContext.cpp
    ANTLR4/runtime/Cpp/runtime/src/BufferedTokenStream.cpp
    ANTLR4/runtime/Cpp/runtime/src/LexerInterpreter.cpp
    ANTLR4/runtime/Cpp/runtime/src/Recognizer.cpp
    ANTLR4/runtime/Cpp/runtime/src/IntStream.cpp
    ANTLR4/runtime/Cpp/runtime/src/InterpreterRuleContext.cpp
    ANTLR4/runtime/Cpp/runtime/src/ParserInterpreter.cpp
    ANTLR4/runtime/Cpp/runtime/src/TokenSource.cpp
    ANTLR4/runtime/Cpp/runtime/src/support/CPPUtils.cpp
    ANTLR4/runtime/Cpp/runtime/src/support/Any.cpp
    ANTLR4/runtime/Cpp/runtime/src/support/StringUtils.cpp
    ANTLR4/runtime/Cpp/runtime/src/support/Arrays.cpp
    ANTLR4/runtime/Cpp/runtime/src/support/Utf8.cpp
    ANTLR4/runtime/Cpp/runtime/src/BailErrorStrategy.cpp
    ANTLR4/runtime/Cpp/runtime/src/CommonTokenStream.cpp
    ANTLR4/runtime/Cpp/runtime/src/atn/Transition.cpp
    ANTLR4/runtime/Cpp/runtime/src/atn/TransitionType.cpp
    ANTLR4/runtime/Cpp/runtime/src/atn/ParseInfo.cpp
    ANTLR4/runtime/Cpp/runtime/src/atn/ATNConfig.cpp
    ANTLR4/runtime/Cpp/runtime/src/atn/LexerTypeAction.cpp
    ANTLR4/runtime/Cpp/runtime/src/atn/AtomTransition.cpp
    ANTLR4/runtime/Cpp/runtime/src/atn/AmbiguityInfo.cpp
    ANTLR4/runtime/Cpp/runtime/src/atn/ATNSimulator.cpp
    ANTLR4/runtime/Cpp/runtime/src/atn/LexerIndexedCustomAction.cpp
    ANTLR4/runtime/Cpp/runtime/src/atn/RuleTransition.cpp
    ANTLR4/runtime/Cpp/runtime/src/atn/PredicateTransition.cpp
    ANTLR4/runtime/Cpp/runtime/src/atn/SingletonPredictionContext.cpp
    ANTLR4/runtime/Cpp/runtime/src/atn/LexerMoreAction.cpp
    ANTLR4/runtime/Cpp/runtime/src/atn/ATNDeserializationOptions.cpp
    ANTLR4/runtime/Cpp/runtime/src/atn/LexerActionExecutor.cpp
    ANTLR4/runtime/Cpp/runtime/src/atn/LexerATNSimulator.cpp
    ANTLR4/runtime/Cpp/runtime/src/atn/SetTransition.cpp
    ANTLR4/runtime/Cpp/runtime/src/atn/LexerModeAction.cpp
    ANTLR4/runtime/Cpp/runtime/src/atn/NotSetTransition.cpp
    ANTLR4/runtime/Cpp/runtime/src/atn/ProfilingATNSimulator.cpp
    ANTLR4/runtime/Cpp/runtime/src/atn/ATNStateType.cpp
    ANTLR4/runtime/Cpp/runtime/src/atn/LexerATNConfig.cpp
    ANTLR4/runtime/Cpp/runtime/src/atn/DecisionInfo.cpp
    ANTLR4/runtime/Cpp/runtime/src/atn/ATNState.cpp
    ANTLR4/runtime/Cpp/runtime/src/atn/ArrayPredictionContext.cpp
    ANTLR4/runtime/Cpp/runtime/src/atn/PredictionMode.cpp
    ANTLR4/runtime/Cpp/runtime/src/atn/LookaheadEventInfo.cpp
    ANTLR4/runtime/Cpp/runtime/src/atn/ATN.cpp
    ANTLR4/runtime/Cpp/runtime/src/atn/SemanticContext.cpp
    ANTLR4/runtime/Cpp/runtime/src/atn/OrderedATNConfigSet.cpp
    ANTLR4/runtime/Cpp/runtime/src/atn/RangeTransition.cpp
    ANTLR4/runtime/Cpp/runtime/src/atn/ATNDeserializer.cpp
    ANTLR4/runtime/Cpp/runtime/src/atn/WildcardTransition.cpp
    ANTLR4/runtime/Cpp/runtime/src/atn/ActionTransition.cpp
    ANTLR4/runtime/Cpp/runtime/src/atn/ParserATNSimulator.cpp
    ANTLR4/runtime/Cpp/runtime/src/atn/PredictionContextMergeCache.cpp
    ANTLR4/runtime/Cpp/runtime/src/atn/DecisionState.cpp
    ANTLR4/runtime/Cpp/runtime/src/atn/ErrorInfo.cpp
    ANTLR4/runtime/Cpp/runtime/src/atn/PredictionContextCache.cpp
    ANTLR4/runtime/Cpp/runtime/src/atn/StarLoopbackState.cpp
    ANTLR4/runtime/Cpp/runtime/src/atn/PredictionContext.cpp
    ANTLR4/runtime/Cpp/runtime/src/atn/LexerAction.cpp
    ANTLR4/runtime/Cpp/runtime/src/atn/LL1Analyzer.cpp
    ANTLR4/runtime/Cpp/runtime/src/atn/EpsilonTransition.cpp
    ANTLR4/runtime/Cpp/runtime/src/atn/LexerCustomAction.cpp
    ANTLR4/runtime/Cpp/runtime/src/atn/PrecedencePredicateTransition.cpp
    ANTLR4/runtime/Cpp/runtime/src/atn/LexerPopModeAction.cpp
    ANTLR4/runtime/Cpp/runtime/src/atn/PredicateEvalInfo.cpp
    ANTLR4/runtime/Cpp/runtime/src/atn/LexerSkipAction.cpp
    ANTLR4/runtime/Cpp/runtime/src/atn/ContextSensitivityInfo.cpp
    ANTLR4/runtime/Cpp/runtime/src/atn/ATNConfigSet.cpp
    ANTLR4/runtime/Cpp/runtime/src/atn/LexerChannelAction.cpp
    ANTLR4/runtime/Cpp/runtime/src/atn/DecisionEventInfo.cpp
    ANTLR4/runtime/Cpp/runtime/src/atn/LexerPushModeAction.cpp
    ANTLR4/runtime/Cpp/runtime/src/DefaultErrorStrategy.cpp
    ANTLR4/runtime/Cpp/runtime/src/CommonToken.cpp
    ANTLR4/runtime/Cpp/runtime/src/NoViableAltException.cpp
    ANTLR4/runtime/Cpp/runtime/src/ParserRuleContext.cpp
    ANTLR4/runtime/Cpp/runtime/src/RecognitionException.cpp
    ANTLR4/runtime/Cpp/runtime/src/Parser.cpp
    ANTLR4/runtime/Cpp/runtime/src/ANTLRInputStream.cpp
    ANTLR4/runtime/Cpp/runtime/src/Token.cpp
    ANTLR4/runtime/Cpp/runtime/src/LexerNoViableAltException.cpp
    ANTLR4/runtime/Cpp/runtime/src/WritableToken.cpp
    generated_c++/MaPLLexer.cpp
    generated_c++/MaPLParser.cpp
    MaPLCompiler/MaPLBuffer.cpp
    MaPLCompiler/MaPLAPI.cpp
    MaPLCompiler/MaPLVariableStack.cpp
    MaPLCompiler/MaPLFileCache.cpp
    MaPLCompiler/MaPLFile.cpp
    MaPLCompiler/MaPLCompilerHelpers.cpp
    MaPLCompiler/main.cpp
)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/Executable")

add_definitions(-DANTLR4CPP_STATIC)

# Create executable
add_executable(MaPLCompiler ${SOURCE_FILES})

# A bunch of linking logic copied from ANTLR.
find_package(Threads REQUIRED)
target_link_libraries(MaPLCompiler Threads::Threads)

if(CMAKE_CXX_COMPILER_ID MATCHES "MSVC")
  set(disabled_compile_warnings "/wd4251")
else()
  set(disabled_compile_warnings "-Wno-overloaded-virtual")
endif()

if(CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  set(disabled_compile_warnings "${disabled_compile_warnings} -Wno-dollar-in-identifier-extension -Wno-four-char-constants")
elseif(CMAKE_CXX_COMPILER_ID MATCHES "GNU" OR CMAKE_CXX_COMPILER_ID MATCHES "Intel")
  set(disabled_compile_warnings "${disabled_compile_warnings} -Wno-multichar")
endif()

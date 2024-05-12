cmake_minimum_required(VERSION 3.0)

# Project name
project(Generator)

# Set C++ standard to 17
set(CMAKE_CXX_STANDARD 17)

# Include ANTLR headers.
include_directories("${CMAKE_SOURCE_DIR}/../Compiler/ANTLR4/runtime/Cpp/runtime/src")
include_directories("${CMAKE_SOURCE_DIR}/../Compiler/ANTLR4/runtime/Cpp/runtime/src/atn")
include_directories("${CMAKE_SOURCE_DIR}/../Compiler/ANTLR4/runtime/Cpp/runtime/src/dfa")
include_directories("${CMAKE_SOURCE_DIR}/../Compiler/ANTLR4/runtime/Cpp/runtime/src/internal")
include_directories("${CMAKE_SOURCE_DIR}/../Compiler/ANTLR4/runtime/Cpp/runtime/src/misc")
include_directories("${CMAKE_SOURCE_DIR}/../Compiler/ANTLR4/runtime/Cpp/runtime/src/support")
include_directories("${CMAKE_SOURCE_DIR}/../Compiler/ANTLR4/runtime/Cpp/runtime/src/tree")
include_directories("${CMAKE_SOURCE_DIR}/../Compiler/ANTLR4/runtime/Cpp/runtime/src/tree/pattern")
include_directories("${CMAKE_SOURCE_DIR}/../Compiler/ANTLR4/runtime/Cpp/runtime/src/tree/xpath")

# Include all other headers.
include_directories("${CMAKE_SOURCE_DIR}/../Compiler/Headers")
include_directories("${CMAKE_SOURCE_DIR}/../Compiler/generated_c++")
include_directories("${CMAKE_SOURCE_DIR}/../Runtime/Headers")
include_directories("${CMAKE_SOURCE_DIR}/../Shared")
include_directories("${CMAKE_SOURCE_DIR}/../Shared/Headers")
include_directories("${CMAKE_SOURCE_DIR}/EaSL/generated_c++")
include_directories("${CMAKE_SOURCE_DIR}/tinyxml2")

# List of source files (modify the file names)
set(SOURCE_FILES
    ../Compiler/ANTLR4/runtime/Cpp/runtime/src/Exceptions.cpp
    ../Compiler/ANTLR4/runtime/Cpp/runtime/src/misc/InterpreterDataReader.cpp
    ../Compiler/ANTLR4/runtime/Cpp/runtime/src/misc/IntervalSet.cpp
    ../Compiler/ANTLR4/runtime/Cpp/runtime/src/misc/Predicate.cpp
    ../Compiler/ANTLR4/runtime/Cpp/runtime/src/misc/MurmurHash.cpp
    ../Compiler/ANTLR4/runtime/Cpp/runtime/src/misc/Interval.cpp
    ../Compiler/ANTLR4/runtime/Cpp/runtime/src/tree/TerminalNodeImpl.cpp
    ../Compiler/ANTLR4/runtime/Cpp/runtime/src/tree/xpath/XPathLexerErrorListener.cpp
    ../Compiler/ANTLR4/runtime/Cpp/runtime/src/tree/xpath/XPathRuleElement.cpp
    ../Compiler/ANTLR4/runtime/Cpp/runtime/src/tree/xpath/XPathElement.cpp
    ../Compiler/ANTLR4/runtime/Cpp/runtime/src/tree/xpath/XPathWildcardAnywhereElement.cpp
    ../Compiler/ANTLR4/runtime/Cpp/runtime/src/tree/xpath/XPathRuleAnywhereElement.cpp
    ../Compiler/ANTLR4/runtime/Cpp/runtime/src/tree/xpath/XPathWildcardElement.cpp
    ../Compiler/ANTLR4/runtime/Cpp/runtime/src/tree/xpath/XPathLexer.cpp
    ../Compiler/ANTLR4/runtime/Cpp/runtime/src/tree/xpath/XPath.cpp
    ../Compiler/ANTLR4/runtime/Cpp/runtime/src/tree/xpath/XPathTokenAnywhereElement.cpp
    ../Compiler/ANTLR4/runtime/Cpp/runtime/src/tree/xpath/XPathTokenElement.cpp
    ../Compiler/ANTLR4/runtime/Cpp/runtime/src/tree/Trees.cpp
    ../Compiler/ANTLR4/runtime/Cpp/runtime/src/tree/ParseTreeListener.cpp
    ../Compiler/ANTLR4/runtime/Cpp/runtime/src/tree/ParseTree.cpp
    ../Compiler/ANTLR4/runtime/Cpp/runtime/src/tree/ParseTreeVisitor.cpp
    ../Compiler/ANTLR4/runtime/Cpp/runtime/src/tree/ErrorNodeImpl.cpp
    ../Compiler/ANTLR4/runtime/Cpp/runtime/src/tree/ParseTreeWalker.cpp
    ../Compiler/ANTLR4/runtime/Cpp/runtime/src/tree/IterativeParseTreeWalker.cpp
    ../Compiler/ANTLR4/runtime/Cpp/runtime/src/tree/pattern/RuleTagToken.cpp
    ../Compiler/ANTLR4/runtime/Cpp/runtime/src/tree/pattern/ParseTreeMatch.cpp
    ../Compiler/ANTLR4/runtime/Cpp/runtime/src/tree/pattern/TokenTagToken.cpp
    ../Compiler/ANTLR4/runtime/Cpp/runtime/src/tree/pattern/Chunk.cpp
    ../Compiler/ANTLR4/runtime/Cpp/runtime/src/tree/pattern/ParseTreePatternMatcher.cpp
    ../Compiler/ANTLR4/runtime/Cpp/runtime/src/tree/pattern/TextChunk.cpp
    ../Compiler/ANTLR4/runtime/Cpp/runtime/src/tree/pattern/TagChunk.cpp
    ../Compiler/ANTLR4/runtime/Cpp/runtime/src/tree/pattern/ParseTreePattern.cpp
    ../Compiler/ANTLR4/runtime/Cpp/runtime/src/DiagnosticErrorListener.cpp
    ../Compiler/ANTLR4/runtime/Cpp/runtime/src/FailedPredicateException.cpp
    ../Compiler/ANTLR4/runtime/Cpp/runtime/src/ConsoleErrorListener.cpp
    ../Compiler/ANTLR4/runtime/Cpp/runtime/src/UnbufferedCharStream.cpp
    ../Compiler/ANTLR4/runtime/Cpp/runtime/src/TokenStream.cpp
    ../Compiler/ANTLR4/runtime/Cpp/runtime/src/UnbufferedTokenStream.cpp
    ../Compiler/ANTLR4/runtime/Cpp/runtime/src/BaseErrorListener.cpp
    ../Compiler/ANTLR4/runtime/Cpp/runtime/src/RuntimeMetaData.cpp
    ../Compiler/ANTLR4/runtime/Cpp/runtime/src/InputMismatchException.cpp
    ../Compiler/ANTLR4/runtime/Cpp/runtime/src/RuleContextWithAltNum.cpp
    ../Compiler/ANTLR4/runtime/Cpp/runtime/src/dfa/DFAState.cpp
    ../Compiler/ANTLR4/runtime/Cpp/runtime/src/dfa/DFASerializer.cpp
    ../Compiler/ANTLR4/runtime/Cpp/runtime/src/dfa/DFA.cpp
    ../Compiler/ANTLR4/runtime/Cpp/runtime/src/dfa/LexerDFASerializer.cpp
    ../Compiler/ANTLR4/runtime/Cpp/runtime/src/Vocabulary.cpp
    ../Compiler/ANTLR4/runtime/Cpp/runtime/src/ListTokenSource.cpp
    ../Compiler/ANTLR4/runtime/Cpp/runtime/src/ANTLRErrorListener.cpp
    ../Compiler/ANTLR4/runtime/Cpp/runtime/src/CommonTokenFactory.cpp
    ../Compiler/ANTLR4/runtime/Cpp/runtime/src/TokenStreamRewriter.cpp
    ../Compiler/ANTLR4/runtime/Cpp/runtime/src/CharStream.cpp
    ../Compiler/ANTLR4/runtime/Cpp/runtime/src/ANTLRFileStream.cpp
    ../Compiler/ANTLR4/runtime/Cpp/runtime/src/internal/Synchronization.cpp
    ../Compiler/ANTLR4/runtime/Cpp/runtime/src/Lexer.cpp
    ../Compiler/ANTLR4/runtime/Cpp/runtime/src/ProxyErrorListener.cpp
    ../Compiler/ANTLR4/runtime/Cpp/runtime/src/ANTLRErrorStrategy.cpp
    ../Compiler/ANTLR4/runtime/Cpp/runtime/src/RuleContext.cpp
    ../Compiler/ANTLR4/runtime/Cpp/runtime/src/BufferedTokenStream.cpp
    ../Compiler/ANTLR4/runtime/Cpp/runtime/src/LexerInterpreter.cpp
    ../Compiler/ANTLR4/runtime/Cpp/runtime/src/Recognizer.cpp
    ../Compiler/ANTLR4/runtime/Cpp/runtime/src/IntStream.cpp
    ../Compiler/ANTLR4/runtime/Cpp/runtime/src/InterpreterRuleContext.cpp
    ../Compiler/ANTLR4/runtime/Cpp/runtime/src/ParserInterpreter.cpp
    ../Compiler/ANTLR4/runtime/Cpp/runtime/src/TokenSource.cpp
    ../Compiler/ANTLR4/runtime/Cpp/runtime/src/support/CPPUtils.cpp
    ../Compiler/ANTLR4/runtime/Cpp/runtime/src/support/Any.cpp
    ../Compiler/ANTLR4/runtime/Cpp/runtime/src/support/StringUtils.cpp
    ../Compiler/ANTLR4/runtime/Cpp/runtime/src/support/Arrays.cpp
    ../Compiler/ANTLR4/runtime/Cpp/runtime/src/support/Utf8.cpp
    ../Compiler/ANTLR4/runtime/Cpp/runtime/src/BailErrorStrategy.cpp
    ../Compiler/ANTLR4/runtime/Cpp/runtime/src/CommonTokenStream.cpp
    ../Compiler/ANTLR4/runtime/Cpp/runtime/src/atn/Transition.cpp
    ../Compiler/ANTLR4/runtime/Cpp/runtime/src/atn/TransitionType.cpp
    ../Compiler/ANTLR4/runtime/Cpp/runtime/src/atn/ParseInfo.cpp
    ../Compiler/ANTLR4/runtime/Cpp/runtime/src/atn/ATNConfig.cpp
    ../Compiler/ANTLR4/runtime/Cpp/runtime/src/atn/LexerTypeAction.cpp
    ../Compiler/ANTLR4/runtime/Cpp/runtime/src/atn/AtomTransition.cpp
    ../Compiler/ANTLR4/runtime/Cpp/runtime/src/atn/AmbiguityInfo.cpp
    ../Compiler/ANTLR4/runtime/Cpp/runtime/src/atn/ATNSimulator.cpp
    ../Compiler/ANTLR4/runtime/Cpp/runtime/src/atn/LexerIndexedCustomAction.cpp
    ../Compiler/ANTLR4/runtime/Cpp/runtime/src/atn/RuleTransition.cpp
    ../Compiler/ANTLR4/runtime/Cpp/runtime/src/atn/PredicateTransition.cpp
    ../Compiler/ANTLR4/runtime/Cpp/runtime/src/atn/SingletonPredictionContext.cpp
    ../Compiler/ANTLR4/runtime/Cpp/runtime/src/atn/LexerMoreAction.cpp
    ../Compiler/ANTLR4/runtime/Cpp/runtime/src/atn/ATNDeserializationOptions.cpp
    ../Compiler/ANTLR4/runtime/Cpp/runtime/src/atn/LexerActionExecutor.cpp
    ../Compiler/ANTLR4/runtime/Cpp/runtime/src/atn/LexerATNSimulator.cpp
    ../Compiler/ANTLR4/runtime/Cpp/runtime/src/atn/SetTransition.cpp
    ../Compiler/ANTLR4/runtime/Cpp/runtime/src/atn/LexerModeAction.cpp
    ../Compiler/ANTLR4/runtime/Cpp/runtime/src/atn/NotSetTransition.cpp
    ../Compiler/ANTLR4/runtime/Cpp/runtime/src/atn/ProfilingATNSimulator.cpp
    ../Compiler/ANTLR4/runtime/Cpp/runtime/src/atn/ATNStateType.cpp
    ../Compiler/ANTLR4/runtime/Cpp/runtime/src/atn/LexerATNConfig.cpp
    ../Compiler/ANTLR4/runtime/Cpp/runtime/src/atn/DecisionInfo.cpp
    ../Compiler/ANTLR4/runtime/Cpp/runtime/src/atn/ATNState.cpp
    ../Compiler/ANTLR4/runtime/Cpp/runtime/src/atn/ArrayPredictionContext.cpp
    ../Compiler/ANTLR4/runtime/Cpp/runtime/src/atn/PredictionMode.cpp
    ../Compiler/ANTLR4/runtime/Cpp/runtime/src/atn/LookaheadEventInfo.cpp
    ../Compiler/ANTLR4/runtime/Cpp/runtime/src/atn/ATN.cpp
    ../Compiler/ANTLR4/runtime/Cpp/runtime/src/atn/SemanticContext.cpp
    ../Compiler/ANTLR4/runtime/Cpp/runtime/src/atn/OrderedATNConfigSet.cpp
    ../Compiler/ANTLR4/runtime/Cpp/runtime/src/atn/RangeTransition.cpp
    ../Compiler/ANTLR4/runtime/Cpp/runtime/src/atn/ATNDeserializer.cpp
    ../Compiler/ANTLR4/runtime/Cpp/runtime/src/atn/WildcardTransition.cpp
    ../Compiler/ANTLR4/runtime/Cpp/runtime/src/atn/ActionTransition.cpp
    ../Compiler/ANTLR4/runtime/Cpp/runtime/src/atn/ParserATNSimulator.cpp
    ../Compiler/ANTLR4/runtime/Cpp/runtime/src/atn/PredictionContextMergeCache.cpp
    ../Compiler/ANTLR4/runtime/Cpp/runtime/src/atn/DecisionState.cpp
    ../Compiler/ANTLR4/runtime/Cpp/runtime/src/atn/ErrorInfo.cpp
    ../Compiler/ANTLR4/runtime/Cpp/runtime/src/atn/PredictionContextCache.cpp
    ../Compiler/ANTLR4/runtime/Cpp/runtime/src/atn/StarLoopbackState.cpp
    ../Compiler/ANTLR4/runtime/Cpp/runtime/src/atn/PredictionContext.cpp
    ../Compiler/ANTLR4/runtime/Cpp/runtime/src/atn/LexerAction.cpp
    ../Compiler/ANTLR4/runtime/Cpp/runtime/src/atn/LL1Analyzer.cpp
    ../Compiler/ANTLR4/runtime/Cpp/runtime/src/atn/EpsilonTransition.cpp
    ../Compiler/ANTLR4/runtime/Cpp/runtime/src/atn/LexerCustomAction.cpp
    ../Compiler/ANTLR4/runtime/Cpp/runtime/src/atn/PrecedencePredicateTransition.cpp
    ../Compiler/ANTLR4/runtime/Cpp/runtime/src/atn/LexerPopModeAction.cpp
    ../Compiler/ANTLR4/runtime/Cpp/runtime/src/atn/PredicateEvalInfo.cpp
    ../Compiler/ANTLR4/runtime/Cpp/runtime/src/atn/LexerSkipAction.cpp
    ../Compiler/ANTLR4/runtime/Cpp/runtime/src/atn/ContextSensitivityInfo.cpp
    ../Compiler/ANTLR4/runtime/Cpp/runtime/src/atn/ATNConfigSet.cpp
    ../Compiler/ANTLR4/runtime/Cpp/runtime/src/atn/LexerChannelAction.cpp
    ../Compiler/ANTLR4/runtime/Cpp/runtime/src/atn/DecisionEventInfo.cpp
    ../Compiler/ANTLR4/runtime/Cpp/runtime/src/atn/LexerPushModeAction.cpp
    ../Compiler/ANTLR4/runtime/Cpp/runtime/src/DefaultErrorStrategy.cpp
    ../Compiler/ANTLR4/runtime/Cpp/runtime/src/CommonToken.cpp
    ../Compiler/ANTLR4/runtime/Cpp/runtime/src/NoViableAltException.cpp
    ../Compiler/ANTLR4/runtime/Cpp/runtime/src/ParserRuleContext.cpp
    ../Compiler/ANTLR4/runtime/Cpp/runtime/src/RecognitionException.cpp
    ../Compiler/ANTLR4/runtime/Cpp/runtime/src/Parser.cpp
    ../Compiler/ANTLR4/runtime/Cpp/runtime/src/ANTLRInputStream.cpp
    ../Compiler/ANTLR4/runtime/Cpp/runtime/src/Token.cpp
    ../Compiler/ANTLR4/runtime/Cpp/runtime/src/LexerNoViableAltException.cpp
    ../Compiler/ANTLR4/runtime/Cpp/runtime/src/WritableToken.cpp
    ../Compiler/generated_c++/MaPLLexer.cpp
    ../Compiler/generated_c++/MaPLParser.cpp
    ../Compiler/MaPLCompiler/MaPLBuffer.cpp
    ../Compiler/MaPLCompiler/MaPLAPI.cpp
    ../Compiler/MaPLCompiler/MaPLVariableStack.cpp
    ../Compiler/MaPLCompiler/MaPLFileCache.cpp
    ../Compiler/MaPLCompiler/MaPLFile.cpp
    ../Compiler/MaPLCompiler/MaPLCompilerHelpers.cpp
    ../Runtime/MaPLRuntime/MaPLRuntime.c
    tinyxml2/tinyxml2.cpp
    EaSL/generated_c++/EaSLLexer.cpp
    EaSL/generated_c++/EaSLParser.cpp
    MaPLGenerator/EaSLHandler.cpp
    MaPLGenerator/ErrorLogger.cpp
    MaPLGenerator/MaPLHandler.cpp
    MaPLGenerator/XmlHandler.cpp
    MaPLGenerator/main.cpp
)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/Executable")

add_definitions(-DANTLR4CPP_STATIC)

# Create executable
add_executable(MaPLGenerator ${SOURCE_FILES})

# A bunch of linking logic copied from ANTLR.
find_package(Threads REQUIRED)
target_link_libraries(MaPLGenerator Threads::Threads)

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

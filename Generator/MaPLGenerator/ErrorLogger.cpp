//
//  ErrorLogger.cpp
//  MaPLGenerator
//
//  Created by Brad Bambara on 5/25/22.
//

#include "ErrorLogger.h"
#include "antlr4-runtime.h"

ErrorLogger::ErrorLogger(const std::filesystem::path &filePath) :
_filePath(filePath),
_hasLoggedError(false) {
}

void ErrorLogger::logError(antlr4::Token *token, const std::string &errorMessage) {
    fprintf(stderr, "%s:%lu:%lu: error: %s\n", _filePath.c_str(), token->getLine(), token->getCharPositionInLine(), errorMessage.c_str());
    _hasLoggedError = true;
}

void ErrorLogger::logError(xmlNode *node, const std::string &errorMessage) {
    fprintf(stderr, "%s:%u: error: %s\n", _filePath.c_str(), node->line, errorMessage.c_str());
    _hasLoggedError = true;
}

void ErrorLogger::logError(const std::string &errorMessage) {
    fprintf(stderr, "%s:1: error: %s\n", _filePath.c_str(), errorMessage.c_str());
    _hasLoggedError = true;
}

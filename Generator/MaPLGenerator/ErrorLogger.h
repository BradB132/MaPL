//
//  ErrorLogger.h
//  MaPLGenerator
//
//  Created by Brad Bambara on 5/25/22.
//

#ifndef ErrorLogger_h
#define ErrorLogger_h

#include <filesystem>
#include <string>

#include "antlr4-common.h"
#include "tinyxml2.h"

class ErrorLogger {
public:
    ErrorLogger(const std::filesystem::path &filePath);
    void logError(antlr4::Token *token, const std::string &errorMessage);
    void logError(const tinyxml2::XMLElement *node, const std::string &errorMessage);
    void logError(const std::string &errorMessage);

    const std::filesystem::path _filePath;
    bool _hasLoggedError;
};

#endif /* ErrorLogger_h */

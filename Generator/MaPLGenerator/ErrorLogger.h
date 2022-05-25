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
#include <libxml/tree.h>
#include "antlr4-common.h"

class ErrorLogger {
public:
    ErrorLogger(const std::filesystem::path &filePath);
    void logError(antlr4::Token *token, const std::string &errorMessage);
    void logError(xmlNode *node, const std::string &errorMessage);
    void logError(const std::string &errorMessage);

    const std::filesystem::path _filePath;
    bool _hasLoggedError;
};

#endif /* ErrorLogger_h */

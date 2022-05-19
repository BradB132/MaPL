//
//  MaPLHandler.h
//  MaPLGenerator
//
//  Created by Brad Bambara on 5/17/22.
//

#ifndef MaPLHandler_h
#define MaPLHandler_h

#include <filesystem>

#include "MaPLRuntime.h"
#include "EaSLParser.h"

struct MaPLGeneratorContext {
    std::vector<EaSLParser::SchemaContext *> schemas;
    // TODO: XML files.
    std::vector<std::pair<std::string, std::string>> flags;
};

void invokeScript(const std::filesystem::path &scriptPath, const MaPLGeneratorContext &context);

#endif /* MaPLHandler_h */

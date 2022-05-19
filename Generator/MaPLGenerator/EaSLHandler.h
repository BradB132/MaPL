//
//  EaSLHandler.h
//  MaPLGenerator
//
//  Created by Brad Bambara on 5/19/22.
//

#ifndef EaSLHandler_h
#define EaSLHandler_h

#include <filesystem>
#include <vector>

#include "EaSLParser.h"

std::vector<EaSLParser::SchemaContext *> schemasForPaths(const std::vector<std::filesystem::path> &schemaPaths);

#endif /* EaSLHandler_h */

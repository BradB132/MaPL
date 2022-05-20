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
#include "EaSLHandler.h"
#include "MaPLGeneratorCollections.h"

void invokeScript(const std::filesystem::path &scriptPath);

#endif /* MaPLHandler_h */

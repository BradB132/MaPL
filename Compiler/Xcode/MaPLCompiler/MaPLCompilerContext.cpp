//
//  MaPLCompilerContext.cpp
//  MaPLCompiler
//
//  Created by Brad Bambara on 2/1/22.
//

#include "MaPLCompilerContext.h"
#include "MaPLFile.h"

MaPLFile *MaPLCompilerContext::fileForAbsolutePath(std::filesystem::path absoluteFilePath) {
    // Verify that the path is normalized.
    if (!absoluteFilePath.is_absolute()) {
        return NULL;
    }
    std::filesystem::path normalizedPath = absoluteFilePath.lexically_normal();
    
    // Attempt first to fetch the file from cache.
    MaPLFile *fileAtPath = files[normalizedPath.string()];
    if (!fileAtPath) {
        // No matching file found in cache, add a new one.
        fileAtPath = new MaPLFile(normalizedPath, this);
        files[normalizedPath.string()] = fileAtPath;
    }
    return fileAtPath;
}

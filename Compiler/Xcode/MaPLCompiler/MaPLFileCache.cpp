//
//  MaPLFileCache.cpp
//  MaPLCompiler
//
//  Created by Brad Bambara on 2/1/22.
//

#include "MaPLFileCache.h"
#include "MaPLFile.h"

MaPLFile *MaPLFileCache::fileForNormalizedPath(const std::filesystem::path &normalizedFilePath) {
    // Attempt first to fetch the file from cache.
    MaPLFile *fileAtPath = files[normalizedFilePath.string()];
    if (!fileAtPath) {
        // No matching file found in cache, add a new one.
        fileAtPath = new MaPLFile(normalizedFilePath, this);
        files[normalizedFilePath.string()] = fileAtPath;
    }
    return fileAtPath;
}

MaPLFileCache::~MaPLFileCache() {
    for(const auto&[path, file] : files) {
        delete file;
    }
}

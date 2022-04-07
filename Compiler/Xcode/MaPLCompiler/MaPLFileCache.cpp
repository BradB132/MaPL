//
//  MaPLFileCache.cpp
//  MaPLCompiler
//
//  Created by Brad Bambara on 2/1/22.
//

#include "MaPLFileCache.h"
#include "MaPLFile.h"

MaPLFileCache::~MaPLFileCache() {
    for(const auto&[path, file] : _files) {
        delete file;
    }
}

MaPLFile *MaPLFileCache::fileForNormalizedPath(const std::filesystem::path &normalizedFilePath) {
    // Attempt first to fetch the file from cache.
    std::string pathString = normalizedFilePath.string();
    MaPLFile *fileAtPath = _files[pathString];
    if (!fileAtPath) {
        // No matching file found in cache, add a new one.
        fileAtPath = new MaPLFile(normalizedFilePath, this);
        _files[pathString] = fileAtPath;
    }
    return fileAtPath;
}

std::unordered_map<std::string, MaPLFile *> MaPLFileCache::getFiles() {
    return _files;
}

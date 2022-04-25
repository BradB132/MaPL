//
//  MaPLFileCache.h
//  MaPLCompiler
//
//  Created by Brad Bambara on 2/1/22.
//

#ifndef MaPLFileCache_h
#define MaPLFileCache_h

#include <stdio.h>
#include <filesystem>
#include <unordered_map>

class MaPLFile;

/**
 * An object that caches the state of the compiler as it traverses the dependency graph of multiple script files.
 */
class  MaPLFileCache {
public:
    
    ~MaPLFileCache();
    
    /**
     * @param normalizedFilePath The lexically normalized filesystem path to the MaPL script file.
     *
     * @return The @c MaPLFile for the script at the given @c normalizedFilePath.
     */
    MaPLFile *fileForNormalizedPath(const std::filesystem::path &normalizedFilePath);
    
    std::unordered_map<std::string, MaPLFile *> getFiles();
    
private:
    
    std::unordered_map<std::string, MaPLFile *> _files;
};

#endif /* MaPLFileCache_h */

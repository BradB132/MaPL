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
    
    /**
     * @param absoluteFilePath The absolute filesystem path to the MaPL script file.
     *
     * @return The @c MaPLFile for the script at the given @c absoluteFilePath. @c NULL if error.
     */
    MaPLFile *fileForAbsolutePath(const std::filesystem::path &absoluteFilePath);
    
private:
    
    std::unordered_map<std::string, MaPLFile *> files;
};

#endif /* MaPLFileCache_h */

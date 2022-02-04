//
//  MaPLCompilerContext.h
//  MaPLCompiler
//
//  Created by Brad Bambara on 2/1/22.
//

#ifndef MaPLCompilerContext_h
#define MaPLCompilerContext_h

#include <stdio.h>
#include <filesystem>
#include <unordered_map>

class MaPLFile;

/**
 * An object that caches the state of the compiler as it traverses the dependency graph of multiple script files.
 */
class  MaPLCompilerContext {
public:
    
    /**
     * @param absoluteFilePath The absolute filesystem path to the MaPL script file.
     *
     * @return The `MaPLFile` for the script at the given `absoluteFilePath`. NULL if error.
     */
    MaPLFile *fileForAbsolutePath(std::filesystem::path absoluteFilePath);
    
private:
    
    std::unordered_map<std::string, MaPLFile *> files;
};

#endif /* MaPLCompilerContext_h */

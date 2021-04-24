//
//  MaPLFile.hpp
//  MaPLCompiler
//
//  Created by Brad Bambara on 3/27/21.
//

#ifndef MaPLFile_hpp
#define MaPLFile_hpp

#include <stdio.h>
#include <string>
#include <fstream>
#include <sstream>
#include <filesystem>

class  MaPLFile {
public:
    
    explicit MaPLFile(std::__fs::filesystem::path scriptFilePath);
    
    /**
     * Reads the human-readabile (ie, non-compiled) script text from the file.
     *
     * @return `true` only when the raw script was read successfully.
     */
    bool readRawScriptFromDisk();
    
    /**
     * @return A file path equivalent to the one which was passed in to the initializer. This file path is normalized so that it's more easily comparable to other paths.
     */
    std::__fs::filesystem::path getNormalizedFilePath();
    
    /**
     * @return A file path equivalent to the one which was passed in to the initializer. This file path is normalized so that it's more easily comparable to other paths.
     */
    std::__fs::filesystem::path getNormalizedParentDirectory();
    
    /**
     * This string will be empty until the raw text is loaded via the `readRawScriptFromDisk` method.
     *
     * @return The human-readabile (ie, non-compiled) script text from the file.
     */
    std::string getRawScriptText();
    
    // TODO: lex and parse within this class?
    
private:
    
    std::__fs::filesystem::path _normalizedFilePath;
    std::string _rawScriptText;
};

#endif /* MaPLFile_hpp */

//
//  main.cpp
//  MaPLCompiler
//
//  Created by Brad Bambara on 3/27/21.
//

#include <stdio.h>
#include "MaPLFileCache.h"
#include "MaPLFile.h"
#include "MaPLBuffer.h"

int main(int argc, const char ** argv) {
    // Make sure that script paths were specified.
    if (argc < 2) {
        // TODO: Make this error message a more verbose description of what usage should look like.
        printf("No paths specified. Pass in the paths of the script(s) as arguments.\n");
        return 1;
    }
    
    // TODO: Add an abstraction that can take options from the command line and pass them through to the compilation logic. We need debug on/off and output paths for each file.
    
    // Parse all script files specified in the args.
    MaPLFileCache fileCache;
    for(int i = 1; i < argc; i++) {
        MaPLFile *file = fileCache.fileForAbsolutePath(std::filesystem::absolute(argv[i]));
        MaPLBuffer *bytecode = file->getBytecode();
        // TODO: Write bytecode to an appropriate output path.
        if (bytecode) {
            printf("bytecode length: %d\n", (int)bytecode->getByteCount());// TODO: this line just for testing, delete later.
        }
    }
    // TODO: Resolve function symbols in each bytecode buffer.
    
    return 0;
}

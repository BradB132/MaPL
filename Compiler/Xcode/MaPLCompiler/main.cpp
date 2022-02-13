//
//  main.cpp
//  MaPLCompiler
//
//  Created by Brad Bambara on 3/27/21.
//

#include <stdio.h>
#include "MaPLCompilerContext.h"
#include "MaPLFile.h"
#include "MaPLBuffer.h"

int main(int argc, const char ** argv) {
    // Make sure that script paths were specified.
    if (argc < 2) {
        printf("No paths specified. Pass in the paths of the script(s) as arguments.\n");
        return 1;
    }
    
    // Parse all script files specified in the args.
    MaPLCompilerContext context;
    for(int i = 1; i < argc; i++) {
        MaPLFile *file = context.fileForAbsolutePath(std::filesystem::absolute(argv[i]));
        MaPLBuffer *bytecode = file->getBytecode();
        //TODO: write bytecode to an appropriate output path.
        if (bytecode) {
            printf("bytecode length: %d\n", (int)bytecode->getByteCount());// TODO: this line just for testing, delete later.
        }
    }
    
    return 0;
}

//
//  main.c
//  MaPLRuntime
//
//  Created by Brad Bambara on 4/5/22.
//

#include <stdio.h>
#include "MaPLRuntime.h"

int main(int argc, const char * argv[]) {
    
    // TODO: Read in script paths from args and execute them.
    MaPLCallbacks callbacks;
    executeMaPLScript(NULL, 0, &callbacks);
    
    return 0;
}

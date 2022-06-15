//
//  MaPLDecompiler.h
//  MaPLTester
//
//  Created by Brad Bambara on 6/14/22.
//

#ifndef MaPLDecompiler_h
#define MaPLDecompiler_h

#include <stdio.h>
#include "MaPLTypedefs.h"

void printDecompilationOfBytecode(const u_int8_t *bytes, MaPLBytecodeLength length);

#endif /* MaPLDecompiler_h */

//
//  MaPLInterface.h
//  MaPLGenerator
//
//  Created by Brad Bambara on 5/19/22.
//

#ifndef MaPLInterface_h
#define MaPLInterface_h

#include "MaPLRuntime.h"

/**
 * Abstract class that can be used for any MaPL API.
 */
class MaPLInterface {
public:
    virtual MaPLParameter invokeFunction(MaPLSymbol functionSymbol, const MaPLParameter *argv, MaPLParameterCount argc) = 0;
    virtual MaPLParameter invokeSubscript(MaPLParameter index) = 0;
};

#endif /* MaPLInterface_h */

//
//  MaPLGeneratorCollections.h
//  MaPLGenerator
//
//  Created by Brad Bambara on 5/19/22.
//

#ifndef MaPLGeneratorCollections_h
#define MaPLGeneratorCollections_h

#include "MaPLInterface.h"

#include <vector>
#include <unordered_map>

#include "MaPLSymbols.h"
#include "MaPLRuntime.h"

template <typename T>
class MaPLArray : public MaPLInterface {
public:
    MaPLArray(std::vector<T> &backingVector) : _backingVector(backingVector) {
    }
    
    virtual MaPLParameter invokeFunction(MaPLSymbol functionSymbol, const MaPLParameter *argv, MaPLParameterCount argc) {
        if (functionSymbol == MaPLSymbols_Array_count) {
            return MaPLUint32((uint32_t)_backingVector.size());
        }
        return MaPLUninitialized();
    }
    
    virtual MaPLParameter invokeSubscript(MaPLParameter index) {
        return parameterForTemplate(_backingVector[index.uint32Value]);
    }
    
    static MaPLParameter parameterForTemplate(const T &value) {
        if constexpr (std::is_same_v<T, std::string>) {
            return MaPLStringByValue(value.c_str());
        }
        if constexpr (std::is_pointer_v<T>) {
            return MaPLPointer(value);
        }
        return MaPLUninitialized();
    }
private:
    std::vector<T> _backingVector;
};

template <typename T>
class MaPLArrayMap : public MaPLArray<T> {
public:
    MaPLArrayMap(std::vector<T> &backingVector, std::unordered_map<std::string, T> &backingMap)
        : MaPLArray<T>(backingVector), _backingMap(backingMap) {
    }
    
    virtual MaPLParameter invokeFunction(MaPLSymbol functionSymbol, const MaPLParameter *argv, MaPLParameterCount argc) {
        if (functionSymbol == MaPLSymbols_ArrayMap_contains_string) {
            return MaPLBool(_backingMap.count(argv[0].stringValue) != 0);
        }
        return MaPLArray<T>::invokeFunction(functionSymbol, argv, argc);
    }
    
    virtual MaPLParameter invokeSubscript(MaPLParameter index) {
        if (index.dataType == MaPLDataType_string) {
            return MaPLArray<T>::parameterForTemplate(_backingMap[index.stringValue]);
        }
        return MaPLArray<T>::invokeSubscript(index);
    }
private:
    std::unordered_map<std::string, T> _backingMap;
};

#endif /* MaPLGeneratorCollections_h */

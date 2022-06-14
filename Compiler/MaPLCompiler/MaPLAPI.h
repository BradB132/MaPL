//
//  MaPLAPI.h
//  MaPLCompiler
//
//  Created by Brad Bambara on 5/3/22.
//

#ifndef MaPLAPI_hpp
#define MaPLAPI_hpp

#include <unordered_map>

#include "MaPLParser.h"
#include "MaPLCompilerHelpers.h"

/**
 * Represents the a type as described by a MaPL API. Can be a primitive, pointer, or reference to a generic type.
 */
struct MaPLGenericType {
    MaPLFile *file;
    antlr4::Token *token;
    MaPLPrimitiveType primitiveType;
    std::string pointerType;
    size_t genericIndex;
    std::vector<MaPLGenericType> generics;
    
    /**
     * @return A @c MaPLType that is equivalent to this generic type, if each generic type was substituted with the corresponding element from @c genericsSubstitutions.
     */
    MaPLType typeWithSubstitutedGenerics(const std::vector<MaPLType> &genericsSubstitutions) const;
    
    /**
     * @return A @c MaPLGenericType that is equivalent to this generic type, if each generic type was substituted with the corresponding element from @c genericsSubstitutions.
     */
    MaPLGenericType genericWithSubstitutedGenerics(const std::vector<MaPLGenericType> &genericsSubstitutions) const;
    
    /**
     * @return A @c MaPLType that is equivalent to this generic type, if all generic types are interpreted as pointers with a @c pointerType matching the generic descriptor.
     */
    MaPLType typeWithoutSubstitutedGenerics() const;
    
    bool operator== (const MaPLGenericType &otherType) const;
    bool operator!= (const MaPLGenericType &otherType) const;
};

struct MaPLFunctionAPI {
    MaPLFile *file;
    MaPLParser::ApiFunctionContext *node;
    std::string name;
    std::string typeAPIName;
    MaPLGenericType returnType;
    std::vector<MaPLGenericType> parameterTypes;
    bool isVariadic;
    bool declaredInDependency = false;
    
    /**
     * @return The symbol representation of this function. This is used in the symbol table.
     */
    std::string symbolDescriptor() const;
    
    /**
     * @return The signature representation of this function. This can be useful for error logging.
     */
    std::string signatureDescriptor() const;
};

struct MaPLPropertyAPI {
    MaPLFile *file;
    MaPLParser::ApiPropertyContext *node;
    std::string name;
    std::string typeAPIName;
    MaPLGenericType returnType;
    bool isReadonly;
    bool declaredInDependency = false;
    
    /**
     * @return The symbol representation of this property. This is used in the symbol table.
     */
    std::string symbolDescriptor() const;
};

struct MaPLSubscriptAPI {
    MaPLFile *file;
    MaPLParser::ApiSubscriptContext *node;
    std::string typeAPIName;
    MaPLGenericType returnType;
    MaPLGenericType indexType;
    bool isReadonly;
};

struct MaPLTypeAPI {
    void assimilate(const MaPLFunctionAPI &function);
    void assimilate(const MaPLPropertyAPI &property);
    void assimilate(const MaPLSubscriptAPI &subscript);
    
    MaPLFile *file;
    MaPLParser::ApiTypeContext *node;
    std::string name;
    std::vector<std::string> generics;
    std::vector<MaPLGenericType> supertypes;
    std::vector<MaPLFunctionAPI> functions;
    std::unordered_map<std::string, MaPLPropertyAPI> properties;
    std::vector<MaPLSubscriptAPI> subscripts;
    bool declaredInDependency = false;
};

class MaPLAPI {
public:
    const MaPLTypeAPI *findType(const std::string &typeName) const;
    
    const MaPLFunctionAPI *findTypeFunction(const std::string &typeName,
                                            const std::string &functionName,
                                            const std::vector<MaPLType> &parameterTypes,
                                            const std::vector<MaPLType> &substitutedGenerics,
                                            const MaPLFunctionAPI *excludingFunction) const;
    
    const MaPLPropertyAPI *findTypeProperty(const std::string &typeName,
                                            const std::string &propertyName,
                                            const MaPLPropertyAPI *excludingProperty) const;
    
    const MaPLSubscriptAPI *findSubscript(const std::string &typeName,
                                          const MaPLType &indexType,
                                          const std::vector<MaPLType> &substitutedGenerics,
                                          const MaPLSubscriptAPI *excludingSubscript) const;
    
    const MaPLFunctionAPI *findGlobalFunction(const std::string &functionName,
                                              const std::vector<MaPLType> &parameterTypes,
                                              const MaPLFunctionAPI *excludingFunction) const;
    
    const MaPLPropertyAPI *findGlobalProperty(const std::string &propertyName) const;
    
    /**
     * @return @c true if @c expressionType can be assigned to a variable of @c assignToType without error. @c false if @c assignToType is ambiguous.
     */
    bool isAssignable(const MaPLType &expressionType, const MaPLType &assignToType) const;
    
    /**
     * @return @c true if @c ancestorTypeName is an ancestor of @c typeName.
     */
    bool inheritsFromType(const std::string &typeName, const std::string &ancestorTypeName) const;
    
    /**
     * @return A @c MaPLType that represents an ancestor type of @c fromType, but with the generics mapped from @c fromType to those of @c toAncestorName.
     */
    MaPLType findEquivalentGenerics(const MaPLType &fromType, const std::string &toAncestorName) const;
    
    /**
     * Populates @c symbolTable with all symbols that appear in this API.
     */
    void collateSymbolsInAPI(std::map<std::string, MaPLSymbol> &symbolTable);
    
    /**
     * Some types of errors can only be detected once all APIs are assimilated. Call this after adding / assimilating all APIs.
     */
    void performErrorChecking();
    
    /**
     * Detects the presence of invalid inheritance patterns.
     */
    void findInheritanceCyclesAndDiamonds();
    
    /**
     * Assimilate the contents of @c otherAPI with this one.
     */
    void assimilate(const MaPLAPI *otherAPI);
    
    /**
     * Traverse @c program and assimilate all APIs.
     */
    void assimilate(MaPLParser::ProgramContext *program, MaPLFile *file);
    
    /**
     * Flags all contents of this API as being from a dependent file.
     */
    void flagAllContentsAsDependency();
    
private:
    
    void assimilate(const MaPLFunctionAPI &globalFunction);
    void assimilate(const MaPLPropertyAPI &globalProperty);
    void assimilate(const MaPLTypeAPI &type);
    
    std::vector<MaPLFunctionAPI> globalFunctions;
    std::unordered_map<std::string, MaPLPropertyAPI> globalProperties;
    std::unordered_map<std::string, MaPLTypeAPI> types;
};

MaPLFunctionAPI functionAPIForNode(MaPLParser::ApiFunctionContext *node, const std::vector<std::string> &generics, MaPLFile *file);
MaPLPropertyAPI propertyAPIForNode(MaPLParser::ApiPropertyContext *node, const std::vector<std::string> &generics, MaPLFile *file);
MaPLSubscriptAPI subscriptAPIForNode(MaPLParser::ApiSubscriptContext *node, const std::vector<std::string> &generics, MaPLFile *file);
MaPLTypeAPI typeAPIForNode(MaPLParser::ApiTypeContext *node, MaPLFile *file);

#endif /* MaPLAPI_hpp */

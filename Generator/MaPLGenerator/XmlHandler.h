//
//  XmlHandler.h
//  MaPLGenerator
//
//  Created by Brad Bambara on 5/25/22.
//

#ifndef XmlHandler_h
#define XmlHandler_h

#include <filesystem>
#include <vector>

#include "tinyxml2.h"
#include "MaPLInterface.h"
#include "MaPLGeneratorCollections.h"

class XmlNode;
class XmlAttribute;

class XmlFile : public MaPLInterface {
public:
    XmlFile(const tinyxml2::XMLDocument *document, const std::filesystem::path &filePath);
    virtual MaPLParameter invokeFunction(MaPLSymbol functionSymbol, const MaPLParameter *argv, MaPLParameterCount argc);
    virtual MaPLParameter invokeSubscript(MaPLParameter index);
    
    const XmlNode *_rootNode;
    std::string _filePath;
};

class XmlNode : public MaPLInterface {
public:
    XmlNode(const tinyxml2::XMLElement *node, const std::filesystem::path &filePath);
    virtual MaPLParameter invokeFunction(MaPLSymbol functionSymbol, const MaPLParameter *argv, MaPLParameterCount argc);
    virtual MaPLParameter invokeSubscript(MaPLParameter index);
    
    const tinyxml2::XMLElement *_node;
    std::string _name;
    std::string _namespace;
    MaPLArrayMap<XmlAttribute *> *_attributes;
    MaPLArray<XmlNode *> *_children;
    MaPLArrayMap<MaPLArray<XmlNode *> *> *_childrenBySequence;
};

class XmlAttribute : public MaPLInterface {
public:
    XmlAttribute(const tinyxml2::XMLAttribute *attribute);
    virtual MaPLParameter invokeFunction(MaPLSymbol functionSymbol, const MaPLParameter *argv, MaPLParameterCount argc);
    virtual MaPLParameter invokeSubscript(MaPLParameter index);
    
    const tinyxml2::XMLAttribute *_attribute;
    std::string _name;
    std::string _value;
    MaPLArray<std::string> *_values;
};

MaPLArray<XmlFile *> *xmlFilesForPaths(const std::vector<std::filesystem::path> &xmlPaths);

#endif /* XmlHandler_h */

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
#include <libxml/tree.h>

#include "MaPLInterface.h"
#include "MaPLGeneratorCollections.h"

class XmlAttribute;

class XmlNode : public MaPLInterface {
public:
    XmlNode(xmlNode *node);
    virtual MaPLParameter invokeFunction(MaPLSymbol functionSymbol, const MaPLParameter *argv, MaPLParameterCount argc);
    virtual MaPLParameter invokeSubscript(MaPLParameter index);
    
    xmlNode *_node;
    std::string _name;
    std::string _namespace;
    MaPLArrayMap<XmlAttribute *> *_attributes;
    MaPLArray<XmlNode *> *_children;
};

class XmlAttribute : public MaPLInterface {
public:
    XmlAttribute(xmlAttr *attribute);
    virtual MaPLParameter invokeFunction(MaPLSymbol functionSymbol, const MaPLParameter *argv, MaPLParameterCount argc);
    virtual MaPLParameter invokeSubscript(MaPLParameter index);
    
    xmlAttr *_attribute;
    std::string _name;
    std::string _value;
};

MaPLArray<XmlNode *> *xmlNodesForPaths(const std::vector<std::filesystem::path> &xmlPaths);

#endif /* XmlHandler_h */

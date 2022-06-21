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

class XmlNode;
class XmlAttribute;

class XmlFile : public MaPLInterface {
public:
    XmlFile(xmlDoc *document);
    virtual MaPLParameter invokeFunction(MaPLSymbol functionSymbol, const MaPLParameter *argv, MaPLParameterCount argc);
    virtual MaPLParameter invokeSubscript(MaPLParameter index);
    
    XmlNode *_rootNode;
    std::string _filePath;
};

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
    MaPLArrayMap<MaPLArray<XmlNode *> *> *_childrenBySequence;
};

class XmlAttribute : public MaPLInterface {
public:
    XmlAttribute(xmlAttr *attribute);
    virtual MaPLParameter invokeFunction(MaPLSymbol functionSymbol, const MaPLParameter *argv, MaPLParameterCount argc);
    virtual MaPLParameter invokeSubscript(MaPLParameter index);
    
    xmlAttr *_attribute;
    std::string _name;
    std::string _value;
    MaPLArray<std::string> *_values;
};

MaPLArray<XmlFile *> *xmlFilesForPaths(const std::vector<std::filesystem::path> &xmlPaths);

#endif /* XmlHandler_h */

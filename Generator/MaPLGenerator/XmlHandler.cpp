//
//  XmlHandler.cpp
//  MaPLGenerator
//
//  Created by Brad Bambara on 5/25/22.
//

#include "XmlHandler.h"

#include <libxml/parser.h>

#include "ErrorLogger.h"

XmlNode::XmlNode(xmlNode *node) :
_node(node),
_name((char *)node->name) {
    // XML requires that namespaces be articulated as URLs. MaPLGenerator expects that
    // the string after the URL's scheme matches the namespace in the EaSL schema.
    std::string href = (char *)node->ns->href;
    _namespace = href.substr(href.find("://") + 3);
    
    std::vector<XmlNode *> children;
    for (xmlNode *child = node->children; child; child = child->next) {
        if (child->type != XML_ELEMENT_NODE) { continue; }
        children.push_back(new XmlNode(child));
    }
    _children = new MaPLArray<XmlNode *>(children);
    
    std::vector<XmlAttribute *> attributes;
    for (xmlAttr *attribute = node->properties; attribute; attribute = attribute->next) {
        attributes.push_back(new XmlAttribute(attribute));
    }
    _attributes = new MaPLArray<XmlAttribute *>(attributes);
}

MaPLParameter XmlNode::invokeFunction(MaPLSymbol functionSymbol, const MaPLParameter *argv, MaPLParameterCount argc) {
    switch (functionSymbol) {
        case MaPLSymbols_XMLNode_attributes:
            return MaPLPointer(_attributes);
        case MaPLSymbols_XMLNode_children:
            return MaPLPointer(_children);
        case MaPLSymbols_XMLNode_name:
            return MaPLStringByReference(_name.c_str());
        case MaPLSymbols_XMLNode_namespace:
            return MaPLStringByReference(_namespace.c_str());
        default:
            return MaPLUninitialized();
    }
}

MaPLParameter XmlNode::invokeSubscript(MaPLParameter index) {
    return MaPLUninitialized();
}

XmlAttribute::XmlAttribute(xmlAttr *attribute) :
_attribute(attribute),
_name((char *)attribute->name),
_value((char *)xmlGetProp(attribute->parent, attribute->name)) {
}

MaPLParameter XmlAttribute::invokeFunction(MaPLSymbol functionSymbol, const MaPLParameter *argv, MaPLParameterCount argc) {
    switch (functionSymbol) {
        case MaPLSymbols_XMLAttribute_name:
            return MaPLStringByReference(_name.c_str());
        case MaPLSymbols_XMLAttribute_value:
            return MaPLStringByReference(_value.c_str());
        default:
            return MaPLUninitialized();
    }
}

MaPLParameter XmlAttribute::invokeSubscript(MaPLParameter index) {
    return MaPLUninitialized();
}

MaPLArray<XmlNode *> *xmlNodesForPaths(const std::vector<std::filesystem::path> &xmlPaths) {
    std::vector<XmlNode *> xmlNodesVector;
    
    for (const std::filesystem::path &xmlPath : xmlPaths) {
        xmlDoc *doc = xmlReadFile(xmlPath.c_str(), NULL, 0);
        if (doc == NULL) {
            ErrorLogger logger(xmlPath);
            logger.logError("Failed to open data file.");
            exit(1);
        }
        xmlNode *node = xmlDocGetRootElement(doc);
        xmlNodesVector.push_back(new XmlNode(node));
    }
    
    return new MaPLArray<XmlNode *>(xmlNodesVector);
}

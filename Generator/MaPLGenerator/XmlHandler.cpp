//
//  XmlHandler.cpp
//  MaPLGenerator
//
//  Created by Brad Bambara on 5/25/22.
//

#include "XmlHandler.h"

#include <libxml/parser.h>
#include <unordered_map>
#include <sstream>

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
    
    // Initialize this empty. It gets filled in during validation.
    std::vector<MaPLArray<XmlNode *> *> emptyVector;
    std::unordered_map<std::string, MaPLArray<XmlNode *> *> emptyMap;
    _childrenBySequence = new MaPLArrayMap<MaPLArray<XmlNode *> *>(emptyVector, emptyMap);
    
    std::vector<XmlAttribute *> attributes;
    std::unordered_map<std::string, XmlAttribute *> attributeMap;
    for (xmlAttr *xmlAttribute = node->properties; xmlAttribute; xmlAttribute = xmlAttribute->next) {
        XmlAttribute *attribute = new XmlAttribute(xmlAttribute);
        attributes.push_back(attribute);
        attributeMap[attribute->_name] = attribute;
    }
    _attributes = new MaPLArrayMap<XmlAttribute *>(attributes, attributeMap);
}

MaPLParameter XmlNode::invokeFunction(MaPLSymbol functionSymbol, const MaPLParameter *argv, MaPLParameterCount argc) {
    switch (functionSymbol) {
        case MaPLSymbols_XMLNode_attributes:
            return MaPLPointer(_attributes);
        case MaPLSymbols_XMLNode_children:
            return MaPLPointer(_children);
        case MaPLSymbols_XMLNode_childrenBySequence:
            return MaPLPointer(_childrenBySequence);
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
    std::vector<std::string> commaDelimitedValues;
    std::stringstream stringStream(_value);
    const char* whitespaceCharset = " \f\n\r\t\v";
    while (stringStream.good()) {
        std::string splitValue;
        std::getline(stringStream, splitValue, ',');
        splitValue.erase(splitValue.find_last_not_of(whitespaceCharset)+1);
        splitValue.erase(0,splitValue.find_first_not_of(whitespaceCharset));
        commaDelimitedValues.push_back(splitValue);
    }
    _values = new MaPLArray<std::string>(commaDelimitedValues);
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

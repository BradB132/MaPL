//
//  XmlHandler.cpp
//  MaPLGenerator
//
//  Created by Brad Bambara on 5/25/22.
//

#include "XmlHandler.h"

#include <unordered_map>
#include <sstream>

#include "ErrorLogger.h"

XmlFile::XmlFile(const tinyxml2::XMLDocument *document, const std::filesystem::path &filePath) :
_rootNode(new XmlNode(document->RootElement(), filePath)),
_filePath(filePath.u8string()) {
}

MaPLParameter XmlFile::invokeFunction(MaPLSymbol functionSymbol, const MaPLParameter *argv, MaPLParameterCount argc) {
    switch (functionSymbol) {
        case MaPLSymbols_XMLFile_filePath:
            return MaPLStringByReference(_filePath.c_str());
        case MaPLSymbols_XMLFile_rootNode:
            return MaPLPointer((void *)_rootNode);
        default:
            return MaPLUninitialized();
    }
}

MaPLParameter XmlFile::invokeSubscript(MaPLParameter index) {
    return MaPLUninitialized();
}

XmlNode::XmlNode(const tinyxml2::XMLElement *node, const std::filesystem::path &filePath) :
_node(node) {
    
    const tinyxml2::XMLElement *rootElement = node->GetDocument()->RootElement();
    
    // Parse the name vs namespace.
    std::string fullName = node->Name();
    size_t namespaceSeparatorIndex = fullName.find(":");
    std::string namespaceURL;
    if (namespaceSeparatorIndex != std::string::npos) {
        _name = fullName.substr(namespaceSeparatorIndex + 1);
        std::string namespaceIdentifier = fullName.substr(0, namespaceSeparatorIndex);
        std::string prefix = "xmlns:";
        const char *matchingNamespace = rootElement->Attribute((prefix + namespaceIdentifier).c_str());
        if (!matchingNamespace) {
            ErrorLogger logger(filePath);
            logger.logError(node, "The root element in this document must have a 'xmlns' attribute which supplies a namespace URL for the '"+namespaceIdentifier+"' identifier. It should look something like: xmlns:"+namespaceIdentifier+"=\"schema://MyNamespace\".");
            exit(1);
        }
        namespaceURL = matchingNamespace;
    } else {
        _name = fullName;
        const char *defaultNamespace = rootElement->Attribute("xmlns");
        if (!defaultNamespace) {
            ErrorLogger logger(filePath);
            logger.logError(node, "The root element in this document must have a 'xmlns' attribute which supplies a namespace URL. It should look something like: xmlns=\"schema://MyNamespace\".");
            exit(1);
        }
        namespaceURL = defaultNamespace;
    }
    
    // XML requires that namespaces be articulated as URLs. MaPLGenerator expects that
    // the string after the URL's scheme matches the namespace in the EaSL schema.
    size_t namespaceURLPrefixIndex = namespaceURL.find("://");
    if (namespaceURLPrefixIndex != std::string::npos) {
        _namespace = namespaceURL.substr(namespaceURLPrefixIndex + 3);
    } else {
        ErrorLogger logger(filePath);
        logger.logError(node, "The 'xmlns' value for this node must be represented as a URL. It should look something like: xmlns=\"schema://MyNamespace\".");
        exit(1);
    }
    
    std::vector<XmlNode *> children;
    for(const tinyxml2::XMLElement* child = node->FirstChildElement(); child; child = child->NextSiblingElement()) {
        children.push_back(new XmlNode(child, filePath));
    }
    _children = new MaPLArray<XmlNode *>(children);
    
    // Initialize this empty. It gets filled in during validation.
    std::vector<MaPLArray<XmlNode *> *> emptyVector;
    std::unordered_map<std::string, MaPLArray<XmlNode *> *> emptyMap;
    _childrenBySequence = new MaPLArrayMap<MaPLArray<XmlNode *> *>(emptyVector, emptyMap);
    
    std::vector<XmlAttribute *> attributes;
    std::unordered_map<std::string, XmlAttribute *> attributeMap;
    for(const tinyxml2::XMLAttribute* xmlAttribute = node->FirstAttribute(); xmlAttribute; xmlAttribute=xmlAttribute->Next()) {
        if (node == rootElement) {
            const char *xmlAttributeName = xmlAttribute->Name();
            if (!strncmp(xmlAttributeName, "xmlns", 5)) {
                // This attribute is specifying XML namespace and should not be examined as if it's an attribute of this object.
                continue;
            }
        }
        
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

XmlAttribute::XmlAttribute(const tinyxml2::XMLAttribute *attribute) :
_attribute(attribute),
_name(attribute->Name()),
_value(attribute->Value()) {
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
        case MaPLSymbols_XMLAttribute_values:
            return MaPLPointer(_values);
        default:
            return MaPLUninitialized();
    }
}

MaPLParameter XmlAttribute::invokeSubscript(MaPLParameter index) {
    return MaPLUninitialized();
}

MaPLArray<XmlFile *> *xmlFilesForPaths(const std::vector<std::filesystem::path> &xmlPaths) {
    std::vector<XmlFile *> xmlFilesVector;
    
    for (const std::filesystem::path &xmlPath : xmlPaths) {
        tinyxml2::XMLDocument *createdDocument = new tinyxml2::XMLDocument();
        tinyxml2::XMLError loadResult = createdDocument->LoadFile(xmlPath.u8string().c_str());
        if (loadResult != tinyxml2::XML_SUCCESS) {
            ErrorLogger logger(xmlPath);
            logger.logError("Failed to open data file.");
            exit(1);
        }
        xmlFilesVector.push_back(new XmlFile(createdDocument, xmlPath));
    }
    
    return new MaPLArray<XmlFile *>(xmlFilesVector);
}

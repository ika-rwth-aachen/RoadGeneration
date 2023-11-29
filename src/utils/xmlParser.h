/**
 * Road-Generation
 * --------------------------------------------------------
 * Copyright (c) 2021 Institut für Kraftfahrzeuge, RWTH Aachen, ika
 * Report bugs and download new versions https://github.com/ika-rwth-aachen/RoadGeneration
 *
 * This library is distributed under the MIT License.
 *
 * @file xmlParser.h
 *
 * @brief This file contains functions to parse xml code.
 *
 * @author Jannik Busse
 * Contact: jannik.busse@rwth-aachen.de, christian.geller@rwth-aachen.de
 *
 */

#pragma once

#include <xercesc/util/PlatformUtils.hpp>
#include <xercesc/util/XMLString.hpp>
#include <xercesc/dom/DOM.hpp>
#include <xercesc/util/OutOfMemoryException.hpp>
#include <xercesc/framework/LocalFileFormatTarget.hpp>
#include <xercesc/dom/DOMCDATASection.hpp>
#include <xercesc/parsers/XercesDOMParser.hpp>
#include <iostream>

using namespace XERCES_CPP_NAMESPACE;
using namespace std;

DOMImplementation *impl;
DOMDocument *doc;

bool initialized = false;

/**
 * @brief very simple error hander for the xerces dom parser
 *
 */
class ValidationErrorHandler : public xercesc::ErrorHandler
{
public:
    void warning(const xercesc::SAXParseException &ex);
    void error(const xercesc::SAXParseException &ex);
    void fatalError(const xercesc::SAXParseException &ex);
    void resetErrors();

private:
    void reportParseException(const xercesc::SAXParseException &ex);
};
void ValidationErrorHandler::reportParseException(const xercesc::SAXParseException &ex)
{
    char *message = xercesc::XMLString::transcode(ex.getMessage());
    std::cout << "\t" << message << " at line " << ex.getLineNumber() << " column " << ex.getColumnNumber() << std::endl;

    xercesc::XMLString::release(&message);
}

void ValidationErrorHandler::warning(const xercesc::SAXParseException &ex)
{
    reportParseException(ex);
}

void ValidationErrorHandler::error(const xercesc::SAXParseException &ex)
{
    reportParseException(ex);
}

void ValidationErrorHandler::fatalError(const xercesc::SAXParseException &ex)
{
    reportParseException(ex);
}

void ValidationErrorHandler::resetErrors()
{
}

/**
 * @brief wrapper class that handles conversion from xml string to string smoothly
 * 
 */
class XStr
{
public:
    // -----------------------------------------------------------------------
    //  Constructors and Destructor
    // -----------------------------------------------------------------------
    XStr(const char *const toTranscode)
    {
        // Call the private transcoding method
        fUnicodeForm = XMLString::transcode(toTranscode);
    }

    XStr(const XMLCh *transcoded)
    {
        // Call the private transcoding method
        fUnicodeForm = (XMLCh *)malloc(XMLString::stringLen(transcoded));

        XMLString::copyNString(fUnicodeForm, transcoded, XMLString::stringLen(transcoded));
    }

    ~XStr()
    {
        XMLString::release(&fUnicodeForm);
    }

    operator string()
    {
        char *c_type = XMLString::transcode(fUnicodeForm);
        std::string res(c_type);

        XMLString::release(&c_type);
        return res;
    }

    operator int()
    {
        char *c_type = XMLString::transcode(fUnicodeForm);
        std::string res(c_type);
        XMLString::release(&c_type);
        return stoi(res);
    }

    operator double()
    {
        char *c_type = XMLString::transcode(fUnicodeForm);
        std::string res(c_type);
        XMLString::release(&c_type);
        return stod(res);
    }

    operator XMLCh *() { return fUnicodeForm; }

    const XMLCh *unicodeForm() const
    {
        return fUnicodeForm;
    }

private:
    XMLCh *fUnicodeForm;
};
//just to make the code more readable
#define X(str) XStr(str)


/**
 * @brief Wrapper class that stores the xml dom context
 * 
 */
struct xmlTree
{

private:
    XercesDOMParser *parser;
    DOMDocument *doc;
    ValidationErrorHandler *inputHandler;

public:
    xmlTree()
    {
        try
        {

            if (!initialized)
            {
                XMLPlatformUtils::Initialize();
                initialized = true;
            }
            
            string schema = string_format("%s/xml/input.xsd", PROJ_DIR);
            const char *schema_path = schema.c_str();

            parser = new XercesDOMParser;
            inputHandler = new ValidationErrorHandler();
            
            parser->setValidationScheme(XercesDOMParser::Val_Always);
            parser->setDoNamespaces(true);
            parser->setDoSchema(true);
            parser->setExternalNoNamespaceSchemaLocation(schema_path);
            parser->setValidationConstraintFatal(true);
            parser->setErrorHandler(inputHandler);
            parser->setValidationSchemaFullChecking(true);
            parser->setHandleMultipleImports(true);
        }
        catch (const XMLException &toCatch)
        {
            cout << "ERR: in initializing xml Tree" << endl;
            return;
        }
    }

    DOMImplementation *getDocImpl()
    {
        return doc->getImplementation();
    }

    DOMElement *getRootElement()
    {
        if (doc != NULL)
        {
            return doc->getDocumentElement();
        }
        else
        {
            cerr << "ERR: in getRootElement(). xml document not parsed" << endl;
            return NULL;
        }
    }

    DOMDocument *getDoc()
    {
        return doc;
    }

    int parseDocument(const char *path)
    {
        try
        {
            parser->parse(path);
            doc = parser->getDocument();
        }
        catch (...)
        {
            std::cerr << "An error occurred during parsing\n " << std::endl;
            return 1;
        }
        return 0;
    }

    int getErrorCount()
    {
        return parser->getErrorCount();
    }

    xercesc_3_2::Grammar *loadGrammar(const char *const schema_file)
    {
        return parser->loadGrammar(schema_file, Grammar::SchemaGrammarType);
    }

    ~xmlTree()
    {
    }
};

string readNameFromNode(const DOMElement *node)
{
    if (node == NULL)
    {
        cout << "ERR in readNameFromNode; dom node does not exists!" << endl;
        cerr << "ERR in readNameFromNode; dom node does not exists!" << endl;
        return "";
    }

    char *c_type = XMLString::transcode(node->getTagName());
    std::string res(c_type);

    XMLString::release(&c_type);

    return res;
}

/**
 * @brief reads an attribute from a xercesC node without causing memory leak. use this method for reading attributes!!!
 *
 * @param node the node to read the attribute from
 * @param attribute attribute to read
 * @param suppressOutput suppress output if the attribute is not found.
 * @return string returns a string value. Returns an empty string if no attribute with the given name is found.
 */
string readStrAttrFromNode(const DOMElement *node, const char *attribute, bool suppressOutput = false)
{

    if (node == NULL)
    {
        if (!suppressOutput)
        {
            cout << "ERR: in readStrAttriValueFromNode; dom node does not exists!" << endl;
            cerr << "ERR: in readStrAttriValueFromNode; dom node does not exists!" << endl;
        }
        return "";
    }
    XMLCh *typestring = XMLString::transcode(attribute);
    DOMAttr *attr = node->getAttributeNode(typestring);

    if (attr == NULL)
    {
        return "";
    }
    char *c_type = XMLString::transcode(attr->getValue());
    std::string res(c_type);

    XMLString::release(&c_type);
    XMLString::release(&typestring);
    typestring = NULL;
    attr = NULL;

    return res;
}

int readIntAttrFromNode(const DOMElement *node, const char *attribute, bool suppress = false)
{
    string res = readStrAttrFromNode(node, attribute, suppress);
    if (res == "")
        return -1;
    return stoi(res);
}

double readDoubleAttrFromNode(const DOMElement *node, const char *attribute)
{
    string res = readStrAttrFromNode(node, attribute);
    if (res == "")
        return -1;
    return stod(res);
}

bool readBoolAttrFromNode(const DOMElement *node, const char *attribute)
{
    string str = readStrAttrFromNode(node, attribute);
    if (str == "")
        return NULL;
    return str == "1" || str == "True" || str == "true";
}

bool attributeExits(const DOMElement *node, const char *attribute)
{
    return !readStrAttrFromNode(node, attribute, true).empty();
}

/**
 * @brief Get the Next Sibling with the specified tag name. Return NULL if nothing is found.
 *
 * @param elem element which to find the sibling of
 * @param tag name of the desired sibling
 * @return DOMElement* sibling if found. NULL otherwise
 */
DOMElement *getNextSiblingWithTagName(DOMElement *elem, const char *tag)
{

    for (DOMElement *curr = elem->getNextElementSibling(); curr != NULL; curr = curr->getNextElementSibling())
    {
        if (!XMLString::compareString(X(tag), curr->getTagName()))
        {
            return curr;
        }
    }
    return NULL;
}

/**
 * @brief looks for the first node that with matching name in the xml document
 *
 * @param childName name too look for
 * @param res return Element
 * @return first child element with name. NULL if nothing is found
 */
DOMElement *getChildWithName(const DOMElement *node, const char *childName)
{
    if (node == NULL)
        return NULL;
    DOMElement *res = NULL;

    //DOMNodeList * nodelist = (DOMNodeList*)malloc(0); //this fixes the memory corruption bug that can occur but introduces memory leak beacuse the pointer will be changed
    DOMNodeList *nodelist = node->getElementsByTagName(X(childName));

    for (int i = 0; i < nodelist->getLength(); i++)
    {
        if (!XMLString::compareString(X(childName), nodelist->item(i)->getNodeName()))
        {
            res = (DOMElement *)nodelist->item(0);
            break;
        }
    }
    return res;
}

/**
 * @brief Get the First Child object
 *
 * @param node of the parent object
 * @return DOMElement* first child of parent object in the xml tree
 */
DOMElement *getFirstChildFromNode(const DOMElement *node)
{
    if (node == NULL)
        return NULL;
    return node->getFirstElementChild();
}

/**
 * @brief Reads the attribute from a child of a node. Use with caution: if multiple child nodes with the name exist, only the first one will be read.
 *
 * @param node the node whose childs should be read
 * @param firstchild name of the first child
 * @param attr name of the attribute
 * @return string value of the attribute
 */
string readAttributeFromChildren(DOMElement *node, const char *firstchild, const char *attr)
{
    return readStrAttrFromNode(getChildWithName(node, firstchild), attr);
}

/**
 * @brief Reads the attribute from a child of a child of a node. Use with caution: if multiple child nodes with the name exist, only the first one will be read.
 *
 * @param node the node whose childs should be read
 * @param firstchild name of the first child
 * @param firstchild name of the child of the first child
 * @param attr name of the attribute
 * @return string value of the attribute
 */
string readAttributeFromChildren(DOMElement *node, const char *firstchild, const char *secondchild, const char *attr)
{
    return readStrAttrFromNode(getChildWithName(getChildWithName(node, firstchild), secondchild), attr);
}

// Wrapper code to create xml trees with the xercesC dom parser
struct nodeElement
{

    nodeElement(const char *value)
    {
        createNode(value);
    }

    nodeElement(const char *value, nodeElement &parent)
    {
        createNode(value);
        appendToNode(parent);
    }

    ~nodeElement()
    {
        // TODO implement destructor
    }

    DOMElement *domelement;

    int createNode(const char *name)
    {
        if (!initialized)
        {
            cout << "error, xml parser not initialized" << endl;
            return 1;
        }
        int errorCode = 0;

        try
        {
            domelement = doc->createElement(X(name));
        }
        catch (const OutOfMemoryException &)
        {
            std::cerr << "OutOfMemoryException" << std::endl;
            errorCode = 5;
        }
        catch (const DOMException &e)
        {
            std::cerr << "DOMException code is:  " << e.code << std::endl;
            errorCode = 2;
        }
        catch (...)
        {
            std::cerr << "An error occurred creating the document" << std::endl;
            errorCode = 3;
        }

        return errorCode;
    }

    int addAttribute(const char *key, const char *value)
    {
        int errorCode = 0;
        try
        {
            domelement->setAttribute(X(key), X(value));
        }
        catch (const OutOfMemoryException &)
        {
            std::cerr << "OutOfMemoryException" << std::endl;
            errorCode = 5;
        }
        catch (const DOMException &e)
        {
            std::cerr << "DOMException code is:  " << e.code << std::endl;
            errorCode = 2;
        }
        catch (...)
        {
            std::cerr << "An error occurred creating the document" << std::endl;
            errorCode = 3;
        }
        return errorCode;
    }

    int addAttribute(const char *key, string value)
    {
        return addAttribute(key, value.c_str());
    }

    int addAttribute(const char *key, junctionGroupType value)
    {
        if (value == roundaboutType)
            return addAttribute(key, "roundabout");
        else
            return addAttribute(key, "unknown");
    }

    int addAttribute(const char *key, int value)
    {
        return addAttribute(key, to_string(value));
    }

    int addAttribute(const char *key, float value)
    {
        return addAttribute(key, to_string(value));
    }

    int addAttribute(const char *key, double value)
    {

        ostringstream strobj;
        strobj << value;
        return addAttribute(key, strobj.str());
    }

    int appendToNode(nodeElement &node)
    {
        int errorCode = 0;
        try
        {
            node.domelement->appendChild(domelement);
        }
        catch (const OutOfMemoryException &)
        {
            std::cerr << "OutOfMemoryException" << std::endl;
            errorCode = 5;
        }
        catch (const DOMException &e)
        {
            std::cerr << "DOMException code is:  " << e.code << std::endl;
            errorCode = 2;
        }
        catch (...)
        {
            std::cerr << "An error occurred creating the document" << std::endl;
            errorCode = 3;
        }
        return errorCode;
    }

    int appendToNode(DOMElement *node)
    {
        int errorCode = 0;
        try
        {
            node->appendChild(domelement);
        }
        catch (const OutOfMemoryException &)
        {
            std::cerr << "OutOfMemoryException" << std::endl;
            errorCode = 5;
        }
        catch (const DOMException &e)
        {
            std::cerr << "DOMException code is:  " << e.code << std::endl;
            errorCode = 2;
        }
        catch (...)
        {
            std::cerr << "An error occurred creating the document" << std::endl;
            errorCode = 3;
        }
        return errorCode;
    }

    int addTextNode(const char *text)
    {
        int errorCode = 0;
        try
        {
            DOMText *prodDataVal = doc->createTextNode(X(text));
            domelement->appendChild(prodDataVal);
        }
        catch (const OutOfMemoryException &)
        {
            std::cerr << "OutOfMemoryException" << std::endl;
            errorCode = 5;
        }
        catch (const DOMException &e)
        {
            std::cerr << "DOMException code is:  " << e.code << std::endl;
            errorCode = 2;
        }
        catch (...)
        {
            std::cerr << "An error occurred creating the document" << std::endl;
            errorCode = 3;
        }

        return errorCode;
    }
};

int generateCDATA(const char *data, DOMCDATASection **res)
{
    *res = doc->createCDATASection(X(data));
    return 0;
}

/**
 * @brief initializes the dom document. needs to be called befofe using the parser
 *
 * @param rootNode name of the root node
 * @return int error code
 */
int init(const char *rootNode)
{
    int errorCode = 0;
    try
    {
        if (!initialized)
        {
            XMLPlatformUtils::Initialize();
            initialized = true;
        }
    }
    catch (const XMLException &toCatch)
    {
        char *pMsg = XMLString::transcode(toCatch.getMessage());
        std::cerr << "Error during Xerces-c Initialization.\n"
                  << "  Exception message:"
                  << pMsg;
        XMLString::release(&pMsg);
        return 1;
    }

    impl = DOMImplementationRegistry::getDOMImplementation(X("Core"));

    if (impl != NULL)
    {
        try
        {
            doc = impl->createDocument(
                0,           // root element namespace URI.
                X(rootNode), // root element name
                0);          // document type object (DTD).
        }
        catch (const OutOfMemoryException &)
        {
            std::cerr << "OutOfMemoryException" << std::endl;
            errorCode = 5;
        }
        catch (const DOMException &e)
        {
            std::cerr << "DOMException code is:  " << e.code << std::endl;
            errorCode = 2;
        }
        catch (...)
        {
            std::cerr << "An error occurred creating the document" << std::endl;
            errorCode = 3;
        }
    }

    initialized = true;
    return 0;
}

/**
 * @brief seriaizes the constructed xml tree
 *
 * @param outname name of the output file
 * @return int error code
 */
int serialize(const char *outname)
{
    DOMLSSerializer *domSerializer = impl->createLSSerializer();

    DOMLSOutput *theOutputDesc = ((DOMImplementationLS *)impl)->createLSOutput();
    XMLFormatTarget *myFormTarget = new LocalFileFormatTarget(XMLString::transcode(outname));
    theOutputDesc->setByteStream(myFormTarget);
    theOutputDesc->setEncoding(XMLString::transcode("ISO-8859-1"));

    domSerializer->getDomConfig()->setParameter(XMLUni::fgDOMXMLDeclaration, true);

    domSerializer->getDomConfig()->setParameter(XMLUni::fgDOMWRTFormatPrettyPrint, true);
    domSerializer->write(doc, theOutputDesc);

    myFormTarget->flush();

    delete myFormTarget;

    theOutputDesc->release();
    domSerializer->release();

    doc->release();

    return 0;
}


DOMElement *getRootElement()
{
    return doc->getDocumentElement();
}

void terminateXMLUtils()
{
    XMLPlatformUtils::Terminate();
}

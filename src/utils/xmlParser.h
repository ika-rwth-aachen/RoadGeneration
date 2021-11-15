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


DOMImplementation* impl;
DOMDocument* doc;

bool initialized = false;



class XStr
{
public :
    // -----------------------------------------------------------------------
    //  Constructors and Destructor
    // -----------------------------------------------------------------------
    XStr(const char* const toTranscode)
    {
        // Call the private transcoding method
        fUnicodeForm = XMLString::transcode(toTranscode);
    }



    XStr(const XMLCh* transcoded)
    {
        // Call the private transcoding method
        fUnicodeForm = (XMLCh*)malloc(XMLString::stringLen(transcoded));

        XMLString::copyNString(fUnicodeForm,transcoded, XMLString::stringLen(transcoded));

    }


    ~XStr()
    {
        XMLString::release(&fUnicodeForm);
    }

    operator string() {
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

    operator XMLCh*() {return fUnicodeForm;}

    const XMLCh* unicodeForm() const
    {
        return fUnicodeForm;
    }

private :
    XMLCh*   fUnicodeForm;
};

#define X(str) XStr(str)


struct xmlTree{

    private:
        XercesDOMParser *parser;
        DOMDocument *doc;

    public:

        xmlTree(){
            try{
                if(!initialized){
                    XMLPlatformUtils::Initialize();
                    initialized = true;
                }
                parser = new XercesDOMParser;
                parser->setValidationScheme(XercesDOMParser::Val_Auto);
                parser->setDoNamespaces(true);
                parser->setDoSchema(true);
                parser->setValidationConstraintFatal(true);
                
            }
            catch(const XMLException &toCatch)
            {
                cout << "something wrong in xmlreader" << endl;
                return;
            }
        }

        DOMImplementation* getDocImpl()
        {
            return doc->getImplementation();
        }

        DOMElement *getRootElement()
        {
            if(doc != NULL)
            {
                return doc->getDocumentElement();
            }
            else
            {
                cerr << "ERR: in getRootElement(). xml document not parsed" <<endl;
                return NULL;
            }
        }

        DOMDocument *getDoc()
        {
            return doc;
        }

        int parseDocument(const char* path)
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

        /**
         * @brief looks for the first node that with matching name in the xml document
         * 
         * @param childName name too look for
         * @param res return Element
         * @return int 0 if a node is found. 1 if nothing is found with a matching name
         */
        int findNodeWithName(const char *childName, DOMElement *&res)
        {
            DOMNodeList * nodelist = doc->getElementsByTagName(X(childName));
            for(int i = 0; i < nodelist->getLength(); i++)
            {
                if(!XMLString::compareString(X(childName), nodelist->item(i)->getNodeName()))
                {
                    res = (DOMElement *)(nodelist->item(0));
                    return 0;
                }
            }
            return 1;
        }

        DOMNodeList *findNodeswithName(const char *childName)
        {
            DOMNodeList *nodelist = doc->getElementsByTagName(X(childName));
            return nodelist;
        }


        xercesc_3_2::Grammar *loadGrammar(const char *const schema_file)
        {
            return parser->loadGrammar(schema_file, Grammar::SchemaGrammarType);
        }

        ~xmlTree()
        {
            delete parser;
        }

};

string readNameFromNode(const DOMElement* node)
{
    if(node == NULL){
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
string readStrAttrFromNode(const DOMElement* node, const char* attribute, bool suppressOutput = false)
{

    if(node == NULL){
        cout << "ERR in readStrAttriValueFromNode; dom node does not exists!" << endl;
        cerr << "ERR in readStrAttriValueFromNode; dom node does not exists!" << endl;
        return "";
    }
    XMLCh *typestring = XMLString::transcode(attribute);
    DOMAttr* attr = node->getAttributeNode(typestring);
      if(attr == NULL){
        if(!suppressOutput)
        {
            cout << "ERR in readStrAttriValueFromNode; attr "<< attribute <<  " does not exists in node " << readNameFromNode(node) << endl;
            cerr << "ERR in readStrAttriValueFromNode; attr does not exists: " << attribute << endl;
        }
        return "";
    }
    char *c_type = XMLString::transcode(attr->getValue());
    std::string res(c_type);

    XMLString::release(&c_type); 
    XMLString::release(&typestring);

    return res;
}

int readIntAttrFromNode(const DOMElement* node, const char* attribute)
{
    string res = readStrAttrFromNode(node, attribute);
    if (res == "") return -1;
    return stoi(res);
}

double readDoubleAttrFromNode(const DOMElement* node, const char* attribute)
{
    string res = readStrAttrFromNode(node, attribute);
    if (res == "") return -1;
    return stod(res);
}



bool readBoolAttrFromNode(const DOMElement* node, const char* attribute)
{
    string str = readStrAttrFromNode(node, attribute);
    if(str == "") return NULL;
    return str == "1" || str == "True" || str == "true";
}

bool attributeExits(const DOMElement* node, const char* attribute)
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
DOMElement* getNextSiblingWithTagName(DOMElement* elem, const char* tag)
{

    for(DOMElement* curr = elem->getNextElementSibling(); curr != NULL; curr = curr->getNextElementSibling())
    {
        if(!XMLString::compareString(X(tag), curr->getTagName()))
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
    DOMElement* getChildWithName( const DOMElement* node, const char *childName)
    {
        if (node == NULL) return NULL;
        DOMNodeList * nodelist = node->getElementsByTagName(X(childName));
        for(int i = 0; i < nodelist->getLength(); i++)
        {
            if(!XMLString::compareString(X(childName), nodelist->item(i)->getNodeName()))
            {
                return (DOMElement*) nodelist->item(0);
            }
        }
        return NULL;
    }




//code for creating a xml document



//TODO im not sure if this is the best implementation. The wrapper class
// makes it easier for the xml.h module to produce the output file (less overhead and easier to read)
//but its more tedious to release all structs..
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
        //TODO implement destructor
    }


    DOMElement *domelement;

    int createNode(const char* name)
    {
        if(!initialized)
        {
            cout << "error, xml parser not initialized" << endl;
            return 1;
        }
        int errorCode = 0;

        try{
            domelement = doc->createElement(X(name));
        }
        catch (const OutOfMemoryException&)
        {
            std::cerr << "OutOfMemoryException" << std::endl;
            errorCode = 5;
        }
        catch (const DOMException& e)
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

    int addAttribute(const char* key, const char* value)
    {
        int errorCode = 0;
        try{
            domelement->setAttribute(X(key), X(value));
        }
        catch (const OutOfMemoryException&)
        {
            std::cerr << "OutOfMemoryException" << std::endl;
            errorCode = 5;
        }
        catch (const DOMException& e)
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

    int addAttribute(const char* key, string value)
    {
        return addAttribute(key, value.c_str());
    }


    int addAttribute(const char* key, int value)
    {
        return addAttribute(key, to_string(value));
    }

    int addAttribute(const char* key, float value)
    {
        return addAttribute(key, to_string(value));
    }
    
    int addAttribute(const char* key, double value)
    {
        return addAttribute(key, to_string(value));
    }


    int appendToNode(nodeElement &node)
    {
        int errorCode = 0;
        try{
            node.domelement->appendChild(domelement);
        }
        catch (const OutOfMemoryException&)
        {
            std::cerr << "OutOfMemoryException" << std::endl;
            errorCode = 5;
        }
        catch (const DOMException& e)
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
        try{
            node->appendChild(domelement);
        }
        catch (const OutOfMemoryException&)
        {
            std::cerr << "OutOfMemoryException" << std::endl;
            errorCode = 5;
        }
        catch (const DOMException& e)
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


    int addTextNode(const char* text)
    {
        int errorCode = 0;
        try{
            DOMText*    prodDataVal = doc->createTextNode(X(text));
            domelement->appendChild(prodDataVal);
        }
        catch (const OutOfMemoryException&)
        {
            std::cerr << "OutOfMemoryException" << std::endl;
            errorCode = 5;
        }
        catch (const DOMException& e)
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

int generateCDATA(const char* data, DOMCDATASection **res)
{
    *res = doc->createCDATASection(X(data));
    return 0;
}

/**
 * @brief inits the xml parser library
 * 
 * @return int Error code
 */
int init(const char *rootNode)
{
    int errorCode = 0;
 try
    {
        if(!initialized)
            XMLPlatformUtils::Initialize();
    }
    catch(const XMLException& toCatch)
    {
        char *pMsg = XMLString::transcode(toCatch.getMessage());
        std::cerr << "Error during Xerces-c Initialization.\n"
             << "  Exception message:"
             << pMsg;
        XMLString::release(&pMsg);
        return 1;
    }

    impl =  DOMImplementationRegistry::getDOMImplementation(X("Core"));

    if (impl != NULL)
    {
        try
        {
            doc = impl->createDocument(
                        0,                    // root element namespace URI.
                        X(rootNode),         // root element name
                        0);                   // document type object (DTD).
            
        }
        catch (const OutOfMemoryException&)
        {
            std::cerr << "OutOfMemoryException" << std::endl;
            errorCode = 5;
        }
        catch (const DOMException& e)
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

int serialize(const char* outname)
{
    DOMLSSerializer * theSerializer = impl->createLSSerializer();

    DOMLSOutput       *theOutputDesc = ((DOMImplementationLS*)impl)->createLSOutput();
    XMLFormatTarget *myFormTarget  = new LocalFileFormatTarget(XMLString::transcode(outname));
    theOutputDesc->setByteStream(myFormTarget);
    theOutputDesc->setEncoding(XMLString::transcode("ISO-8859-1"));

    theSerializer->getDomConfig()->setParameter(XMLUni::fgDOMXMLDeclaration, true);

    theSerializer->getDomConfig()->setParameter(XMLUni::fgDOMWRTFormatPrettyPrint, true);
    theSerializer->write(doc, theOutputDesc);

    myFormTarget->flush();

    delete myFormTarget;

    theOutputDesc->release();
    theSerializer->release();

    doc->release();

    return 0;
}

int terminateParser()
{
    XMLPlatformUtils::Terminate();
    return 0;

}

DOMElement* getRootElement()
{
    return doc->getDocumentElement();
}
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


//code for reading a xml document

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

    ~XStr()
    {
        XMLString::release(&fUnicodeForm);
    }

    const XMLCh* unicodeForm() const
    {
        return fUnicodeForm;
    }

private :
    XMLCh*   fUnicodeForm;
};

#define X(str) XStr(str).unicodeForm()


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
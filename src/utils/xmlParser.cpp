#include <xercesc/util/PlatformUtils.hpp>
#include <xercesc/util/XMLString.hpp>
#include <xercesc/dom/DOM.hpp>
#include <xercesc/util/OutOfMemoryException.hpp>
#include <xercesc/framework/LocalFileFormatTarget.hpp>
#include <iostream>
#include "xmlParser.h"

using namespace XERCES_CPP_NAMESPACE;
using namespace std;





/**
 * @brief inits the xml parser library
 * 
 * @return int Error code
 */
int init()
{
 try
    {
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
                        X("company"),         // root element name
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

int main(int argC, char*[])
{

    init();
    try
    {

               DOMElement* rootElem = doc->getDocumentElement();

               DOMElement*  devByElem = doc->createElement(X("developedBy"));
               rootElem->appendChild(devByElem);

               DOMText*    devByDataVal = doc->createTextNode(X("Apache Software Foundation"));
               devByElem->appendChild(devByDataVal);

               DOMElement* res;


               rootElem->appendChild(res);


     


                  
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
         // (inpl != NULL)

    serialize("outdoc.xml");
     
   

   XMLPlatformUtils::Terminate();
   return errorCode;
}
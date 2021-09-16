#include <xercesc/util/PlatformUtils.hpp>
#include <xercesc/util/XMLString.hpp>
#include <xercesc/dom/DOM.hpp>
#include <xercesc/util/OutOfMemoryException.hpp>
#include <xercesc/framework/LocalFileFormatTarget.hpp>
#include <iostream>


using namespace XERCES_CPP_NAMESPACE;
using namespace std;

DOMImplementation* impl;
DOMDocument* doc;


int errorCode = 0;
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

    ~XStr()
    {
        XMLString::release(&fUnicodeForm);
    }


    // -----------------------------------------------------------------------
    //  Getter methods
    // -----------------------------------------------------------------------
    const XMLCh* unicodeForm() const
    {
        return fUnicodeForm;
    }

private :
    // -----------------------------------------------------------------------
    //  Private data members
    //
    //  fUnicodeForm
    //      This is the Unicode XMLCh format of the string.
    // -----------------------------------------------------------------------
    XMLCh*   fUnicodeForm;
};

#define X(str) XStr(str).unicodeForm()



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

int createNode( const char* elemName, DOMElement* res)
{
    if(!initialized)
    {
        cout << "error, xml parser not initialized" << endl;
        return 1;
    }

    DOMElement* rootElem = doc->getDocumentElement();

    res = doc->createElement(X("node"));
    rootElem->appendChild(res);

    return 0;
}

int createNodeWithText(const char* elemName, const char* textNode, DOMElement* res)
{
        createNode(elemName, res);

        DOMText*    prodDataVal = doc->createTextNode(X("Xerces-C"));
        res->appendChild(prodDataVal);

        return 0;
}

int main(int argC, char*[])
{

    init();
    try
    {

       // <company>
       //     <product>Xerces-C</product>
       //     <category idea='great'>XML Parsing Tools</category>
       //     <developedBy>Apache Software Foundation</developedBy>
       // </company>

       

               DOMElement* rootElem = doc->getDocumentElement();

               DOMElement*  prodElem = doc->createElement(X("product"));
               rootElem->appendChild(prodElem);

               DOMText*    prodDataVal = doc->createTextNode(X("Xerces-C"));
               prodElem->appendChild(prodDataVal);

               DOMElement*  catElem = doc->createElement(X("category"));
               rootElem->appendChild(catElem);

               catElem->setAttribute(X("idea"), X("great"));

               DOMText*    catDataVal = doc->createTextNode(X("XML Parsing Tools"));
               catElem->appendChild(catDataVal);

               DOMElement*  devByElem = doc->createElement(X("developedBy"));
               rootElem->appendChild(devByElem);

               DOMText*    devByDataVal = doc->createTextNode(X("Apache Software Foundation"));
               devByElem->appendChild(devByDataVal);

               DOMElement* res;
               createNodeWithText("node", "hallo test string", res);

               //
               // Now count the number of elements in the above DOM tree.
               //

               const XMLSize_t elementCount = doc->getElementsByTagName(X("*"))->getLength();
               std::cout << "The tree just created contains: " << elementCount
                    << " elements." << std::endl;


                    DOMLSSerializer * theSerializer = impl->createLSSerializer();

                    DOMLSOutput       *theOutputDesc = ((DOMImplementationLS*)impl)->createLSOutput();
                    XMLFormatTarget *myFormTarget  = new LocalFileFormatTarget(XMLString::transcode("out.xml"));
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
     
   

   XMLPlatformUtils::Terminate();
   return errorCode;
}
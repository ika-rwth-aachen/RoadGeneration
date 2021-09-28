#include <xercesc/util/PlatformUtils.hpp>

#include <xercesc/dom/DOM.hpp>

#include <xercesc/framework/StdOutFormatTarget.hpp>
#include <xercesc/framework/LocalFileFormatTarget.hpp>
#include <xercesc/parsers/XercesDOMParser.hpp>
#include <xercesc/util/XMLUni.hpp>

#include <xercesc/util/OutOfMemoryException.hpp>

#include <iostream>
#include <cstring>
#include <cstdlib>


using namespace XERCES_CPP_NAMESPACE;
using namespace std;

static char*                    gXmlFile               = 0;
static bool                     gDoNamespaces          = false;
static bool                     gDoSchema              = false;
static bool                     gSchemaFullChecking    = false;
static bool                     gDoCreate              = false;

static char*                    goutputfile            = 0;
static char*                    gXPathExpression       = 0;

// options for DOMLSSerializer's features
static XMLCh*                   gOutputEncoding        = 0;

static bool                     gSplitCdataSections    = true;
static bool                     gDiscardDefaultContent = true;
static bool                     gUseFilter             = false;
static bool                     gFormatPrettyPrint     = false;
static bool                     gXMLDeclaration        = true;
static bool                     gWriteBOM              = false;


// ---------------------------------------------------------------------------
//
//  main
//
// ---------------------------------------------------------------------------
int main(int argC, char* argV[])
{
    int retval = 0;
    try
    {
        XMLPlatformUtils::Initialize();
    }
    catch(const XMLException &toCatch)
    {
        cout << "something wrong in xmlreader" << endl;
        return 1;
    }
    gXmlFile = "example.xml";

    //
    //  Create our parser, then attach an error handler to the parser.
    //  The parser will call back to methods of the ErrorHandler if it
    //  discovers errors during the course of parsing the XML document.
    //
    XercesDOMParser *parser = new XercesDOMParser;
    parser->setDoNamespaces(gDoNamespaces);
    parser->setDoSchema(gDoSchema);
    parser->setHandleMultipleImports (true);
    parser->setValidationSchemaFullChecking(gSchemaFullChecking);
    parser->setCreateEntityReferenceNodes(gDoCreate);


    //
    //  Parse the XML file, catching any XML exceptions that might propogate
    //  out of it.
    //
    bool errorsOccured = false;
    try
    {
        parser->parse(gXmlFile);
    }
    catch (...)
    {
        std::cerr << "An error occurred during parsing\n " << std::endl;
        errorsOccured = true;
    }

    // If the parse was successful, output the document data from the DOM tree
    if (!errorsOccured)
    {

        try
        {
            // get the DOM representation
            DOMDocument *doc = parser->getDocument();

            //
            // do the serialization through DOMLSSerializer::write();
            //
            DOMElement* root = doc->getDocumentElement() ;

            cout << XMLString::transcode(root->getNodeName())<< endl;
      
        }
    catch (...)
    {
        std::cerr << "An error occurred during parsing\n " << std::endl;
        errorsOccured = true;
    }
    //
    //  Delete the parser itself.  Must be done prior to calling Terminate, below.
    //
    }
    delete parser;

    XMLString::release(&gOutputEncoding);

    // And call the termination method
    XMLPlatformUtils::Terminate();



    cout << "ja ich habe funktioniert hehe" << endl;
    return retval;
}

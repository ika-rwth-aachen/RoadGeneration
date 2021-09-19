#include <xercesc/util/PlatformUtils.hpp>
#include <xercesc/util/XMLString.hpp>
#include <xercesc/dom/DOM.hpp>
#include <xercesc/util/OutOfMemoryException.hpp>
#include <xercesc/framework/LocalFileFormatTarget.hpp>
#include <iostream>
#include "xmlParser.h"

using namespace XERCES_CPP_NAMESPACE;
using namespace std;




int main(int argC, char*[])
{

    init();

    DOMElement* rootElem = getRootElement();


    nodeElement person("Persona");
    person.addTextNode("Manfred");
    person.addAttribute("gewicht", "100");
    person.appendToNode(rootElem);

    nodeElement person1("Persona1");
    person1.addTextNode("Manfred1");
    person1.addAttribute("gewicht1", "1001");
    person1.appendToNode(person);



                  
    
         // (inpl != NULL)

    serialize("outdoc.xml");
     
   
    terminateParser();
    return 0;
}
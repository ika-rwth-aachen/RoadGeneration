// file io.h

int parseXML(pugi::xml_document &doc)
{   
	if (doc.load_file("../xml/test.xml")) return 0;
    else return -1;

}

int createXML(pugi::xml_document &doc, roadNetwork data)
{   
    if (doc.save_file("../xml/output.xodr")) return 0;
    else return -1;
}
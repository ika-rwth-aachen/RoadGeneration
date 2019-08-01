// file buildOpenDriveData.h

int buildOpenDriveData(pugi::xml_document &doc, roadNetwork data)
{
	
	pugi::xml_node root = doc.child("roadNetwork");

	if(!root) cout << "ERR: 'roadNetwork' not found in input file."  << endl;

    // TODO remove
	root.set_name("rea");
	root.attribute("test").set_value("6");
	root.append_child("node");

    // output
	doc.save(std::cout);

    return 0;
}
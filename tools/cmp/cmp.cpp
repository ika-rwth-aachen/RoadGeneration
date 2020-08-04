#include "pugixml.hpp"
#include <iostream>
#include <stdio.h>
#include <string>
#include <vector>


pugi::xml_document doc1, doc2;
std::vector<std::string> list1, list2;

/*
const char* node_types[] =
{
    "null", "document", "element", "pcdata", "cdata", "comment", "pi", "declaration"
};
*/


struct simple_walker: pugi::xml_tree_walker
{
    std::vector<std::string> *list;
    virtual bool for_each(pugi::xml_node& node)
    {
        for (int i = 0; i < depth(); ++i) //std::cout << "  "; // indentation

        //std::cout << node_types[node.type()] << ": name='" << node.name() << "', value='" << node.value() << " ";
        list->push_back(node.name());
        list->push_back(node.value());

        for (pugi::xml_attribute_iterator ait = node.attributes_begin(); ait != node.attributes_end(); ++ait)
        {
        //  std::cout << " " << ait->name() << "=" << ait->value();
            list->push_back(ait->name());
            list->push_back(ait->value());
        }

        //std::cout << "\n";

        return true; // continue traversal
    }
};


int loadXML( const char* path, pugi::xml_document& doc){

    pugi::xml_parse_result result;    
    result = doc.load_file(path);

    if(result)
    {
        std::cout << "no errors while parsing" << std::endl;
    }
}

void printVector(std::vector<std::string> vector){
    for (std::string it: vector){
        std::cout << it << std::endl;
    }
}


int main(){
    std::cout << "compare test tool" << std::endl;


    loadXML("test.xml",doc1);
    loadXML("junction_4a.xodr",doc2);
    simple_walker walker;
    walker.list = &list1;
    doc1.traverse(walker);
    walker.list = &list2;
    doc2.traverse(walker);

    int i = 90;
    std::cout << list1[i] << std::endl;
    std::cout << list2[i] << std::endl;


    //printVector(list1);
    //printVector(list2);

    return 0;
}
#include "pugixml.hpp"
#include <iostream>
#include <stdio.h>
#include <string>
#include <vector>
#include <math.h>


pugi::xml_document doc1, doc2;
std::vector<std::string> list1, list2;

float threshold = 0.0001f;

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

int isFloat(std::string s){
    std::string::size_type sz;
    try{ 
        float parsed_number = std::stod(s, &sz); 
        return true;
    } 
    catch(std::exception& ia) 
	{ 
        return false; 
    } 
}

int compareLists(std::vector<std::string> l1, std::vector<std::string> l2){
    if (l1.size() != l2.size()){
        std::cout << "ERROR: different number of elements" << std::endl;
        return 1;
    }

    int s_error = 0;
    int n_error = 0;
    int error = 0;

    for(int i = 0; i < l1.size(); i++){
        std::string s1 = l1.at(i), s2 = l2.at(i);

        if(s1.compare(s2)){
            if(isFloat(s1) && isFloat(s2)){
                float f1 = std::stod(s1);
                float f2 = std::stod(s2);

                if(fabs(f1 - f2) > threshold){
                    error ++;
                    std::cout << f1 << "  <<-->>  " << f2 << std::endl;
                }else {
                    n_error ++;
                }
            }
            else{
                s_error ++;
                continue;
            }
        }
    }

    std::cout << "large numerical errors: " << error << "\nsmall numerical errors: " << n_error << "\nstring errors: " << s_error << std::endl;
    return error + s_error;
}


void printVector(std::vector<std::string> vector){
    for (std::string it: vector){
        std::cout << it << std::endl;
    }
}


int main(){
    std::cout << "compare test tool" << std::endl;


    loadXML("gt/junction_4a.xodr",doc1);
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

    return compareLists(list1, list2);
}
/**
 * Road-Generation
 * --------------------------------------------------------
 * Copyright (c) 2021 Institut für Kraftfahrzeuge, RWTH Aachen, ika
 * Report bugs and download new versions https://github.com/ika-rwth-aachen/RoadGeneration
 *
 * This library is distributed under the MIT License.
 * 
 * @file junctionWrapper.h
 *
 * @brief file contains method for generating junctions
 *
 * @author Christian Geller
 * Contact: christian.geller@rwth-aachen.de
 *
 */

#include "xjunction.h"
#include "tjunction.h"

/**
 * @brief function takes the junction as input argument and calls the junction type function
 * 
 * @param node  input data from the input file for the tjunction
 * @param data  roadNetwork structure where the generated roads and junctions are stored
 * @return int  error code
 */
int junctionWrapper(const DOMElement* node, roadNetwork &data)
{   

    //AVOID MEMORY LEAK
    XMLCh *typestring = XMLString::transcode("type");
    char *c_type = XMLString::transcode(node->getAttributeNode(typestring)->getValue());
    std::string type(c_type);
    XMLString::release(&c_type); 
    XMLString::release(&typestring);
    //AVOID MEMORY LEAK
    
    // check type of the junction (M = mainroad, A = accessroad)
    int mode = 0;
    if (type == "2M")
        mode = 1;
    if (type == "M2A")
        mode = 1;
    if (type == "4A")
        mode = 1;
    if (type == "MA")
        mode = 2;
    if (type == "3A")
        mode = 2;

    if (mode == 1)
    {
        cout << "TODO MODE 1 IN JUNCTION WRAPPER " << endl;
        // if (xjunction(node, data))
        // {
        //     cerr << "ERR: error in xjunction." << endl;
        //     return 1;
        // }
    }
    if (mode == 2)
    {
        cout << "TODO MODE 2 IN JUNCTION WRAPPER " << endl;

        // if (tjunction(node, data))
        // {
        //     cerr << "ERR: error in tjunction." << endl;
        //     return 1;
        // }
    }

    if (mode == 0)
    {
        cout << "TODO MODE 0 IN JUNCTION WRAPPER " << endl;
        cerr << "ERR: junction type is not defined correct." << endl;
        return 1;
    }
    return 0;
}
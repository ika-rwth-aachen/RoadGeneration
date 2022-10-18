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
 * @author Jannik Busse, Christian Geller
 * Contact: jannik.busse@rwth-aachen.de
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

    std::string type = X(node->getAttributeNode(X("type"))->getValue());

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
        if (xjunction(node, data))
        {
            cerr << "ERR: error in xjunction." << endl;
            return 1;
        }
    }
    if (mode == 2)
    {

        if (tjunction(node, data))
        {
            cerr << "ERR: error in tjunction." << endl;
            return 1;
        }
    }

    if (mode == 0)
    {
        cerr << "ERR: junction type is not defined correct." << endl;
        return 1;
    }
    return 0;
}
/**
 * @file junctionWrapper.h
 *
 * @brief function contains method for generating junctions
 *
 * @author Christian Geller
 * Contact: christian.geller@rwth-aachen.de
 *
 */

#include "xjunction.h"
#include "tjunction.h"

/**
 * @brief 
 * 
 * @param node 
 * @param data 
 * @return int 
 */
int junctionWrapper(pugi::xml_node &node, roadNetwork &data)
{
    // check type of the junction (M = mainroad, A = accessroad)
    int mode = 0;
    if ((string)node.attribute("type").value() == "2M")
        mode = 1;
    if ((string)node.attribute("type").value() == "M2A")
        mode = 1;
    if ((string)node.attribute("type").value() == "4A")
        mode = 1;
    if ((string)node.attribute("type").value() == "MA")
        mode = 2;
    if ((string)node.attribute("type").value() == "3A")
        mode = 2;

    if (mode == 1)
        xjunction(node, data);
    if (mode == 2)
        tjunction(node, data);
    if (mode == 0)
    {
        cerr << "ERR: junction type is not defined correct." << endl;
        return 1;
    }
    return 0;
}
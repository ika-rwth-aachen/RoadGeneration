/**
 * @file connectingRoad.h
 *
 * @brief file contains method for generating connecting road segment
 *
 * @author Christian Geller
 * Contact: christian.geller@rwth-aachen.de
 *
 */

extern settings setting;

/**
 * @brief function generates the road for a connecting road which is specified in the input file
 *  
 * @param node  input data from the input file for the connecting road
 * @param data  roadNetwork structure where the generated roads and junctions are stored
 * @return int  error code
 */
int connectingRoad(pugi::xml_node &node, roadNetwork &data)
{
    // define segment
    data.nSegment++;
    pugi::xml_node mainRoad = node.child("road");

    pugi::xml_node dummy;

    if (!mainRoad)
    {
        cerr << "ERR: specified road is not found.";
        return 1;
    }

    // --- generate roads ------------------------------------------------------
    if(!setting.silentMode)
        cout << "\t Generating Roads" << endl;

    road r;
    int id = mainRoad.attribute("id").as_int();
    r.id = 100 * node.attribute("id").as_int() + id;
    r.junction = node.attribute("id").as_int(); // <- might cause a bug
    //r.junction = -1;

    if (buildRoad(mainRoad, r, 0, INFINITY, dummy, 0, 0, 0, 0))
    {
        cerr << "ERR: error in buildRoad" << endl;
        return 1;
    }
    if (addObjects(mainRoad, r, data))
    {
        cerr << "ERR: error in addObjects" << endl;
        return 1;
    }

    data.roads.push_back(r);

    return 0;
}
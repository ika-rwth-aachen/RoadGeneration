// connectingRoad.h

int connectingRoad(pugi::xml_node &node, roadNetwork &data)
{
    // check type (all roads start in intersection point)
   
    // define junction roads
    pugi::xml_node mainRoad = node.child("road");

    if(!mainRoad)
    {
        cout << "ERR: no corresponding roads are found." << endl;
        exit(0);
    }
    
    cout << "Generating Roads" << endl;

    cout << "Road 1" << endl;
    road r;
    int id = mainRoad.attribute("id").as_int();
    r.id = 100 * node.attribute("id").as_int() + id;
    r.junction = node.attribute("id").as_int();
    r.predecessor.elementId = mainRoad.attribute("idStart").as_int();
    r.successor.elementId = mainRoad.attribute("idEnd").as_int();
    generateRoad(mainRoad, r, 0, 0, 0, 0, 0, 0);
    data.roads.push_back(r);

    return 0;
}
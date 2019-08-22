// connectingRoad.h

int connectingRoad(pugi::xml_node &node, roadNetwork &data)
{
    // define junction roads
    pugi::xml_node mainRoad = node.child("road");

    if(!mainRoad)
    {
        cerr << "ERR: specified road is not found.";
        return 1;
    }
    
    // --- generate roads ------------------------------------------------------
    cout << "\t Generating Roads" << endl;

    road r;
    int id = mainRoad.attribute("id").as_int();
    r.id = 100 * node.attribute("id").as_int() + id;
    r.junction = node.attribute("id").as_int();
    
    generateRoad(mainRoad, r, 0, INFINITY, 0, 0, 0, 0, 0);
    addObjects(mainRoad, r, data);

    data.roads.push_back(r);

    return 0;
}
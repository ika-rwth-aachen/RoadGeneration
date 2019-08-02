// tjunction.h

#include "generateRoad.h"
#include "createRoadConnections.h"

int tjunction(pugi::xml_node &node, roadNetwork &data)
{
    if ((string)node.attribute("type").value() == "M1A")
    {
        junction junc;
        junc.id = node.attribute("id").as_int();

        pugi::xml_node iP = node.child("intersectionPoint");
        pugi::xml_node cA = node.child("coupler").child("couplerArea");

        pugi::xml_node referenceRoad;
        pugi::xml_node additionalRoad;

        for (pugi::xml_node_iterator it = node.begin(); it != node.end(); ++it)
	    {
            if((string)it->name() == "road" && it->attribute("id").as_int() == iP.attribute("setReferenceRoad").as_int())
                referenceRoad = *it;

            if((string)it->name() == "road" && it->attribute("id").as_int() == iP.attribute("adRoadId").as_int())
                additionalRoad = *it;
        }   
        if(!additionalRoad || !referenceRoad) 
            cout << "ERR: no corresponding roads are found" << endl;


        // cut original roads
        double sOffset = stod(cA.attribute("sOffset").value(),&sz);
        double sMain = stod(iP.attribute("iPOnMainRoad").value(),&sz);
        double sAccess = stod(iP.attribute("iPOnAccessRoad").value(),&sz);
        double s1 = sMain - sOffset;
        double s2 = sMain + sOffset;
        double s3 = sAccess + sOffset;
        double phi = stod(iP.attribute("angleToReferenceRoad").value(),&sz);

        road r1;
        r1.id = 100*junc.id + 1;
        r1.junction = junc.id;
        r1.predecessor.elementId = referenceRoad.attribute("idStart").as_int();
        r1.successor.elementId   = 100*junc.id + 4;
        generateRoad(referenceRoad, r1, 1, s1, 0, 0, 0);
        data.roads.push_back(r1);

        road r2;
        r2.id = 100*junc.id + 2;
        r2.junction = junc.id;
        r2.predecessor.elementId = 100*junc.id + 4;
        r2.successor.elementId   = referenceRoad.attribute("idEnd").as_int();
        generateRoad(referenceRoad, r2, 2, s2, 0, 0, 0);
        data.roads.push_back(r2);

        road helperRoad;
        generateRoad(referenceRoad, helperRoad, 2, sMain, 0, 0, 0);
        double iPx = helperRoad.geometries.back().x;
        double iPy = helperRoad.geometries.back().y;

        road r3; 
        r3.id = 100*junc.id + 3;
        r3.junction = junc.id;
        r3.predecessor.elementId = 100*junc.id + 6;
        r3.successor.elementId   = additionalRoad.attribute("idEnd").as_int();
        generateRoad(additionalRoad, r3, 2, s3, phi, iPx, iPy);
        data.roads.push_back(r3);
        
        road r4; 
        r4.id = 100*junc.id + 4;
        r4.junction = junc.id;
        r4.predecessor.elementId = r1.id;
        r4.successor.elementId   = r2.id;
        createRoadConnection(r1,r2,r4,junc);
        data.roads.push_back(r4);

        road r5; 
        r5.id = 100*junc.id + 5;
        r5.junction = junc.id;
        r5.predecessor.elementId = r3.id;
        r5.successor.elementId   = r2.id;
        createRoadConnection(r3,r2,r5,junc); // false
        data.roads.push_back(r5);

        road r6; 
        r6.id = 100*junc.id + 6;
        r6.junction = junc.id;
        r6.predecessor.elementId = r1.id;
        r6.successor.elementId   = r3.id;
        createRoadConnection(r1,r3,r6,junc);
        data.roads.push_back(r6);

        data.junctions.push_back(junc);        
    }

    if ((string)node.attribute("type").value() == "3A")
    {
        // TODO
    }

    return 0;
}
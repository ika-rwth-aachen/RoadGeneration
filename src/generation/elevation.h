/**
 * Road-Generation
 * --------------------------------------------------------
 * Copyright (c) 2021 Institut für Kraftfahrzeuge, RWTH Aachen, ika
 * Report bugs and download new versions https://github.com/ika-rwth-aachen/RoadGeneration
 *
 * This library is distributed under the MIT License.
 * 
 * @file elevation.h
 *
 * @brief file contains methods to handle elevation profile generation
 *
 * @author Jannik Busse
 * Contact: jannik.busse@rwth-aachen.de
 *
 */

#include "utils/interface.h"
#include "utils/helper.h"
#include <vector>
#include <queue>
#include <iostream>

/**
 * @brief Gets the x offset of the LEFT polynomial of the cuttofpoint where the connecting line will be. This is in reference to the apex of the polynomial
 * 
 * @param c1 
 * @param c2 
 * @param o1 
 * @param o2 
 * @param y1 
 * @param y2 
 * @return double 
 */
double getX1CutOff(double c1, double  c2, double  o1, double  o2, double  y1, double y2)
{
    double bot = 2*c1 * (c2-c1);
    double sq = c1*c1 * c2*c2 * (2 * o1 - 2 * o2)*(2 * o1 - 2 * o2) + 4 * c1 * c2 * (c2 - c1) * (y1 - y2);
    return ( sqrt(sq) - c1*c2*(2*o1 - 2*o2))/bot;
}

double getX2CutOffFromX1(double x1, double c1, double c2)
{
    return c1*x1/c2;
}

double getCFromRadius(double radius)
{
    return 1/(2*radius);
}

/**
 * @brief Shifts a polynom of the form a + cx^2 on the x axis 
 * 
 * @param a 
 * @param c 
 * @param o amount of shift on the x axis
 */
void shiftPolynom(double a, double c, double o, double &sa, double &sb, double &sc)
{
    sa = a + c * o * o;
    sb = 2 * c * o;
    sc = c;
}

// Calculates f(x) for 3rd deg poly
double poly3(double x, double a, double b, double c, double d)
{
    return a + b * x + c * x * x + d * x * x * x;
}

// Calculates f'(x) for 3rd deg poly
double poly3Derivation(double x, double b, double c, double d)
{
    return b + 2*c*x + 3*d*x*x;
}

int generateElevationProfile(std::vector<elevationProfile> &eps)
{

    for(int i = 0; i < eps.size() - 1; i ++)
    {
        double o1 = eps[i].sOffset;
        double o2 = eps[i+1].sOffset;

        double y1 = eps[i].tOffset;
        double y2 = eps[i+1].tOffset;
        
        double r1 = eps[i].radius;
        double r2 = eps[i+1].radius;

        double c1 = getCFromRadius(r1);
        double c2 = getCFromRadius(r2);

        if (y1 > y2)
            c1 = - c1;
        else
            c2 = - c2;

        double x1 = getX1CutOff(c1, c2, o1, o2, y1, y2);
        double x2 = getX2CutOffFromX1(x1, c1, c2);

        elevationPolynom startPoly;
        elevationPolynom linePoly;
        elevationPolynom resPoly;

        startPoly.a = y1;
        startPoly.b = 0;
        startPoly.c = c1;
        startPoly.d = 0;
        startPoly.s = o1;
        
        linePoly.a = poly3(x1, y1,0,c1,0);
        linePoly.b = poly3Derivation(x1,0,c1,0);
        linePoly.c = 0;
        linePoly.d = 0;
        linePoly.s = o1 + x1;

        double sa, sb, sc;
        shiftPolynom(y2, c2, x2, sa, sb, sc);
        
        resPoly.a = sa;
        resPoly.b = sb;
        resPoly.c = sc;
        resPoly.d = 0;
        resPoly.s = o2 + x2;

        eps[i].outputElevation.push_back(startPoly);
        eps[i].outputElevation.push_back(linePoly);
        eps[i].outputElevation.push_back(resPoly);
    }
    return 0;
}



int generateElevationProfiles(const DOMElement* rootNode, roadNetwork &data)
{
    std::cout << "Generating Elevation Profiles TODO: DELETE THIS" << std::endl;

    //this helper struct stores information that about the parent object that is required to process the elevation of each road. 
    //Since (for instance) the successor of the successor of a road can be the same road again, we need to keep book of what road we were coming from
    struct elevationLinkInformation
    {
        road*               curRoad;                //road that needs processing
        road*               parentRoad;             //road that put curRoad in the processing queue. This can be successor or predecessor
        contactPointType    parentLinkingPoint      = noneType; //point where parent is linked to cur 
        contactPointType    curLinkPointToparent    = noneType; //point where cur is linked to parent
        double              offsetHeightAtLink      = 0; //height at linking point

    };

    //sanity checks
    for(road &r: data.roads)
    {
        //fill in missing elevation data
        if(r.elevationProfiles.size() == 0)
        {
            elevationProfile ep1, ep2;
            ep1.radius = 10;
            ep1.sOffset = 0;
            ep1.tOffset = 0;
            ep2.radius = 10;
            ep2.sOffset = r.length;
            ep2.tOffset = 0;

            r.elevationProfiles.push_back(ep1);
            r.elevationProfiles.push_back(ep2);
        }

        //check if s bounds are violated
        if(r.elevationProfiles[0].sOffset < 0 || r.elevationProfiles[r.elevationProfiles.size() -1].sOffset > r.length)
        {
            throwError("invalid elevation profile in segment " + std::to_string(r.inputSegmentId) + " road " + std::to_string(r.inputId));
            return 1;
        }

        //check if multiple elevation poitns occur at the same s
        if(r.elevationProfiles.size() > 1)
        {
            for(int i = 0; i < r.elevationProfiles.size() -2; i++)
            {
                if(r.elevationProfiles[i].sOffset == r.elevationProfiles[i+1].sOffset)
                {
                    throwWarning("multiple elevation points are defined for one s offset in segment " + std::to_string(r.inputSegmentId) + " road " + std::to_string(r.inputId));
                }
            }
        }
    }

    //--------------

    std::queue<elevationLinkInformation> remaining = std::queue<elevationLinkInformation>();
    std::vector<int> completededIds = std::vector<int>();


	DOMElement *links = getChildWithName(rootNode, "links");
	if (links == NULL)
	{
		throwWarning("'links' are not specified in input file.\n\t -> skip segment linking", true);

		return 0;
	}

	// define reference system
    
    //find ref road
    road* ref = NULL;
    for(road &r: data.roads)
    {
        if(r.inputSegmentId == data.refRoad)
        {
            ref = &r;
        }
    }
    if(NULL == ref)
    {
        throwError("Could not find reference road in elevation generation");
        return -1;
    }

    data.refElev = readIntAttrFromNode(links, "reElev"); // This tag is not yet defined. TODO: add this tag to xml scheme
    completededIds.push_back(ref->id);
    
    //handle successors of root
    if(ref->successor.id > -1)
    {
        road *suc;
        findRoad(data.roads, suc, ref->successor.id);

       
        
        if(suc->isLinkedToNetwork)
        {
            elevationLinkInformation eli;
            eli.curRoad = suc;
            eli.parentRoad = ref;
            eli.parentLinkingPoint = endType;
            if(eli.curRoad->predecessor.id == eli.parentRoad->id)
            {
                eli.curLinkPointToparent = startType;
            }
            else{
                eli.curLinkPointToparent = endType;
            }
            remaining.push(eli);
        }
        completededIds.push_back(ref->id);

    }
    
    //----handle predecessors of root-----------------------------------
    if(ref->predecessor.id > -1)
    {
        road *pre;
        findRoad(data.roads, pre, ref->predecessor.id);

        
        if(pre->isLinkedToNetwork)
        {
            elevationLinkInformation eli;
            eli.curRoad = pre;
            eli.parentRoad = ref;
            eli.parentLinkingPoint = startType;
            if(eli.curRoad->predecessor.id == eli.parentRoad->id)
            {
                eli.curLinkPointToparent = startType;
            }
            else{
                eli.curLinkPointToparent = endType;
            }
            remaining.push(eli);
        }
        completededIds.push_back(ref->id);

    }  

  
    while(remaining.size() > 0)
    {
     
        elevationLinkInformation curEli = remaining.front();
        remaining.pop();
        if(isIn(completededIds, curEli.curRoad->id))
            continue;

        if(curEli.parentLinkingPoint == noneType)//when linked from junction
        {
            curEli.curRoad->elevationOffset = curEli.parentRoad->elevationOffset;     
            completededIds.push_back(curEli.curRoad->id);
            std::cout << "processed " << curEli.curRoad->id << "and parent " << curEli.parentRoad->id << std::endl; 

        }
        else if(curEli.parentLinkingPoint == startType) // linked as predecessor in parent road
        {
            std::cout << "iterating loop with id " << curEli.curRoad->id << " and pre " << curEli.parentRoad->id << "\n";
            if(curEli.curLinkPointToparent == endType) // parent is successor in cur road
            {
                double elevationOffsetAtLink = curEli.parentRoad->getRelativeElevationAt(0) + curEli.parentRoad->elevationOffset;
                curEli.curRoad->elevationOffset = elevationOffsetAtLink - curEli.curRoad->getRelativeElevationAt(1);

            }

            else if(curEli.curLinkPointToparent == startType) //parent is predecessor
            {
                double elevationOffsetAtLink = curEli.parentRoad->getRelativeElevationAt(0) + curEli.parentRoad->elevationOffset;
                // <pre|  |suc>
                curEli.curRoad->elevationOffset = elevationOffsetAtLink;
            }

            completededIds.push_back(curEli.curRoad->id);
            
        }
        else if(curEli.parentLinkingPoint == endType) //linked as successor in parent road
        {
            std::cout << "iterating loop with id " << curEli.curRoad->id << " and suc " << curEli.parentRoad->id << "\n";
            if(curEli.curLinkPointToparent == endType ) //parent is successor in cur road
            {
                double elevationOffsetAtLink = curEli.parentRoad->getRelativeElevationAt(1) + curEli.parentRoad->elevationOffset;
                curEli.curRoad->elevationOffset = elevationOffsetAtLink - curEli.curRoad->getRelativeElevationAt(1);


            }
            else if(curEli.curLinkPointToparent == startType) //parent is predecessor in cur road
            {
                double elevationOffsetAtLink = curEli.parentRoad->getRelativeElevationAt(1) + curEli.parentRoad->elevationOffset;
                curEli.curRoad->elevationOffset = elevationOffsetAtLink;
            }

            completededIds.push_back(curEli.curRoad->id);
           
        }
        //Add the next road to the remaining queue
        if(curEli.curRoad->isConnectingRoad && curEli.curRoad->predecessor.id > -1 && curEli.curRoad->predecessor.id != curEli.parentRoad->id && curEli.curRoad->predecessor.elementType == roadType && !isIn(completededIds, curEli.curRoad->predecessor.id))
        {
            elevationLinkInformation newEli;
            road *ppre;
            if(curEli.curRoad->predecessor.elementType == roadType)
            {
                findRoad(data.roads, ppre, curEli.curRoad->predecessor.id);
                newEli.curRoad = ppre;
                newEli.parentRoad = curEli.curRoad;
                newEli.parentLinkingPoint = startType;
                if(newEli.curRoad->predecessor.id == newEli.parentRoad->id)
                {
                    newEli.curLinkPointToparent = startType;
                }
                else{
                    newEli.curLinkPointToparent = endType;
                }
                remaining.push(newEli);
            }
        }
        else if(curEli.curRoad->isConnectingRoad && curEli.curRoad->successor.id > -1 && curEli.curRoad->successor.id != curEli.parentRoad->id && curEli.curRoad->successor.elementType == roadType && !isIn(completededIds, curEli.curRoad->successor.id))
        {
            elevationLinkInformation newEli;
            road *ssuc;
            findRoad(data.roads, ssuc, curEli.curRoad->successor.id);
            newEli.curRoad = ssuc;
            newEli.parentRoad = curEli.curRoad;
            newEli.parentLinkingPoint = endType;
            if(newEli.curRoad->predecessor.id == newEli.parentRoad->id)
            {
                newEli.curLinkPointToparent = startType;
            }
            else{
                newEli.curLinkPointToparent = endType;
            }
            remaining.push(newEli);
        }

        else if(curEli.curRoad->predecessor.elementType == junctionType)//TODO: linking to junction
        {
            std::cout << "linking to junction\n";
            elevationLinkInformation newEli;
            road *ppre;
            findRoad(data.roads, ppre, curEli.curRoad->predecessor.id);
            //process all in junction
            for(road &r: data.roads)
            {
                if(!r.isConnectingRoad && r.junction == curEli.curRoad->predecessor.id && !isIn(completededIds, r.id))
                {
                    r.elevationOffset = curEli.curRoad->elevationOffset;
                    completededIds.push_back(r.id);
                }
                if(r.isConnectingRoad && r.predecessor.id == curEli.curRoad->predecessor.id && !isIn(completededIds, r.id))//this is a connecting road that is generated by the junction as has the junction as predecessor
                {
                    elevationLinkInformation junEli;
                    junEli.curRoad = &r;
                    junEli.parentRoad = curEli.curRoad;
                    junEli.parentLinkingPoint = noneType;
                    remaining.push(junEli);
                }

            }
        }
        

    }
   
  
    for(road &r:data.roads)
    {
        for(elevationProfile &p: r.elevationProfiles)
        {
            p.tOffset += r.elevationOffset; //converts relative offset to absolute offset
        }
        if (generateElevationProfile(r.elevationProfiles))
        {
            std::cerr << "ERR: error in generateElevationProfile" << std::endl;
            return 1;
        }
    }

    return 0;
}


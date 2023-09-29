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
#include <vector>
#include<queue>

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
double get_x1_CutOff(double c1, double  c2, double  o1, double  o2, double  y1, double y2)
{
    double bot = 2*c1 * (c2-c1);
    double sq = c1*c1 * c2*c2 * (2 * o1 - 2 * o2)*(2 * o1 - 2 * o2) + 4 * c1 * c2 * (c2 - c1) * (y1 - y2);
    return ( sqrt(sq) - c1*c2*(2*o1 - 2*o2))/bot;
}

double get_x2_CutOff_from_x1(double x1, double c1, double c2)
{
    return c1*x1/c2;
}

double get_c_from_radius(double radius)
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
void shift_polynom(double a, double c, double o, double &sa, double &sb, double &sc)
{
    sa = a + c * o * o;
    sb = 2 * c * o;
    sc = c;
}

// Calculates f(x) for 3rd deg poly
double poly_3(double x, double a, double b, double c, double d)
{
    return a + b * x + c * x * x + d * x * x * x;
}

// Calculates f'(x) for 3rd deg poly
double poly_3_derivation(double x, double b, double c, double d)
{
    return b + 2*c*x + 3*d*x*x;
}

int generateElevationProfile(vector<elevationProfile> &eps)
{
    cout << "DELETE ME: Startet elevation generation" << endl;

    for(int i = 0; i < eps.size() - 1; i ++)
    {
        double o1 = eps[i].sOffset;
        double o2 = eps[i+1].sOffset;

        double y1 = eps[i].tOffset;
        double y2 = eps[i+1].tOffset;
        
        double r1 = eps[i].radius;
        double r2 = eps[i+1].radius;

        double c1 = get_c_from_radius(r1);
        double c2 = get_c_from_radius(r2);

        if (y1 > y2)
            c1 = - c1;
        else
            c2 = - c2;

        double x1 = get_x1_CutOff(c1, c2, o1, o2, y1, y2);
        double x2 = get_x2_CutOff_from_x1(x1, c1, c2);

        elevationPolynom startPoly;
        elevationPolynom linePoly;
        elevationPolynom resPoly;

        startPoly.a = y1;
        startPoly.b = 0;
        startPoly.c = c1;
        startPoly.d = 0;
        startPoly.s = o1;
        
        linePoly.a = poly_3(x1, y1,0,c1,0);
        linePoly.b = poly_3_derivation(x1,0,c1,0);
        linePoly.c = 0;
        linePoly.d = 0;
        linePoly.s = o1 + x1;

        double sa, sb, sc;
        shift_polynom(y2, c2, x2, sa, sb, sc);
        
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
    cout << "Generating Elevation Profiles TODO: DELETE THIS" << endl;

    //this helper struct stores information that about the parent object that is required to process the elevation of each road. 
    //Since (for instance) the successor of the successor of a road can be the same road again, we need to keep book of what road we were coming from
    struct elevationLinkInformation
    {
        road*               curRoad;                //road that needs processing
        road*               parentRoad;             //road that put curRoad in the processing queue. This can be successor or predecessor
        int                 parentLinkingPoint = 0; //-1: curRoad was linked as predecessor. 1:road was linked as successor
        double              offsetHeightAtLink = 0; //height at linking point

    };

    //sanity checks
    for(road &r: data.roads)
    {
        if(r.elevationProfiles[0].sOffset < 0 || r.elevationProfiles[r.elevationProfiles.size() -1].sOffset > r.length)
        {
            throwError("invalid elevation profile in segment " + to_string(r.inputSegmentId) + " road " + to_string(r.inputId));
            return 1;
        }

        for(int i = 0; i < r.elevationProfiles.size() -2; i++)
        {
            if(r.elevationProfiles[i].sOffset == r.elevationProfiles[i+1].sOffset)
            {
                throwWarning("multiple elevation points are defined for one s offset in segment " + to_string(r.inputSegmentId) + " road " + to_string(r.inputId));
            }
        }
    }

    //--------------

    queue<elevationLinkInformation> remaining = queue<elevationLinkInformation>();
    vector<int> completededIds = vector<int>();

    if(!setting.suppressOutput)
		cout << "Processing linkSegments" << endl;


	DOMElement *links = getChildWithName(rootNode, "links");
	if (links == NULL)
	{
		throwWarning("'links' are not specified in input file.\n\t -> skip segment linking", true);

		return 0;
	}

	// define reference system
    
    //find ref road
    road* ref;
    for(road &r: data.roads)
    {
        if(r.inputId == data.refRoad)
        {
            ref = &r;
        }
    }

    data.refElev = readIntAttrFromNode(links, "reElev"); // This tag is not yet defined. TODO: add this tag to xml scheme
    completededIds.push_back(ref->id);

    if(ref->successor.id > -1)
    {
        road suc;
        findRoad(data.roads, suc, ref->successor.id);
        
        //when processing an element, adjust the next roads elevation offset. This way we dont need to keep the current offset stored
       
        
        if(suc.isLinkedToNetwork)
        {
            elevationLinkInformation eli;
            eli.curRoad = &suc;
            eli.parentRoad = ref;
            eli.parentLinkingPoint = ref->successor.contactPoint;

            remaining.push(eli);
        }
        completededIds.push_back(ref->id);

    }
    else
    {
        throwError("Could not find reference road in elevation generation");
        return -1;
    }

    while(remaining.size() > 0)
    {
        elevationLinkInformation curEli = remaining.front();
        remaining.pop();
        
        if(curEli.parentLinkingPoint == 1) //element is successor
        {
            if(curEli.curRoad->predecessor.contactPoint == endType)
            {
                double elevationOffsetAtLink = curEli.parentRoad->getRelativeElevationAt(1) + curEli.parentRoad->elevationOffset;
                if(curEli.parentRoad->successor.contactPoint == startType)
                {
                    // |pre>  |suc>
                    curEli.curRoad->elevationOffset = elevationOffsetAtLink;
                }

                else if(curEli.parentRoad->successor.contactPoint == endType)
                {
                     // |pre>  |suc>
                    curEli.curRoad->elevationOffset = elevationOffsetAtLink - curEli.curRoad->getRelativeElevationAt(1);
                }

                curEli.curRoad->elevationOffset = curEli.parentRoad->elevationOffset + curEli.parentRoad->getRelativeElevationAt(1);
            }

            if(curEli.parentRoad->successor.contactPoint == startType)
            {
                
            }

            completededIds.push_back(curEli.curRoad->id);
            if(curEli.curRoad->successor.id > -1 && isIn(completededIds, curEli.curRoad->successor.id))
            {
                elevationLinkInformation newEli;
                road ssuc;
                findRoad(data.roads, ssuc, curEli.curRoad->successor.id);
                newEli.curRoad = &ssuc;
                newEli.parentRoad = curEli.curRoad;
                newEli.parentLinkingPoint = 1;
                
                remaining.push(newEli);
            }
        }

        else if(curEli.parentLinkingPoint == -1)
        {

        }

    }

    //----------------------------------------

    for(road &r:data.roads)
    {
        if (generateElevationProfile(r.elevationProfiles))
        {
            cerr << "ERR: error in generateElevationProfile" << endl;
            return 1;
        }
    }

}


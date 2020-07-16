#ifndef ROAD_NETWORK_H
#define ROAD_NETWORK_H

#include <string>
#include <vector>
#include <interface.h>

class roadNetwork
{
    std::string file;

    std::vector<road> roads;
    std::vector<junction> junctions;
    std::vector<control> controller;

    int nSignal = 0;
    int nSegment = 0;


};

#endif
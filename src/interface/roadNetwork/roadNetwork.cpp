
#include "roadNetwork.h"
#include "road.h"

        RoadNetwork::RoadNetwork(){}
        RoadNetwork::RoadNetwork(std::string f){
            file = f;
        }            
        
        RoadNetwork::~RoadNetwork(){}

        std::string RoadNetwork::getFile(){
            return file;
        }

        std::string RoadNetwork::getOutputFile(){
            return outputFile;
        }

         std::vector<Road>& RoadNetwork::getRoads(){
            return roads;
        }

        void RoadNetwork::pushRoad(Road r){
            roads.push_back(r);
        }

         std::vector<junction>& RoadNetwork::getJunctions(){
            return junctions;
        }

         void RoadNetwork::pushJunction(junction j){
            junctions.push_back(j);
        }

         std::vector<control>& RoadNetwork::getController(){
            return controller;
        }

         void RoadNetwork::pushControl(control c){
            controller.push_back(c);
        }

        void RoadNetwork::setFile(std::string file){
            this -> file  = file;
        }

        void RoadNetwork::setOutputFile(std::string file){
            this -> outputFile  = file;
        }


        int nSignal = 0;
        int nSegment = 0;


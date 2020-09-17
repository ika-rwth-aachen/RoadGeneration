#ifndef ROAD
#define ROAD

#include <string>
#include <vector>
#include "interface.h"
#include "pugixml.hpp"

class Road
{

    private:
        int _id = -1;
        int _inputId = -1;     // original id from input file
        std::string _inputPos = ""; // specifying part of original road from input file
        int _junction = -1;

        std::string _type = "town";
        std::string _classification = ""; // either 'main' or 'access'
        double _length = 0;

        link _predecessor;
        link _successor;

        std::vector<geometry> _geometries;
        std::vector<laneSection> _laneSections;
        std::vector<object> _objects;
        std::vector<sign> _signs;



        /**
         * @brief function computes first and last considered geometry in s interval 
         * 
         * @param roadIn        road input data
         * @param foundfirst    id of the first considerd geometry
         * @param foundlast     id of the last considerd geometry
         * @param sStart        start of the s interval
         * @param sEnd          end of the s interval
         * @return int          error code
         */
        int computeFirstLast(pugi::xml_node roadIn, int &foundfirst, int &foundlast, double &sStart, double &sEnd);

        /**
         * @brief function genetares the geometries of the reference line based on an s intervall
         * 
         * @param roadIn    road input data
         * @param r         road data containing the reference line information
         * @param sStart    start of the s interval
         * @param sEnd      end of the s interval
         * @return int      error code
         */
        int generateGeometries(pugi::xml_node roadIn, Road &r, double &sStart, double &sEnd);

        /**
         * @brief function shift the geometries of the reference line
         * 
         * @param r         road input data 
         * @param sStart    start of s interval 
         * @param sEnd      end of s interval 
         * @param s0        s0 is origin of reference line
         * @param x0        global x position of origin
         * @param y0        global y position of origin
         * @param phi0      global angle of origin
         * @return int      error code
         */
        int shiftGeometries(Road &r, double sStart, double sEnd, double s0, double x0, double y0, double phi0);

        /**
         * @brief function flips geometries of reference line
         * 
         * @param r     road data
         * @int         error code
         */
        int flipGeometries(Road &r);

        /**
         * @brief function adds lanes to the road structure
         * 
         * @param roadIn    road input data
         * @param r         road data
         * @param mode      defines the mode (flipped or not)
         * @return int      error code
         */
        int addLanes(pugi::xml_node roadIn, Road &r, int mode);

        /**
         * @brief function adds additional lane sections for changing lane structure
         *  
         * @param roadIn                road input data
         * @param r                     road data
         * @param automaticWidening     automatic widing input data
         * @return int                  error code
         */
        int addLaneSectionChanges(pugi::xml_node roadIn, Road &r, pugi::xml_node automaticWidening);

       
    public:

    std::string getType(){
        return _type;
    }

    std::string getClassification(){
        return _classification;
    }

    void setClassification(std::string classification){
        _classification = classification;
    }

    std::vector<sign>& getSigns(){
        return _signs;
    }

    std::vector<object>& getObjects(){
        return _objects;
    }

    double getLength(){
        return _length;
    }

    double setLength(double length){
        _length = length;
    }

    std::vector<laneSection>& getLaneSections(){
        return _laneSections;
    }

    void setLaneSection(std::vector<laneSection> ls){
        _laneSections = ls;
    }

    void setPredecessor(link pre){
        _predecessor = pre;
    }

     void setSuccessor(link suc){
        _successor = suc;
    }

    link& getPredecessor(){
        return _predecessor;
    }

    link& getSuccessor(){
        return _successor;
    }

    std::vector<geometry>& getGeometries(){
        return _geometries;
    }

    void setID(int id){
        _id = id;
    }
    int getID(){
        return _id;
    }

    void setInputID(int inputId){
        _inputId = inputId;
    }
    int getInputID(){
        return _inputId;
    }

    void setInputPos(std::string inputPos){
        _inputPos = inputPos;
    }
    std::string getInputPos(){
        return _inputPos;
    }

    void setJunction(int junction){
        _junction = junction;
    }
    int getJunction(){
        return _junction;
    }

     /**
         * @brief function builds a road based on the input data
         * 
         * @param road              road input data
         * @param r                 road data
         * @param sStart            starting s position
         * @param sEnd              ending s position
         * @param automaticWidening automaticWiding input data
         * @param s0                position of s where x0, y0, phi0 should be reached
         * @param x0                reference x position
         * @param y0                reference y position
         * @param phi0              reference angle
         * @return int              error code
         */
    int buildRoad(pugi::xml_node roadIn, double sStart, double sEnd, pugi::xml_node automaticWidening, double s0, double x0, double y0, double phi0);


    Road();
    ~Road();
};

#endif
#ifndef ROAD
#define ROAD

#include <string>
#include <vector>
#include "interface.h"

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

    public:

    std::string& getType(){
        return _type;
    }

    std::string& getClassification(){
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

    link& getPredecessor(){
        return _predecessor;
    }

    link& getSucessor(){
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



    Road();
    ~Road();
};

#endif
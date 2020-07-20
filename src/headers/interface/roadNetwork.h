

class RoadNetwork
{
    std::string _file;

    std::vector<road> _roads;
    std::vector<junction> _junctions;
    std::vector<control> _controller;

 

    public:
        RoadNetwork(){}

        RoadNetwork(std::string file);

        std::string getFile(){
            return _file;
        }

         std::vector<road>& getRoads(){
            return _roads;
        }

        void pushRoad(road r){
            _roads.push_back(r);
        }

         std::vector<junction> getJunctions(){
            return _junctions;
        }

         void pushJunction(junction j){
            _junctions.push_back(j);
        }

         std::vector<control> getController(){
            return _controller;
        }

         void pushControl(control c){
            _controller.push_back(c);
        }

        void setFile(std::string file){
            _file  = file;
        }

        int nSignal = 0;
        int nSegment = 0;

};


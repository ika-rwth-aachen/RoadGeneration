FROM amytabb/docker_ubuntu16_essentials


RUN mkdir -p /home/RoadGeneration
RUN apt-get update
RUN apt-get -y install libxerces-c-dev 
COPY . /RoadGeneration 

RUN chmod +x RoadGeneration/buildScriptDocker.sh



CMD cd RoadGeneration && ./buildScriptDocker.sh



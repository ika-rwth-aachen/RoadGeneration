cmake_minimum_required(VERSION 2.6)
project(road-generation)

find_package (XercesC REQUIRED)

# source code
include_directories(src)
include_directories(include)
include_directories(${CMAKE_SOURCE_DIR}/libs/pugixml-1.9/src)
include_directories (${XercesC_INCLUDE_DIR})

set(BIN_DIR ${CMAKE_SOURCE_DIR} CACHE PATH "BIN_DIR")
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${BIN_DIR})

add_executable("${PROJECT_NAME}"
    "${CMAKE_SOURCE_DIR}/src/main.cpp"
    "${CMAKE_SOURCE_DIR}/libs/pugixml-1.9/src/pugixml.cpp"
    "${CMAKE_SOURCE_DIR}/src/connection/closeRoadConnection.cpp"
    "${CMAKE_SOURCE_DIR}/src/connection/closeRoadNetwork.cpp"
    "${CMAKE_SOURCE_DIR}/src/connection/linkSegments.cpp"
    "${CMAKE_SOURCE_DIR}/src/generation/addLaneSections.cpp"
    "${CMAKE_SOURCE_DIR}/src/generation/addObjects.cpp"
    "${CMAKE_SOURCE_DIR}/src/generation/buildRoad.cpp"
    "${CMAKE_SOURCE_DIR}/src/generation/connectingRoad.cpp"
    "${CMAKE_SOURCE_DIR}/src/generation/createLaneConnection.cpp"
    "${CMAKE_SOURCE_DIR}/src/generation/createRoadConnection.cpp"
    "${CMAKE_SOURCE_DIR}/src/interface/roadNetwork/roadNetwork.cpp"
    "${CMAKE_SOURCE_DIR}/src/interface/roadNetwork/buildSegments.cpp"
    "${CMAKE_SOURCE_DIR}/src/utils/xml.cpp"
    "${CMAKE_SOURCE_DIR}/src/utils/helper.cpp"
    "${CMAKE_SOURCE_DIR}/src/utils/curve.cpp"
    "${CMAKE_SOURCE_DIR}/src/utils/geometries.cpp"
)

add_executable("test/cmp"
    "${CMAKE_SOURCE_DIR}/tools/cmp/cmp.cpp"
    "${CMAKE_SOURCE_DIR}/libs/pugixml-1.9/src/pugixml.cpp"
)
add_definitions(-DPROJ_DIR=\"${PROJECT_SOURCE_DIR}\")
add_definitions(-D_GLIBCXX_USE_CXX11_ABI=0)


target_link_libraries("${PROJECT_NAME}" ${XercesC_LIBRARIES})


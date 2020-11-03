cmake_minimum_required(VERSION 2.6)
project(road-generation)

find_package (XercesC REQUIRED)

# source code
include_directories(src)
include_directories(${CMAKE_SOURCE_DIR}/libs/pugixml-1.9/src)
include_directories (${XercesC_INCLUDE_DIR})

set(BIN_DIR ${CMAKE_SOURCE_DIR} CACHE PATH "BIN_DIR")
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${BIN_DIR})

add_library("roadgen" SHARED
    "${CMAKE_SOURCE_DIR}/src/export.cpp"
    "${CMAKE_SOURCE_DIR}/libs/pugixml-1.9/src/pugixml.cpp"
)

target_link_libraries("roadgen" ${XercesC_LIBRARIES})


add_executable("${PROJECT_NAME}"
    "${CMAKE_SOURCE_DIR}/src/main.cpp"
    "${CMAKE_SOURCE_DIR}/libs/pugixml-1.9/src/pugixml.cpp"
)

add_definitions(-DPROJ_DIR=\"${PROJECT_SOURCE_DIR}\")

target_link_libraries("${PROJECT_NAME}" ${XercesC_LIBRARIES})

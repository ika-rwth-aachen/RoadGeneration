cmake_minimum_required(VERSION 2.6)
project(road-generation)

find_package (XercesC REQUIRED)

# source code
include_directories(src)
include_directories(${CMAKE_SOURCE_DIR}/libs/pugixml/src)
include_directories (${XercesC_INCLUDE_DIR})

set(BIN_DIR ${CMAKE_SOURCE_DIR} CACHE PATH "BIN_DIR")
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${BIN_DIR})

add_library("${PROJECT_NAME}_lib" SHARED
    "${CMAKE_SOURCE_DIR}/src/libfiles/export.cpp"
    "${CMAKE_SOURCE_DIR}/libs/pugixml/src/pugixml.cpp"
)


add_executable("${PROJECT_NAME}"
    "${CMAKE_SOURCE_DIR}/src/executable/main.cpp"
    "${CMAKE_SOURCE_DIR}/src/executable/helperExec.cpp"
)

target_link_libraries("${PROJECT_NAME}_lib" ${XercesC_LIBRARIES})
target_link_libraries("${PROJECT_NAME}" ${PROJECT_NAME}_lib)

add_definitions(-DPROJ_DIR=\"${PROJECT_SOURCE_DIR}\")



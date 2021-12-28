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

add_library("${PROJECT_NAME}" SHARED
    "${CMAKE_SOURCE_DIR}/src/libfiles/export.cpp"
    "${CMAKE_SOURCE_DIR}/libs/pugixml/src/pugixml.cpp"
)

add_executable("${PROJECT_NAME}_exec"
    "${CMAKE_SOURCE_DIR}/src/executable/main.cpp"
    "${CMAKE_SOURCE_DIR}/src/executable/helperExec.cpp"
)

target_link_libraries("${PROJECT_NAME}" ${XercesC_LIBRARIES})
target_link_libraries("${PROJECT_NAME}_exec" ${PROJECT_NAME})

add_definitions(-DPROJ_DIR=\"${PROJECT_SOURCE_DIR}\")


find_package(Doxygen)
if (DOXYGEN_FOUND)
    # set input and output files
    set(DOXYGEN_IN ${CMAKE_CURRENT_SOURCE_DIR}/../docs/Doxyfile.in)
    set(DOXYGEN_OUT ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile)

    # request to configure the file
    configure_file(${DOXYGEN_IN} ${DOXYGEN_OUT} @ONLY)
    message("Doxygen build started")

    # note the option ALL which allows to build the docs together with the application
    add_custom_target( doc_doxygen ALL
        COMMAND ${DOXYGEN_EXECUTABLE} ${DOXYGEN_OUT}
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
        COMMENT "Generating API documentation with Doxygen"
        VERBATIM )
else (DOXYGEN_FOUND)
  message("Doxygen need to be installed to generate the doxygen documentation")
endif (DOXYGEN_FOUND)
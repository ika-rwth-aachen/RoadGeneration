cmake_minimum_required(VERSION 2.6)
project(roadGeneration)

find_package (XercesC REQUIRED)

# source code
include_directories(src)
include_directories(${CMAKE_SOURCE_DIR}/libs/pugixml-1.9/src)
include_directories (${XercesC_INCLUDE_DIR})

set(BIN_DIR ${CMAKE_SOURCE_DIR}/bin CACHE PATH "BIN_DIR")
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${BIN_DIR})

file(GLOB_RECURSE DRIVER_SRC
    "${CMAKE_SOURCE_DIR}/src/*.h"
    "${CMAKE_SOURCE_DIR}/src/*.cpp"
    "${CMAKE_SOURCE_DIR}/libs/pugixml-1.9/src/*"
)

add_executable(roadGeneration
        ${DRIVER_SRC}
    )

add_definitions(${GCC_COVERAGE_COMPILE_FLAGS})
add_definitions(-DPROJ_DIR=\"${PROJECT_SOURCE_DIR}\")

target_link_libraries(roadGeneration ${XercesC_LIBRARIES})


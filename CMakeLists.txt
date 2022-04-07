cmake_minimum_required(VERSION 2.6)
project(road-generation)
find_package (XercesC REQUIRED)

include_directories(src)
include_directories (${XercesC_INCLUDE_DIR})

set(BIN_DIR ${CMAKE_SOURCE_DIR} CACHE PATH "BIN_DIR")
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${BIN_DIR})

add_definitions(-DPROJ_DIR=\"${PROJECT_SOURCE_DIR}\")

option(CREATE_DOXYGEN_TARGET "Creates the doxygen documentation if set." OFF)

# doxygen ---------------------------------------------------------------------------------
if (CREATE_DOXYGEN_TARGET)
    message("-- Generation of doxygen target enabled")
    find_package(Doxygen)
    if (DOXYGEN_FOUND)

        # create doc directory
        file(MAKE_DIRECTORY ${PROJECT_SOURCE_DIR}/docs)

        # settings
        set(DOXYGEN_GENERATE_HTML YES)
        set(DOXYGEN_GENERATE_MAN YES)
        set(DOXYGEN_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/docs)
        set(DOXYGEN_USE_MDFILE_AS_MAINPAGE README.md) 

        message("Doxygen build started")

         doxygen_add_docs(
                doxygen
                ${PROJECT_SOURCE_DIR} README.md #this is not optimal, since the project uses an rst file at the moment
                COMMENT "Generate man pages"
        )

    else (DOXYGEN_FOUND)
        message("Doxygen need to be installed to generate the doxygen documentation")
    endif (DOXYGEN_FOUND)
endif (CREATE_DOXYGEN_TARGET)

# project code ---------------------------------------------------------------------------------

add_library("${PROJECT_NAME}" SHARED
    "${CMAKE_SOURCE_DIR}/src/libfiles/export.cpp"
)


add_executable("${PROJECT_NAME}_executable"
    "${CMAKE_SOURCE_DIR}/src/executable/main.cpp"
    "${CMAKE_SOURCE_DIR}/src/executable/helperExec.cpp"
)


target_link_libraries("${PROJECT_NAME}" ${XercesC_LIBRARIES})
target_link_libraries("${PROJECT_NAME}_executable" ${PROJECT_NAME})


add_definitions(-DPROJ_DIR=\"${PROJECT_SOURCE_DIR}\")



cmake_minimum_required(VERSION 2.8.7)
project(TSDF2CUBES)

set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)
#set(LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/lib)
set(CMAKE_BUILD_TYPE "Release")

add_compile_options(-std=c++11)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/src/src/ThirdParty/lib3mf/)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/src/src/ThirdParty/lib3mf/Model/COM/)

OPTION(USE_3MF "Use 3MF Backend to alow output 3mf file" OFF)

###########################################################################################
IF(USE_3MF)
    ADD_SUBDIRECTORY(Tool3mf)
    ADD_DEFINITIONS(-DUSE_3MF)
ENDIF()

ADD_SUBDIRECTORY(app)

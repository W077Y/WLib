# CMakeList.txt : CMake project for tst_WLib, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.15...3.17)

include( "${CMAKE_CURRENT_LIST_DIR}/project-meta-info.in")

project(WLib
        VERSION ${project_version}
        DESCRIPTION ${project_description}
        HOMEPAGE_URL ${project_homepage_url}
        LANGUAGES CXX )



include(CTest)
add_subdirectory("./WLib")
add_subdirectory("./examples")
add_subdirectory("./tst_WLib")


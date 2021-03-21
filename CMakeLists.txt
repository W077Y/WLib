# CMakeList.txt : CMake project for tst_WLib, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.15)

project(WLib)



include(CTest)
add_subdirectory("./WLib")
add_subdirectory("./examples")
add_subdirectory("./tst_WLib")


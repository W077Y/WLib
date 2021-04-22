# CMakeList.txt : CMake project for tst_WLib, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.15)


add_library(WLib_ObserverPattern STATIC)

target_sources(WLib_ObserverPattern
	PUBLIC "${CMAKE_CURRENT_LIST_DIR}/inc/ObserverPattern.h"
	PRIVATE "${CMAKE_CURRENT_LIST_DIR}/src/ObserverPattern.cpp"
	)


target_include_directories(
	WLib_ObserverPattern
	PUBLIC "${CMAKE_CURRENT_LIST_DIR}/inc"
	PRIVATE "${CMAKE_CURRENT_LIST_DIR}/src")




target_compile_features(WLib_ObserverPattern PUBLIC cxx_std_17)
if (MSVC)
	string(REGEX REPLACE "/W[1-4]" "" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
	string(APPEND CMAKE_CXX_FLAGS " /W4 /WX")
	string(APPEND CMAKE_CXX_FLAGS " /std:c++17")
else()
	target_compile_options(WLib_ObserverPattern PRIVATE -Wall -Wextra -pedantic -Werror -O3)
endif()

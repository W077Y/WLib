# CMakeList.txt : CMake project for tst_WLib, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.15)


add_library(
	WLib_Serializer STATIC
	"inc/WLib_Serializer.h"
	"src/WLib_Serializer.cpp")


target_include_directories(
	WLib_Serializer
	PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/inc"
	PRIVATE_HEADER "${CMAKE_CURRENT_SOURCE_DIR}/src")


target_compile_features(WLib_Serializer PUBLIC cxx_std_17)
if(MSVC)
  string(REGEX REPLACE "/W[1-3]" "/W4" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
else()
  target_compile_options(WLib_Serializer PUBLIC -Wall -Wextra -pedantic -Werror -O3)
endif()




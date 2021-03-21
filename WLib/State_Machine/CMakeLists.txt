# CMakeList.txt : CMake project for tst_WLib, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.15)


add_library(
	WLib_State_Machine	STATIC
	"inc/WLib_State_Machine.h"

	"src/WLib_State_Machine.cpp")

target_include_directories(
	WLib_State_Machine
	PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/inc"
	PRIVATE_HEADER "${CMAKE_CURRENT_SOURCE_DIR}/src")


target_compile_features(WLib_State_Machine PUBLIC cxx_std_17)
if(MSVC)
  string(REGEX REPLACE "/W[1-3]" "/W4" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
else()
  target_compile_options(WLib_State_Machine PUBLIC -Wall -Wextra -pedantic -Werror)
endif()


# TODO: Add tests and install targets if needed.

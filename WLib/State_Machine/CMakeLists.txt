# CMakeList.txt : CMake project for tst_WLib, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.15)


add_library(WLib_State_Machine	STATIC)
target_sources(WLib_State_Machine
	PRIVATE "src/WLib_State_Machine.cpp")

target_include_directories(
	WLib_State_Machine
	PUBLIC "${CMAKE_CURRENT_LIST_DIR}/inc"
	PRIVATE_HEADER "${CMAKE_CURRENT_LIST_DIR}/src")


target_compile_features(WLib_State_Machine PUBLIC cxx_std_17)
if(MSVC)
  string(REGEX REPLACE "/W[1-4]" "" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
  string(APPEND CMAKE_CXX_FLAGS " /W4 /WX")
  string(APPEND CMAKE_CXX_FLAGS " /std:c++17")
else()
  target_compile_options(WLib_State_Machine PUBLIC -Wall -Wextra -pedantic -Werror -O3)
endif()


# TODO: Add tests and install targets if needed.

# CMakeList.txt : CMake project for tst_WLib, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.15)


add_library(
	WLib_CRC STATIC
	"inc/WLib_CRC.h"
	
	"src/crc_16_ccitt_false.cpp"
	"src/crc_32.cpp"
	"src/crc_64_go_iso.cpp"
	"src/crc_8.cpp"	)


target_include_directories(
	WLib_CRC
	PUBLIC "${CMAKE_CURRENT_LIST_DIR}/inc"
	PRIVATE_HEADER "${CMAKE_CURRENT_LIST_DIR}/src")




target_compile_features(WLib_CRC PUBLIC cxx_std_17)
if(MSVC)
  string(REGEX REPLACE "/W[1-4]" "" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
  string(APPEND CMAKE_CXX_FLAGS " /W4 /WX")
  string(APPEND CMAKE_CXX_FLAGS " /std:c++17")
else()
  target_compile_options(WLib_CRC PUBLIC -Wall -Wextra -pedantic -Werror -O3)
endif()




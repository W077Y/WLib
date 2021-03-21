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
	PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/inc"
	PRIVATE_HEADER "${CMAKE_CURRENT_SOURCE_DIR}/src")




target_compile_features(WLib_CRC PUBLIC cxx_std_17)
if(MSVC)
  string(REGEX REPLACE "/W[1-3]" "/W4" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
else()
  target_compile_options(WLib_CRC PUBLIC -Wall -Wextra -pedantic -Werror)
endif()




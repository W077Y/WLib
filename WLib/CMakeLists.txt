# CMakeList.txt : CMake project for tst_WLib, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.15)



add_library(
	WLib STATIC
	"WLib.cpp"	)


target_compile_features(WLib PUBLIC cxx_std_17)

if(MSVC)
  string(REGEX REPLACE "/W[1-3]" "/W4" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
else()
  target_compile_options(WLib PUBLIC -Wall -Wextra -pedantic -Werror)
endif()

add_subdirectory("CRC")
add_subdirectory("State_Machine")



target_link_libraries(WLib PUBLIC WLib_CRC)
target_link_libraries(WLib PUBLIC WLib_State_Machine)
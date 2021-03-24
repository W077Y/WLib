# CMakeList.txt : CMake project for tst_WLib, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.15)



add_library(
	WLib STATIC
	"WLib.cpp"	)


target_compile_features(WLib PUBLIC cxx_std_17)

if(MSVC)
  string(REGEX REPLACE "/W[1-3]" "" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
  string(APPEND CMAKE_CXX_FLAGS " /W4")
else()
  target_compile_options(WLib PUBLIC -Wall -Wextra -pedantic -Werror -O3)
endif()

add_subdirectory("CRC")
add_subdirectory("State_Machine")
add_subdirectory("Serializer")



target_link_libraries(WLib PUBLIC WLib_CRC)
target_link_libraries(WLib PUBLIC WLib_State_Machine)
target_link_libraries(WLib PUBLIC WLib_Serializer)
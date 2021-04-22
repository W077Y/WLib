# CMakeList.txt : CMake project for tst_WLib, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.15)

add_executable (
	tst_WLib

	"tst_WLib.h"
	"tst_WLib.cpp"

	"CRC/tst_crc8.cpp"
	"CRC/tst_crc16_ccitt_false.cpp"
	"CRC/tst_crc32.cpp"
	"CRC/tst_crc64_go_iso.cpp"

	"State_Machine/tst_Engine.cpp"
	"State_Machine/tst_State_Base.cpp"
	"State_Machine/tst_Transition.cpp"
	"State_Machine/tst_StateMachineClass.cpp"

	"Serializer/tst_serializer.cpp"
	
	"ObserverPattern/tst_ObserverPattern.cpp"
	)
target_include_directories(tst_WLib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_compile_features(tst_WLib PUBLIC cxx_std_17)
if(MSVC)
  string(REGEX REPLACE "/W[1-3]" "" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
  string(REGEX REPLACE "/W[1-4]" "" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
  string(APPEND CMAKE_CXX_FLAGS " /W4 /WX")
  string(APPEND CMAKE_CXX_FLAGS " /std:c++17")
else()
  target_compile_options(tst_WLib PUBLIC -Wall -Wextra -pedantic -Werror -O3)
endif()

MESSAGE(STATUS "Current CMAKE_CXX_FLAGS: " ${CMAKE_CXX_FLAGS})


target_link_libraries(tst_WLib LINK_PUBLIC WLib)

add_dependencies(tst_WLib WLib)

add_dependencies(tst_WLib exa_usecase_crc16_ccitt_false)
add_dependencies(tst_WLib exa_usecase_crc32)

add_dependencies(tst_WLib exa_usecase_state_machine)


enable_testing()
add_test(NAME test_WLib COMMAND tst_WLib)



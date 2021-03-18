# CMakeList.txt : CMake project for tst_WLib, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.8)


file(GLOB tst_files "State_Machine/*.cpp" "CRC/*.cpp")

add_executable (tst_WLib "tst_WLib.cpp" "tst_WLib.h" ${tst_files})

target_include_directories(tst_WLib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(tst_WLib LINK_PUBLIC WLib)

add_dependencies(tst_WLib WLib)
add_dependencies(tst_WLib exa_usecase_crc16_ccitt_false)
add_dependencies(tst_WLib exa_usecase_crc32)

enable_testing()
add_test(NAME test_WLib COMMAND tst_WLib)



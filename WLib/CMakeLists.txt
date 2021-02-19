# CMakeList.txt : CMake project for tst_WLib, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.8)


file(GLOB header_files "State_Machine/*.h")

# Add source to this project's executable.
add_library(WLib STATIC "WLib.cpp" ${header_files})

target_compile_features(WLib PUBLIC cxx_std_17)

target_include_directories(WLib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

if(MSVC)
  string(REGEX REPLACE "/W[1-3]" "/W4" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
else()
  target_compile_options(WLib PUBLIC -Wall -Wextra -pedantic -Werror)
endif()

# TODO: Add tests and install targets if needed.

# CMakeList.txt : CMake project for tst_WLib, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.8)


add_executable(exa_usecase_state_machine "usecase_state_machine.cpp")
target_compile_features(exa_usecase_state_machine PUBLIC cxx_std_17)
target_link_libraries(exa_usecase_state_machine LINK_PUBLIC WLib)

add_test(test_exa_usecase_state_machine exa_usecase_state_machine)
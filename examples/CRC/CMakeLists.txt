# CMakeList.txt : CMake project for tst_WLib, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.8)


add_executable(exa_usecase_crc32 "usecase_crc32.cpp")
target_compile_features(exa_usecase_crc32 PUBLIC cxx_std_17)
target_link_libraries(exa_usecase_crc32 LINK_PUBLIC WLib)


add_executable(exa_usecase_crc16_ccitt_false "usecase_crc16_ccitt_false.cpp")
target_compile_features(exa_usecase_crc16_ccitt_false PUBLIC cxx_std_17)
target_link_libraries(exa_usecase_crc16_ccitt_false LINK_PUBLIC WLib)



add_test(test_exa_usecase_crc32 exa_usecase_crc32)
add_test(test_exa_usecase_crc16_ccitt_false exa_usecase_crc16_ccitt_false)
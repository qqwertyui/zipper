cmake_minimum_required(VERSION 3.18.4)
project(Zipper VERSION 0.2)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED true)

add_subdirectory(src)
add_subdirectory(tests)

enable_testing()
add_test(NAME test1 COMMAND zipper_tests)
cmake_minimum_required(VERSION 3.16.3)
project(Zipper VERSION 0.4)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED true)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
add_custom_target(
  clang-tidy
  COMMAND python3 ${CMAKE_SOURCE_DIR}/scripts/run-clang-tidy.py -quiet
)

add_subdirectory(src)

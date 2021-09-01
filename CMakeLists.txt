cmake_minimum_required(VERSION 3.19.4)
project(Zipper VERSION 0.2)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

file(GLOB SRC_FILES
    "src/*.cpp"
)
add_executable(zipper)

target_sources(zipper PUBLIC ${SRC_FILES})
include_directories(include)
configure_file(include/Version.hpp.in ${CMAKE_SOURCE_DIR}/include/Version.hpp)

# link libraries
target_link_libraries(zipper PUBLIC gflags_static)
target_link_libraries(zipper PUBLIC z)
target_link_libraries(zipper PUBLIC shlwapi)
target_link_libraries(zipper PUBLIC gcc)
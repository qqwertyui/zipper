cmake_minimum_required(VERSION 2.8.12)
project(zipper)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

file(GLOB SRC_FILES
    "src/*.cpp"
)
add_executable(zipper)

target_sources(zipper PUBLIC ${SRC_FILES})
include_directories(include)

# link libraries
target_link_libraries(zipper PUBLIC gflags_static)
target_link_libraries(zipper PUBLIC z)
target_link_libraries(zipper PUBLIC shlwapi)
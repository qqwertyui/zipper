file(GLOB SRC_FILES "*.cpp")
list(FILTER SRC_FILES EXCLUDE REGEX "App.cpp")

add_library(zipcore STATIC ${SRC_FILES})
target_include_directories(zipcore PUBLIC ${CMAKE_SOURCE_DIR}/include/)
configure_file(${CMAKE_SOURCE_DIR}/include/Version.hpp.in ${CMAKE_SOURCE_DIR}/include/Version.hpp)
target_link_libraries(zipcore PUBLIC z)
target_link_libraries(zipcore PUBLIC shlwapi)

add_executable(zipper ${CMAKE_SOURCE_DIR}/src/App.cpp)
target_link_libraries(zipper PUBLIC zipcore)
target_link_libraries(zipper PUBLIC gflags_static)
target_link_options(zipper PUBLIC -static)
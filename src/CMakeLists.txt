set(
  APP_SOURCES
  "Main.cpp"
)

add_subdirectory(zipcxx)
add_subdirectory(tools)
add_executable(zipper ${APP_SOURCES})
configure_file(Version.hpp.in Version.hpp)

find_package(gflags REQUIRED)

target_link_libraries(zipper PUBLIC zipcxx)
target_link_libraries(zipper PUBLIC gflags)

if(WIN32)
  target_link_libraries(zipper PUBLIC shlwapi)
endif(WIN32)
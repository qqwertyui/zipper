set(
  APP_SOURCES
  "Main.cpp"
)

add_subdirectory(zipcxx)
add_subdirectory(tools)
add_executable(zipper ${APP_SOURCES})
configure_file(Version.hpp.in Version.hpp)

target_link_libraries(zipper PUBLIC zipcxx)
target_link_libraries(zipper PUBLIC gflags_nothreads)
target_link_options(zipper PUBLIC -static)

if(WIN32)
    target_link_libraries(zipper PUBLIC shlwapi)
endif(WIN32)
set(
  UTILS_SOURCES
  "Log.cpp"
  "Utils.cpp"
)

add_library(utils STATIC ${UTILS_SOURCES})
find_package(ZLIB REQUIRED)
find_package(fmt REQUIRED)

target_link_libraries(utils PUBLIC ZLIB::ZLIB)
target_link_libraries(utils PUBLIC fmt::fmt)
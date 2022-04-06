set(
  UTILS_SOURCES
  "Log.cpp"
  "Utils.cpp"
)

add_library(utils STATIC ${UTILS_SOURCES})
target_link_libraries(utils PUBLIC z)
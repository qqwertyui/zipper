set(
  ZIPCXX_SOURCES 
  "Zip.cpp"
  "ZipEntry.cpp"
  "ZipStructs.cpp"
)

set(ZIPCXX_ROOT ${CMAKE_CURRENT_SOURCE_DIR})
include_directories(${ZIPCXX_ROOT})

add_subdirectory(utils)
add_subdirectory(ut)

add_library(zipcxx STATIC ${ZIPCXX_SOURCES})
target_link_libraries(zipcxx PUBLIC utils)
set(
  ZIPVIEWER_SOURCES 
  "${CMAKE_CURRENT_SOURCE_DIR}/main.cpp"
)

add_executable(zipviewer ${ZIPVIEWER_SOURCES})
target_link_libraries(zipviewer PUBLIC zipcxx)
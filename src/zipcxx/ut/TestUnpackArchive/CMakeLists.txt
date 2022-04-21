set(TestUnpackArchiveLibraries
  "zipcxx"
)
set(TestUnpackArchiveSamples
  "${CMAKE_CURRENT_SOURCE_DIR}/test1.zip"
  "${CMAKE_CURRENT_SOURCE_DIR}/test2.zip"
  "${CMAKE_CURRENT_SOURCE_DIR}/test3.zip"
)
add_ut(
  TEST_NAME "TestUnpackArchive"
  LIBS "${TestUnpackArchiveLibraries}"
  SAMPLES "${TestUnpackArchiveSamples}"
)
set(TestInvalidChecksumLibraries
  "zipcxx"
)
set(TestInvalidChecksumSamples
  "${CMAKE_CURRENT_SOURCE_DIR}/validChecksum.zip"
  "${CMAKE_CURRENT_SOURCE_DIR}/invalidChecksum.zip"
  "${CMAKE_CURRENT_SOURCE_DIR}/invalidChecksumExtract.zip"
)
add_ut(
  TEST_NAME "TestInvalidChecksum"
  LIBS "${TestInvalidChecksumLibraries}"
  SAMPLES "${TestInvalidChecksumSamples}"
)
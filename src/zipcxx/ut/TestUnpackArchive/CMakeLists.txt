set(TestUnpackArchiveSources
  "${ZIPCXX_ROOT}/Zip.cpp"
  "${ZIPCXX_ROOT}/ZipEntry.cpp"
  "${ZIPCXX_ROOT}/ZipStructs.cpp"
)
set(TestUnpackArchiveSamples
  "${CMAKE_CURRENT_SOURCE_DIR}/test1.zip"
  "${CMAKE_CURRENT_SOURCE_DIR}/test2.zip"
  "${CMAKE_CURRENT_SOURCE_DIR}/test3.zip"
)
set(TestUnpackArchiveLibraries
  "utils"
)
add_ut(TestUnpackArchive "${TestUnpackArchiveSources}" "${TestUnpackArchiveLibraries}" "${TestUnpackArchiveSamples}")
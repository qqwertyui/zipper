set(TemplateTestSources
  #"${CMAKE_CURRENT_SOURCE_DIR}/AdditionalFile.cpp"
)
set(TemplateTestLibraries
  #"zipcxx"
)
set(TemplateTestSamples
  #"${CMAKE_CURRENT_SOURCE_DIR}/test1.zip"
  #"${COMMON_SAMPLES}/sample1.zip"
)
add_ut(
  TEST_NAME "TemplateTest"
  SRCS "${TemplateTestSources}"
  LIBS "${TemplateTestLibraries}"
  SAMPLES "${TemplateTestSamples}"
)
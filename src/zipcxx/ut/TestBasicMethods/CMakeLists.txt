set(TestBasicMethodsLibraries
  "zipcxx"
)
set(TestBasicMethodsSamples
  "${COMMON_SAMPLES}/sample1.zip"
)
add_ut(
  TEST_NAME "TestBasicMethods"
  LIBS "${TestBasicMethodsLibraries}"
  SAMPLES "${TestBasicMethodsSamples}"
)
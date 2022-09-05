include(CMakeParseArguments)
find_package(GTest REQUIRED)

function(add_ut)
  cmake_parse_arguments(
    ARG
    ""
    "TEST_NAME"
    "SRCS;LIBS;SAMPLES"
    ${ARGN}
  )
  add_executable(${ARG_TEST_NAME} ${ARG_TEST_NAME}.cpp ${ARG_SRCS})
  target_link_libraries(${ARG_TEST_NAME} PUBLIC GTest::GTest pthread ${ARG_LIBS})
  
  foreach(current_sample ${ARG_SAMPLES})
  add_custom_command(
      TARGET ${ARG_TEST_NAME}
      POST_BUILD
      COMMAND ${CMAKE_COMMAND}
      ARGS -E copy ${current_sample} ${CMAKE_CURRENT_BINARY_DIR}
  )
  endforeach(current_sample)
endfunction(add_ut)

set(COMMON_SAMPLES "${CMAKE_CURRENT_SOURCE_DIR}/common_samples")

#add_subdirectory(TemplateTest)
add_subdirectory(TestUnpackArchive)
add_subdirectory(TestBasicMethods)
add_subdirectory(TestInvalidChecksum)
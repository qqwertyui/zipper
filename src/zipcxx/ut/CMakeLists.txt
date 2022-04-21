function(add_ut target_name sources libraries input_samples)
  add_executable(${target_name} ${target_name}.cpp ${sources})
  target_link_libraries(${target_name} PUBLIC gtest gtest_main pthread ${libraries})
  
  foreach(current_sample ${input_samples})
  add_custom_command(
      TARGET ${target_name}
      POST_BUILD
      COMMAND ${CMAKE_COMMAND}
      ARGS -E copy ${current_sample} ${CMAKE_CURRENT_BINARY_DIR}
  )
  endforeach(current_sample)
endfunction()

set(COMMON_SAMPLES "${CMAKE_CURRENT_SOURCE_DIR}/common_samples")

#add_subdirectory(TemplateTest)
add_subdirectory(TestUnpackArchive)
add_subdirectory(TestBasicMethods)
add_subdirectory(TestInvalidChecksum)
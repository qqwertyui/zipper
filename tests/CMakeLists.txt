file(GLOB SRC_TEST_FILES "*.cpp")
file(GLOB ZIP_TEST_SAMPLES "test*.zip")

add_executable(zipper_tests ${SRC_TEST_FILES})
target_link_libraries(zipper_tests PUBLIC zipcore)
target_link_libraries(zipper_tests PUBLIC gtest)
target_link_libraries(zipper_tests PUBLIC gtest_main)

foreach(current_zip ${ZIP_TEST_SAMPLES})
    add_custom_command(
        TARGET zipper_tests
        POST_BUILD
        COMMAND ${CMAKE_COMMAND}
        ARGS -E copy ${current_zip} .
    )
endforeach(current_zip)
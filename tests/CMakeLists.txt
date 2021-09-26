file(GLOB SRC_TEST_FILES "*.cpp")

add_executable(zipper_tests ${SRC_TEST_FILES})
target_link_libraries(zipper_tests PUBLIC zipcore)
target_link_libraries(zipper_tests PUBLIC gtest)
target_link_libraries(zipper_tests PUBLIC gtest_main)

file(COPY test1.zip DESTINATION .)
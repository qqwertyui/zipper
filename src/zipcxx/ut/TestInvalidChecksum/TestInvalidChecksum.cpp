#include <Zip.hpp>
#include <gtest/gtest.h>
#include <iostream>

using namespace zipcxx;

TEST(TestInvalidChecksum, testValidChecksum) {
  try {
    Zip{"validChecksum.zip"};
  } catch (const std::exception &e) {
    std::cout << e.what() << std::endl;
    EXPECT_TRUE(false);
  }
}

TEST(TestInvalidChecksum, testInvalidChecksum) {
  try {
    Zip{"invalidChecksum.zip"};
    EXPECT_TRUE(false);
  } catch (const std::exception &e) {
    std::cout << e.what() << std::endl;
  }
}

TEST(TestInvalidChecksum, testInvalidChecksumExtract) {
  Zip archive{"invalidChecksumExtract.zip"};
  try {
    archive.extract("asd.txt");
    EXPECT_TRUE(false);
  } catch (const std::exception &e) {
    std::cout << e.what() << std::endl;
  }
}

int main(int argc, char **argv) {
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
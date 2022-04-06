#include <gtest/gtest.h>

#include <Zip.hpp>

class TestBasicMethods : public ::testing::Test {
public:
  void SetUp() override {}
  void TearDown() override {}

protected:
  zipcxx::Zip archive{"sample1.zip"};
};

TEST_F(TestBasicMethods, testIfFilesAreValid) {
  std::vector<std::string> expectedFilenames{"snapshots.img"};
  std::vector<std::string> outputFilenames{archive.getFilenames()};

  bool verdict{std::equal(expectedFilenames.begin(), expectedFilenames.end(),
                          outputFilenames.begin(), outputFilenames.end())};
  EXPECT_TRUE(verdict);
}

int main(int argc, char **argv) {
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
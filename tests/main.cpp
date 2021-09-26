#include <gtest/gtest.h>
#include <string>
#include <vector>
#include "Zip.hpp"

TEST(UnzipArchive, unzip1) {
    std::string archive1 = "test1.zip";
    std::vector<std::string> expected = { "cryptest_bds.bdsgroup", "cryptest_bds.bdsproj", "cryptest_bds.bpf", "cryptlib_bds.bdsproj", "cryptlib_bds.cpp" };
    
    Zip z(archive1);
    for(ZipEntry *e : z.get_entries()) {
        const std::string &current = e->get_name();
        bool found = false;
        for(std::string file : expected) {
            if(file.compare(current) == 0) {
                found = true;
                break;
            }
        }
        if(found == false) {
            printf("COULDN'T FIND: %s\n", current.c_str());
            FAIL();
        }
    }
}

int main(int argc, char **argv) {
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
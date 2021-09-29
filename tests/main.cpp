#include <gtest/gtest.h>
#include <string>
#include <vector>
#include "Zip.hpp"

TEST(UnzipArchive, unzip1) {
    std::string archive = "test1.zip";
    std::vector<std::string> expected = { 
        "cryptest_bds.bdsgroup", 
        "cryptest_bds.bdsproj", 
        "cryptest_bds.bpf", 
        "cryptlib_bds.bdsproj", 
        "cryptlib_bds.cpp" 
    };
    
    Zip z(archive);
    bool ok = false;
    if(expected.size() != z.get_entries().size()) {
        FAIL();
    }
    for(ZipEntry *e : z.get_entries()) {
        const std::string &current = e->get_name();
        for(std::string file : expected) {
            if(file.compare(current) == 0) {
                ok = true;
                break;
            }
        }
        if(ok == false) {
            FAIL();
        }
    }
}


TEST(UnzipArchive, unzip2) {
    std::string archive = "test2.zip";
    std::vector<std::string> expected = { 
        "ligowave/",
        "ligowave/LIGO-802DOT11-EXT-MIB.mib", 
        "ligowave/LIGO-ATHDRV-STATS-MIB.mib", 
        "ligowave/LIGO-GENERIC-MIB.mib", 
        "ligowave/LIGO-RADIO3-DRV-MIB.mib", 
        "ligowave/LIGOWAVE-MIB.mib",
        "ligowave/LIGO-WIRELESS-MIB.mib",
        "ligowave/LIGO-W-JET-MIB.mib",
        "ligowave/LIGO-W-JET-MIMO-MIB.mib"
    };
    
    Zip z(archive);
    bool ok = false;
    if(expected.size() != z.get_entries().size()) {
        FAIL();
    }
    for(ZipEntry *e : z.get_entries()) {
        const std::string &current = e->get_name();
        for(std::string file : expected) {
            if(file.compare(current) == 0) {
                ok = true;
                break;
            }
        }
        if(ok == false) {
            FAIL();
        }
    }
}


int main(int argc, char **argv) {
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
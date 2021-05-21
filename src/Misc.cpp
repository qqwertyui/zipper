#include "Misc.hpp"
#include <cstdio>
#include <cstring>

namespace Misc {
	void help() {
		puts(
		"Usage: zipper.exe <zip_archive> [options]\n"
		"-x, --extract	\textracts [filename]\n"
		"\t-a \textract all files\n"
		"-v, --version	\tprints current zipper version\n"
		"-h, --help	\tprints this message"
		);
	}
	
	void version() {
		printf("zipper v.%d.%d\n", Misc::VERSION, Misc::SUBVERSION);
	}

    bool is_wildcard_present(const char *text) {
        return (strcmp(text, Misc::WILDCARD) == 0) ? true : false;
    }
}
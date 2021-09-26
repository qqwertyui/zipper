# zipper
Zip archive unpacker created for learning purposes.

It supports following data formats:
- stored
- deflate

It also doesn't understand what encryption is so any zip encrypted with password cannot be opened.
This project uses slightly modified version of gflags which can be found in this repo.

1. Requirements:
- g++ with c++17 support
- gflags
- gtest
- zlib compression library

2. Build:
git clone https://github.com/qqwertyui/zipper && cd zipper
mkdir build && cd build
cmake .. -G "Unix Makefiles"
make

3. Examples:
./zipper -f SDL2-2.0.16-win32-x64.zip
modification time | file
----------------------------------
08:54 10-08-2021 | README-SDL.txt
08:54 10-08-2021 | SDL2.dll

./zipper -f SDL2-2.0.16-win32-x64.zip -m extract
Extracting README-SDL.txt
Extracting SDL2.dll
[+] Sucesfully extraced file(s)


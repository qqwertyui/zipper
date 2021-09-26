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
- Get archive contents:

./zipper -f SDL2-2.0.16-win32-x64.zip

- Unpack archive:

./zipper -f SDL2-2.0.16-win32-x64.zip -m extract

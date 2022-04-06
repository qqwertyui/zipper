## Application and library for zip manipulations

### 1. Description
Simple zip archive unpacker and lister created for learning purposes. If you are not interested in application
but in library itself it can be found under src/zipcxx
It supports zip files that encryptions scheme is:
- stored
- deflated

It doesn't understand what encryption is so any zip encrypted with password cannot be opened.

### 2. Requirements:
#### 2.1 Libraries
- gflags
- gtest
- zlib
#### 2.2 Tools
- g++ with c++17 support
- cygwin (bash, find)
- cmake
- gnu make

### 3. Build:
- git clone https://github.com/qqwertyui/zipper && cd zipper
- bash ; source setup.sh
- build_app
- run_tests (optional)

### 4. Examples:
#### Get archive contents:
- ./zipper -f SDL2-2.0.16-win32-x64.zip -m list
#### Unpack archive:
- ./zipper -f SDL2-2.0.16-win32-x64.zip -m extract

# zipper
Zip format unpacker created for learning purposes.

Which zip archives it supports:
- stored (non compressed)
- deflate

It also doesn't understand what encryption is so any zip encrypted with password cannot be opened.

To compile the project you need to have GNU Make installed and GNU compiler which supports c++17 standard (it doesn't really have to be this standard but I included this one in Makefile; I'm using g++ 8.1.0).

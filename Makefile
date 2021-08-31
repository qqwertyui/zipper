CC = g++
CFLAGS = -Wall -Wextra -I./include -std=c++17

SRC = $(shell ls src/*.cpp)
INC = $(shell ls include/*.hpp)
OBJ = $(SRC:.cpp=.o)
LIB = -lz -lgflags -lshlwapi

%.o: %.cpp
	$(CC) $(CFLAGS) -c $< -o $@
	
all: $(OBJ)
	$(CC) $(CFLAGS) $(OBJ) $(LIB) -o zipper.exe
	
clean:
	rm zipper.exe -f
	rm src/*.o

format:
	clang-format -i --style=LLVM $(SRC)
	clang-format -i --style=LLVM $(INC)
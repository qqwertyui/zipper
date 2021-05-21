CC = g++
CFLAGS = -Wall -Wextra -I./include -std=c++17

SRC = $(shell ls src/*.cpp)
OBJ = $(SRC:.cpp=.o)
LIB = -lz

%.o: %.cpp
	$(CC) $(CFLAGS) -c $< -o $@
	
all: $(OBJ)
	$(CC) $(CFLAGS) $(OBJ) $(LIB) -o zipper.exe
	
clean:
	rm zipper.exe -f
	rm src/*.o
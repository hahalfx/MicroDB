CXX = g++

CXXFLAGS = -std=c++20 -Wall -Wextra -Iinclude

SRC = $(wildcard src/*.cpp)
OBJ = $(SRC:.cpp=.o)

all: dbms

dbms: $(OBJ)
	$(CXX) $(CXXFLAGS) -o $@ $^

clean:
	rm -f $(OBJ) dbms

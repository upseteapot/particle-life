CXX    := g++
FLAGS  := -Wall -Wextra -std=c++17 
LIBS   := -lsfml-graphics -lsfml-system -lsfml-window -lpthread
TARGET := main

all: clean $(TARGET)

clean:
	rm -f $(TARGET)

$(TARGET): ./src/*.cpp
	$(CXX) $(FLAGS) $(LIBS) -I./include/ $^ -o $@


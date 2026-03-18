CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -I./include

TARGET = Snake.out
SRC = src/main.cpp src/glad.c src/snake.cpp

all: $(TARGET)

$(TARGET): $(SRC)
	$(CXX) $(CXXFLAGS) $(SRC) -o $(TARGET) \
		-L/opt/homebrew/lib -lglfw -framework OpenGL -framework Cocoa -framework IOKit -framework CoreVideo

clean:
	rm -f $(TARGET)

.PHONY: all clean

#17
# Breyta þessu líka eftir nýja verkefninu
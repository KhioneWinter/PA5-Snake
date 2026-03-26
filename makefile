CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -I./include

# auto detect the OS
ifeq ($(OS),Windows_NT)
    # Windows settings
    TARGET = Snake.exe
    LDFLAGS = -L./lib -lglfw3dll -lopengl32 -lgdi32
else ifeq ($(shell uname -s),Darwin)
    # macOS settings
    TARGET = Snake.out
    LDFLAGS = -L/opt/homebrew/lib -lglfw -framework OpenGL -framework Cocoa -framework IOKit -framework CoreVideo

else 
    TARGET = Snake.out
    LDFLAGS = -lglfw -lGL -ldl -lpthread 

endif



SRC = src/main.cpp \
    src/Game.cpp src/Snake.cpp src/Map.cpp src/Score.cpp \
    src/Food.cpp


all: $(TARGET)

$(TARGET): $(SRC)
	$(CXX) $(CXXFLAGS) $(SRC) -o $(TARGET) $(LDFLAGS) -x c++ src/glad.c

clean:
	rm -f Snake.out Snake.exe

.PHONY: all clean

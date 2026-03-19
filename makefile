# CXX = g++
# CXXFLAGS = -std=c++17 -Wall -Wextra -I./include

# TARGET = Snake.out
# SRC = src/main.cpp src/glad.c src/snake.cpp

# all: $(TARGET)

# $(TARGET): $(SRC)
# 	$(CXX) $(CXXFLAGS) $(SRC) -o $(TARGET) \
# 		-L/opt/homebrew/lib -lglfw -framework OpenGL -framework Cocoa -framework IOKit -framework CoreVideo

# clean:
# 	rm -f $(TARGET)

# .PHONY: all clean

# #17
# # Breyta þessu líka eftir nýja verkefninu


CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -I./include

# Automatically detect the OS
ifeq ($(OS),Windows_NT)
    # Windows settings (Assumes MinGW and your provided lib/libglfw3dll.a)
    TARGET = Snake.exe
    LDFLAGS = -L./lib -lglfw3dll -lopengl32 -lgdi32
else
    # macOS settings
    TARGET = Snake.out
    LDFLAGS = -L/opt/homebrew/lib -lglfw -framework OpenGL -framework Cocoa -framework IOKit -framework CoreVideo
endif

# Added Game.cpp to the source list
SRC = src/main.cpp src/glad.c src/Snake.cpp src/Game.cpp 

all: $(TARGET)

$(TARGET): $(SRC)
	$(CXX) $(CXXFLAGS) $(SRC) -o $(TARGET) $(LDFLAGS)

clean:
	rm -f Snake.out Snake.exe

.PHONY: all clean
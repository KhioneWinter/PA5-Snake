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



SRC = src/main.cpp src/glad.c \
    src/Game.cpp src/Snake.cpp src/Map.cpp src/Score.cpp \
    src/Food.cpp


all: $(TARGET)

$(TARGET): $(SRC)
	$(CXX) $(CXXFLAGS) $(SRC) -o $(TARGET) $(LDFLAGS)

clean:
	rm -f Snake.out Snake.exe

.PHONY: all clean
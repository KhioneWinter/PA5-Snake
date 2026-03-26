CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -I./include

# auto detect the OS
ifeq ($(OS),Windows_NT)
    # Windows settings
    TARGET = Snake.exe

    LDFLAGS = -L./lib -lglfw3dll -lopengl32 -lgdi32 # for OpenGL and windowing
    LDFLAGS += -lwinmm -lole32 # audio

    CXXFLAGS += -DMA_ENABLE_WINMM # for miniaudio on Windows

else ifeq ($(shell uname -s),Darwin)
    # macOS settings
    TARGET = Snake.out
    
    LDFLAGS = -L/opt/homebrew/lib -lglfw -framework OpenGL -framework Cocoa -framework IOKit -framework CoreVideo # for OpenGL and windowing
    LDFLAGS += -framework CoreFoundation -framework CoreAudio -framework AudioToolbox # audio

else
    # Linux settings
    TARGET = Snake.out
    LDFLAGS = -lglfw -lGL -lpthread -ldl -lm

endif



SRC = src/main.cpp \
    src/AudioManager.cpp src/miniaudio_impl.cpp \
    src/Renderer.cpp src/TextRenderer.cpp \
    src/Game.cpp src/Snake.cpp src/Map.cpp \
    src/Food.cpp src/Score.cpp




all: $(TARGET)

$(TARGET): $(SRC)
	$(CXX) $(CXXFLAGS) $(SRC) -o $(TARGET) $(LDFLAGS) -x c++ src/glad.c

clean:
	rm -f Snake.out Snake.exe

.PHONY: all clean

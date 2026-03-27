# PA5-Snake
-----------------------------------------
T-403-FORC - group 27

Students:
- Brynhildur Þorbjörg Þórhallsdóttir 
- Herdís Heiða Jing Guðjohnsen
- Sara Elísabet Jónsdóttir

Teachers:
- Atli Fannar <atlif@ru.is>
- Konráð Elí <konrad21@ru.is>
- Teitur Hrólfur <teitur23@ru.is>
------------------------------------------

# Table of Content

- [Setup](#setup)
  - [Libraries Required](#libraries-required)
  - [For macOS](#for-macos)
  - [For Linux/Windows](#for-linuxwindows)
  - [Install GLFW Manually](#install-glfw-manually)

- [How to Play the Game](#how-to-play-the-game)
  - [Compiling the Game](#compiling-the-game)
  - [Running the Game](#running-the-game)
  - [Clean Compiled Files](#clean-compiled-files)

- [Files and Class Diagram](#files-and-class-diagram)
  - [File Structure](#file-structure)
  - [Class Diagram](#class-diagram)

------------------------------------------

# Setup

## Libraries required
- OpenGL (version 4.1+)
- A graphics driver that supports OpenGL 4.1+ (usually already installed)

### For macOS
- glfw (for graphics rendering)
```bash
brew install glfw
```
<em>Note: macOS supports up to OpenGL 4.1 maximum.</em>

### For Linux
- audio library (miniaudio)
```bash
sudo apt install pulseaudio pulseaudio-utils alsa-utils
sudo apt install libasound2-dev
```
- libglfw3 (for graphics rendering)
```bash
sudo apt update
sudo apt install libglfw3 libglfw3-dev
```


#### Install GLFW manually
If you are on windows or if you prefer to install GLFW manually, you can follow these steps:

1. Download GLFW package from https://www.glfw.org/download.html according to your operating system.
2. Locate the `lib` folder inside it and copy the `libglfw3` file to the root directory of this project (replace the existing one if it exists).

------------------------------------------

# How to play the game

## Compiling the game

1. Go to the root directory of the project
```bash
cd path/to/PA5-Snake
```

2. Compile the code using the Makefile:
```bash
make
```

Then we have executable file:
| mac /linux        | windows            |
| ----------------- | ----------------- |
| `Snake.out`       | `Snake.exe`       |

## Running the game

3. Run the executable:
| mac /linux        | windows            |
| ----------------- | ----------------- |
| `./Snake.out`     | `./Snake.exe`     |


<br><em>Note2: on Mac, to have the screen size fit properly, you may need to adjust the window size manually after launching the game</em>


## Clean compiled files
```bash
make clean
```


---------------------------------------------------


# Files and Class Diagram


## File structure
```bash
SnakeGame/ 
│
├── main.cpp
│
├── Game.hpp / Game.cpp
├── Snake.hpp / Snake.cpp
├── Map.hpp / Map.cpp
│
├── Food.hpp / Food.cpp
├── NormalFood.hpp
├── SpecialFood.hpp
│
├── Score.hpp / Score.cpp
├── Leaderboard.hpp / Leaderboard.cpp
│
├── AudioManager.hpp / AudioManager.cpp
│
├── Renderer.hpp / Renderer.cpp
├── TextRenderer.hpp / TextRenderer.cpp
│
├── glad/                   # OpenGL loader
├── GLFW/                   # GLFW header files
├── KHR/                    # OpenGL header files
├── stb_truetype.h          # font rendering library
├── miniaudio_impl.cpp / miniaudio.h  # audio rendering library

lib/ 
│
├── libglwf3.dylib  # mac
├── libglwf3.dll.a  # windows

assets/ 
│
├── all assets (audio, etc.)

root-directory/ 
│
├── Micro5-Regular.ttf  # font for text rendering
├── README.md
├── Makefile
```


## Class Diagram
```mermaid
classDiagram

%% enums
class GameState {
    <<enumeration>>
    START_SCREEN
    PLAYING
    GAME_OVER
}
class MapSize {
    <<enumeration>>
    SMALL
    MEDIUM
    LARGE
}

class MapDimensions {
    width: int
    height: int

    getDimensions(size): MapDimensions
}

%% renderers
class Renderer {
    - shaderProgram: unsigned int
    - VAO: unsigned int
    - VBO: unsigned int

    + drawQuad(int x, int y, float r, float g, float b, MapSize currentSize)
}
class TextRenderer {
     - ShaderProgram: unsigned int
     - VAO: unsigned int
     - VBO: unsigned int
     - fontTexture: unsigned int
     - cdata: char*

    init(fontPath)
    render(const string text, float x, float y, float scale, float r, float g, float b)
}

%% classes
class Game {
    - deathMessage: string

    + Map map
    + Snake snake
    + Score score

    + GameState currentState

    + MapSize currentMapSize;
    + bool wrapAround;

    + dy: int %% next movement direction
    + dx: int %% next movement direction

    + randEngine
    + activeFoods
    + spawnRandomFood(forcePositive)

    + start()
    + update(deltaTime)
    + render()
    + handleInput(GLFWwindow* window)

    + gameOver(message)
    + scoreCalculator()

    %% multiplayer attributes
    isMultiplayer: bool
    Snake snake2
    Score score2
    dx2: int  %% next movement direction
    dy2: int  %% next movement direction
}

class Map {
    - int width
    - int height
    - MapSize size

    + getWidth(): int
    + getHeight(): int
    + getSize(): MapSize

    + isInside(x, y): bool
    + spawnFood(x, y): bool
    + removeFood(x, y): bool
}

class Snake {
    - x: int %% head row
    - y: int %% head col
    - length: int
    - speed: int

    - scoreMultiplier: int
    - boostTimer: float

    - directionX: int
    - directionY: int

    - body: vector<pair<int, int>>

    - dead: bool
    - r: float %% red color component
    - g: float %% green color component
    - b: float %% blue color component

    + move()
    + grow()
    + checkSelfCollision(newX, newY) bool

    + update()
    + render()

    + getY() int
    + getX() int
    + getBody()

    + kill()
    + isDead() bool

    + increaseSpeed(amount)
    + applyScoreBoost(multiplier, duration)
    + getBoostTimer() int
    + getScoreMultiplier() int
    + updateBoost(float deltaTime)
    
    + setColor(r, g, b)
    + getR() float
    + getG() float
    + getB() float
}

class Food {
    <<abstract>>
    + x: int %% column position
    + y: int %% row position
    + r: float %% red color component
    + g: float %% green color component
    + b: float %% blue color component

    + applyEffect(snake)
    + isBomb() bool
}

class NormalFood {
    + applyEffect(snake)
}

class Bomb {
    + applyEffect(snake)
    + isBomb() bool
}

class SpeedFood {
    + applyEffect(snake)
}

class ScoreIncreaseFood {
    + increaseAmount: int
    + duration: int
    + applyEffect(snake)
}

class Score {
    - score: int
    - start: time_t
    - end: time_t
    - multiplier: float
    - foodAmount: int

    + startime()
    + endtime()
    + getScore(): int
    + calculateTime()
    + increase(amount)
    + reset()
    + timeAddedToFinal()
    + getFoodAmount(): int
    + incrementFoodAmount()
}

class Leaderboard {
    + addScore(playerId, score)
    + top(K): int
    + reset(playerId)
}

class AudioManager {
    - engine: ma_engine
    - sounds: std::map<string, ma_sound>
    - initialized: bool

    + init(): bool
    + playSound(name)
    + playMusic(name)
    + stopMusic(name)

    + loseSoundPath: string
    + eatFoodSoundPath: string
    + powerupSoundPath: string
    + selectSoundPath: string
    + backgroundMusicPath: string
}

%% tengingar
Game --> Map : updates
Game --> Snake : controls
Game --> Score : tracks

Game --> AudioManager : uses
Game --> Renderer : uses
Game --> TextRenderer : uses


Map "1" --> "0..*" Food : contains

Food <|-- NormalFood
Food <|-- Bomb
Food <|-- SpeedFood
Food <|-- ScoreIncreaseFood


Snake --> Food : consumes

Leaderboard "1" --> "0..*" Score : stores
```

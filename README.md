# PA5-Snake
We are going to implement the classic Snake game using OpenGL.

-----------------------------------------
Students:
- Brynhildur Þorbjörg Þórhallsdóttir 
- Herdís Heiða Jing Guðjohnsen
- Sara Elísabet Jónsdóttir


<!-- // bara var á töflunni -->
#T-403-FORC - group 27

Teachers:
- Atli Fannar <atlif@ru.is>
- Konráð Elí <konrad21@ru.is>
- Teitur Hrólfur <teitur23@ru.is>

<!-- ps- það er fyrir neðan
#compile/run
""" 
cd build
cmake ..
cmake -- build
""" -->

------------------------------------------


## Compilation and Running the Game!!

### Libraries required:
- OpenGL (at least ver 4.1)
- glfw (mac: `brew install glfw`) (TODO brynhildur/SARA ég vtk hvað þið þurftuð að download-a)

### Before compiling:

1. Download GLFW package from https://www.glfw.org/download.html according to your operating system.
2. Locate the `lib` folder inside it and copy the `libglfw3` file to the root directory of this project (replace the existing one if it exists).

### Compile walk through:
| mac               | linux / window    |
| ----------------- | ----------------- |
| `cmd + shift + B` | `ctr + shift + B` |

or use `make` command in terminal


Then we have executable file:
| mac               | linux / window    |
| ----------------- | ----------------- |
| `Snake.out`       | `Snake.exe`       |


### Run game:
| mac               | linux / window    |
| ----------------- | ----------------- |
| `./Snake.out`     | `./Snake.exe`     |

<em>Note: the program might not detect the correct OS, therefore the executable name might still be `Snake.out` even though you are on Windows</em>


### Clean compiled files:
```bash
make clean
```


---------------------------------------------------


# Files and Class Diagram


## File structure:
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

root-directory/ 
│
├── Micro5-Regular.ttf  # font for text rendering
├── README.md
├── Makefile
```


## Class Diagram:
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

    + dy: int %% current movement direction
    + dx: int %% current movement direction

    + randEngine
    + activeFoods
    + spawnRandomFood(forcePositive)

    + start()
    + update(deltaTime)
    + render()
    + handleInput(GLFWwindow* window)
    + gameOver(message)

    + scoreCalculator()
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
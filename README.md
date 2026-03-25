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
| mac               | windows           |
| ----------------- | ----------------- |
| `cmd + shift + B` | `ctr + shift + B` |

or use `make` command in terminal


Then we have executable file:
| mac               | windows           |
| ----------------- | ----------------- |
| `Snake.out`       | `Snake.exe`       |


### Run game:
| mac               | windows           |
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
├── Game.h / Game.cpp
├── Snake.h / Snake.cpp
├── SnakeSegment.h
├── Map.h / Map.cpp
│
├── Food.h / Food.cpp
├── NormalFood.h
├── SpecialFood.h
│
├── Score.h / Score.cpp
├── Leaderboard.h / Leaderboard.cpp
│
├── glad/           # OpenGL loader
├── GLFW/           # GLFW header files
├── KHR/            # OpenGL header files
├── stb_truetype.h  # font rendering

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

%% classes
class Game {
    - shaderProgram: unsigned int
    - VAO: unsigned int
    - VBO: unsigned int
    - setupOpenGL()
    - drawQuad(int x, int y, float r, float g, float b)

    - textShaderProgram: unsigned int
    - textVAO: unsigned int
    - textVBO: unsigned int
    - fontTexture: unsigned int
    - cdata: char*

    + Map map
    + Snake snake
    + Score score

    + GameState state

    + mapWidth: int
    + mapHeight: int
    + wrapAround: bool

    + randEngine
    + activeFoods
    + spawnRandomFood(forcePositive)

    + setupTextRendering()
    + renderText(text, x, y, scale, r, g, b)

    + start()
    + update()
    + render()
    + handleInput(GLFWwindow* window)

    + scoreCalculator()
}

class Map {
    - int width
    - int height

    + getWidth(): int
    + getHeight(): int

    + isInside(x, y): bool
    + spawnFood()
    + removeFood(food)
}

class Snake {
    - x: int %% head row
    - y: int %% head col
    - length: int
    - speed: int

    - scoreMultiplier: int
    - boostTimer: int

    - directionX: int
    - directionY: int

    - dead: bool
    - r: float
    - g: float
    - b: float

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
    + updateBoost()
    
    + setColor(r, g, b)
    + getR() float
    + getG() float
    + getB() float
}

class Food {
    <<abstract>>
    +  x
    + int y
    + float r
    + float g
    + float b

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

%% tengingar
Game --> Map : updates
Game --> Snake : controls
Game --> Score : tracks

Map "1" --> "0..*" Food : contains

Food <|-- NormalFood
Food <|-- Bomb
Food <|-- SpeedFood
Food <|-- ScoreIncreaseFood


Snake --> Food : consumes

Leaderboard "1" --> "0..*" Score : stores
```
#ifndef GAME_HPP
#define GAME_HPP


class Map;
class Snake;
class Score;

class Game {
public:
    Game();
    ~Game();
    
    Map *map;
    Snake *snake;
    Score *score;
    bool isRunning;

    void start();
    void update();
    void render();
    // TODO void handleInput(direction);
};


#endif // GAME_HPP
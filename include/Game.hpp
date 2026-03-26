#ifndef GAME_HPP
#define GAME_HPP

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "stb_truetype.h" // text rendering

#include <vector>
#include <random> // for random engine

#include "Food.hpp"
#include "Map.hpp"
#include "AudioManager.hpp"
#include "Renderer.hpp"
#include "TextRenderer.hpp"


class Snake;
class Score;


// the different states the game can be in
enum class GameState {
    START_SCREEN,
    PLAYING,
    GAME_OVER
};

class Game {
private:
    // game data
    std::string deathMessage = "";

public:
    Game();
    ~Game();
    
    // game obj
    Renderer* worldRenderer;
    TextRenderer* textRenderer;
    AudioManager* audio;
    Map *map;
    Snake *snake;
    Score *score;

    GameState currentState;

    // map settings
    MapSize currentMapSize;
    bool wrapAround;

    // track food and movement
    int dx;
    int dy;

    // food management
    std::subtract_with_carry_engine<uint32_t, 24, 10, 24> randEngine;
    std::vector<Food*> activeFoods;
    void spawnRandomFood(bool forcePositive = false); // positive meaning (GOOD FOOD)

    // game loop functions
    void start();
    void update();
    void render();
    void handleInput(GLFWwindow* window);
    
    void gameOver(std::string message);
    void scoreCalculator();
};


#endif // GAME_HPP
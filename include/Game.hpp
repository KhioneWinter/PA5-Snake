#ifndef GAME_HPP
#define GAME_HPP

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <vector>
#include <random> // for random engine
#include "Food.hpp"

class Map;
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
    // OpenGL rendering helpers
    unsigned int shaderProgram;
    unsigned int VAO, VBO;
    void setupOpenGL();
    void drawQuad(int x, int y, float r, float g, float b);

public:
    Game();
    ~Game();
    
    Map *map;
    Snake *snake;
    Score *score;

    GameState currentState;

    // map settings
    int mapWidth;
    int mapHeight;
    bool wrapAround;

    // track food and movement
    int currentFoodX;
    int currentFoodY;
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

};


#endif // GAME_HPP
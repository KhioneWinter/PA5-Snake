#ifndef GAME_HPP
#define GAME_HPP

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "stb_truetype.h" // text rendering

#include <vector>
#include <random> // for random engine
#include <fstream>
#include <algorithm>

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
    COUNTDOWN,
    PLAYING,
    GAME_OVER,
    ADD_INITIALS,
    SCORE_BOARD
};

class Game {
struct ScoreEntry {
    std::string initials;
    int score;
};
private:
    // game data
    std::string deathMessage = "";

    // countdown state
    float countdownTimer;
    int lastCountdownSecond;

public:
    std::string playerInitials = "";
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

    // track movement (next direction)
    int dx; // next direction
    int dy; // next direction
    int lastDx = 1; // tracking so snake doesn't reverse into itself
    int lastDy = 0; // tracking so snake doesn't reverse into itself
    float p1MoveTimer = 0.0f;
    float globalSpeedTimer = 0.0f; // speed up snake every 10 sec

    // food management
    std::subtract_with_carry_engine<uint32_t, 24, 10, 24> randEngine;
    std::vector<Food*> activeFoods;
    void spawnRandomFood(bool forcePositive = false); // positive meaning (GOOD FOOD)

    // game loop functions
    void start();
    void update(float deltaTime);
    void render();

    // input handling
    void handleInput(GLFWwindow* window);
    bool mKeyWasPressed = false;
    bool spaceKeyWasPressed = false;
    
    // game management
    void gameOver(std::string message);
    void scoreCalculator();
    void showCurrTopScores();
    void addInitialsToFile();

    // countdown management
    void startCountdown();
    void startGameplay();

    // multiplayr (player 2)
    bool isMultiplayer = false;
    Snake* snake2;
    Score* score2;
    int dx2, dy2; // next direction
    int lastDx2 = -1;
    int lastDy2 = 0;
    float p2MoveTimer = 0.0f;
};

void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);


#endif // GAME_HPP
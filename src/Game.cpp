#include "Game.hpp"
#include "Map.hpp"
#include "Snake.hpp"
#include "Score.hpp"
#include "NormalFood.hpp"
#include "SpecialFood.hpp"

#include <cstdlib>
#include <iostream>
#include <chrono>  // for chrono random seed
#include <stdio.h> // for FILE reading


// Pick multiplayer winner by score when a collision happens
static std::string getMultiplayerWinnerMessage(const Score* score1, const Score* score2) {
    if (!score1 || !score2) return "Head to Head! It's a Tie!";
    int s1 = score1->getScore();
    int s2 = score2->getScore();
    if (s1 > s2) return "Player 1 Wins! Higher score!";
    if (s2 > s1) return "Player 2 Wins! Higher score!";
    return "Head to Head! It's a Tie!";
}


    
Game::Game() : currentState(GameState::START_SCREEN), currentMapSize(MapSize::SMALL), wrapAround(false), dx(1), dy(0) {
    // initalize game objects
    map = nullptr;
    snake = nullptr; 
    score = nullptr;

    snake2 = nullptr;
    score2 = nullptr;

    // init audio
    audio = new AudioManager();
    if (audio) {
        if (!audio->init()) {
            std::cout << "Failed to initialize audio!" << std::endl;
        }
    }

    // init renderers
    worldRenderer = new Renderer();
    textRenderer = new TextRenderer();

    // random seed
    randEngine.seed(std::chrono::system_clock::now().time_since_epoch().count());
}

Game::~Game() {
    // clean up memory
    delete map;

    delete snake;
    delete score;

    delete snake2;
    delete score2;

    delete audio;

    delete worldRenderer;
    delete textRenderer;

    for (Food* f : activeFoods) { delete f; }
    activeFoods.clear();
}

void Game::start() {
    worldRenderer->init();
    textRenderer->init("Micro5-Regular.ttf");

    // play music
    if (audio) audio->playMusic(audio->backgroundMusicPath.c_str(), true);


    currentState = GameState::START_SCREEN;
    std::cout << "===========================" << std::endl;
    std::cout << "       SNAKE GAME          " << std::endl;
    std::cout << "===========================" << std::endl;
    std::cout << " SETTINGS:" << std::endl;
    std::cout << " Press 1 : Small Map" << std::endl;
    std::cout << " Press 2 : Medium Map" << std::endl;
    std::cout << " Press 3 : Large Map" << std::endl;
    std::cout << " Press B : Bounded Walls (Die on hit)" << std::endl;
    std::cout << " Press L : Loop Walls (Wrap around)" << std::endl;
    std::cout << "---------------------------" << std::endl;
    std::cout << " Press SPACE to start!     \n" << std::endl;
}

void Game::handleInput(GLFWwindow* window) {
    if (currentState == GameState::GAME_OVER) {
        if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) { // Allows user to restart game
            start(); 
        }
        if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS) { // Allows users to quit the game
            glfwSetWindowShouldClose(window, GLFW_TRUE);
        }
        return; // any other input
    }
    if (currentState == GameState::START_SCREEN) {
         // --- quit game ---
        if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS) glfwSetWindowShouldClose(window, GLFW_TRUE);

        // --- settings bindings ---
        if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS && currentMapSize != MapSize::SMALL) { currentMapSize  = MapSize::SMALL; std::cout  << "Selected: SMALL MAP \n"; audio->playSound(audio->selectSoundPath.c_str()); }
        if (glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS && currentMapSize != MapSize::MEDIUM) { currentMapSize = MapSize::MEDIUM; std::cout << "Selected: MEDIUM MAP\n"; audio->playSound(audio->selectSoundPath.c_str()); }
        if (glfwGetKey(window, GLFW_KEY_3) == GLFW_PRESS && currentMapSize != MapSize::LARGE) { currentMapSize  = MapSize::LARGE; std::cout  << "Selected: LARGE MAP \n"; audio->playSound(audio->selectSoundPath.c_str()); }
        
        if (glfwGetKey(window, GLFW_KEY_B) == GLFW_PRESS && wrapAround == true)  { wrapAround = false; std::cout << "Selected: BOUNDED WALLS\n"; audio->playSound(audio->selectSoundPath.c_str()); }
        if (glfwGetKey(window, GLFW_KEY_L) == GLFW_PRESS && wrapAround == false) { wrapAround = true;  std::cout << "Selected: LOOP WALLS   \n"; audio->playSound(audio->selectSoundPath.c_str()); }

        // --- multiplayer toggle ---
        if (glfwGetKey(window, GLFW_KEY_M) == GLFW_PRESS) {
            isMultiplayer = !isMultiplayer;
            std::cout << "Multiplayer: " << (isMultiplayer ? "ON" : "OFF") << std::endl;
            audio->playSound(audio->selectSoundPath.c_str());
        }

        // --- start Game ---
        if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) {
            startCountdown();
        }


    }
    else if (currentState == GameState::PLAYING) {
        // prevent snake from moving into itself == disallowing reverse direction

        // Player 1 controls
        if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS && dy != -1)    { dx = 0; dy = 1; }
        if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS && dy != 1)   { dx = 0; dy = -1; }
        if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS && dx != 1)   { dx = -1; dy = 0; }
        if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS && dx != -1) { dx = 1; dy = 0; }

        // Player 2 controls
        if (isMultiplayer) {
            if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS && dy2 != -1) { dx2 = 0; dy2 = 1; }
            if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS && dy2 != 1)  { dx2 = 0; dy2 = -1; }
            if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS && dx2 != 1)  { dx2 = -1; dy2 = 0; }
            if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS && dx2 != -1) { dx2 = 1; dy2 = 0; }
        }
    }
}


void Game::gameOver(std::string message) {
    deathMessage = message;

    scoreCalculator();
    if (isMultiplayer && score2) {
        score2->endTime();
        score2->calculateTime();
        score2->timeAddedToFinal();
    }
    std::cout << "\n---------------------------" << std::endl;
    std::cout << message << std::endl;
    std::cout << "Game Over! Score: " << score->getScore();
    std::cout << "\n---------------------------";
    std::cout << "\n Press SPACE to start again or Q to quit!" << std::endl;
    currentState = GameState::GAME_OVER;

    if (audio) {
        audio->stopMusic(audio->backgroundMusicPath.c_str());
        audio->playSound(audio->loseSoundPath.c_str());
    }
}

void Game::update(float deltaTime) {
    // --- COUNTDOWN LOGIC ---
    if (currentState == GameState::COUNTDOWN) {
        countdownTimer -= deltaTime;
        int currentSecond = (int)countdownTimer;
        
        // Play a sound every time the number changes
        if (currentSecond < lastCountdownSecond && currentSecond > 0) {
            if (audio) audio->playSound(audio->selectSoundPath.c_str());
            lastCountdownSecond = currentSecond;
        }
        
        // When timer hits 0, start the real game!
        if (countdownTimer <= 0.0f) {
            startGameplay();
        }
        return; // Don't run any snake movement logic while counting down!
    }


    if (currentState != GameState::PLAYING) return;

    // --- speed up slowly over time ---
    snake->increaseSpeed(1);
    if (isMultiplayer) snake2->increaseSpeed(1); 

    // --- decrement booster timer if active ---
    snake->updateBoost(deltaTime);
    if (isMultiplayer) snake2->updateBoost(deltaTime);

    // --- Player 1 Movement ---
    // calculate where the snake is trying to go
    int nextX = snake->getX() + dx;
    int nextY = snake->getY() + dy;

    // -- Player 2 Movement ---
    int nextX2 = 0, nextY2 = 0;
    if (isMultiplayer) {
        nextX2 = snake2->getX() + dx2;
        nextY2 = snake2->getY() + dy2;

        // --- Rule 1: Head-to-Head Collision ---
        if (nextX == nextX2 && nextY == nextY2) {
            if (score->getScore() > score2->getScore()) {
                gameOver("Player 1 Wins! Head to Head with higher score.");
            } else if (score2->getScore() > score->getScore()) {
                gameOver("Player 2 Wins! Head to Head with higher score.");
            } else {
                gameOver("Head to Head! It's a Tie!");
            }
            return;
        }

        // --- Rule 2: P1 Head hits P2 Body ---
        if (snake2->checkCollision(nextX, nextY)) {
            gameOver(getMultiplayerWinnerMessage(score, score2));
            return;
        }

        // --- Rule 3: P2 Head hits P1 Body ---
        if (snake->checkCollision(nextX2, nextY2)) {
            gameOver(getMultiplayerWinnerMessage(score, score2));
            return;
        }
    }

    // check Map Boundaries
    if (wrapAround) {
        // Wrap logic: if we go out of bounds, appear on the other side
        if (nextX < 0) nextX = map->getWidth() - 1;
        if (nextX >= map->getWidth()) nextX = 0;
        if (nextY < 0) nextY = map->getHeight() - 1;
        if (nextY >= map->getHeight()) nextY = 0;

        if (isMultiplayer) {
            if (nextX2 < 0) nextX2 = map->getWidth() - 1;
            if (nextX2 >= map->getWidth()) nextX2 = 0;
            if (nextY2 < 0) nextY2 = map->getHeight() - 1;
            if (nextY2 >= map->getHeight()) nextY2 = 0;
        }

    } else {
        // Solid wall logic: die if we go out of bounds
        if (!map->isInside(nextX, nextY)) {
            if (isMultiplayer) {
                gameOver(getMultiplayerWinnerMessage(score, score2));
            } else {
                gameOver("Ouch! You hit a wall!");
            }
            return;
        }

        if (isMultiplayer) {
            if (!map->isInside(nextX2, nextY2)) {
                gameOver(getMultiplayerWinnerMessage(score, score2));
                return;
            }
        }
    }

    // check Self-Collision
    if (snake->checkCollision(nextX, nextY)) {
        if (isMultiplayer) {
            gameOver(getMultiplayerWinnerMessage(score, score2));
        } else {
            gameOver("Oh no! You ran into yourself!");
        }
        return;
    }
    if (isMultiplayer) {
        if (snake2->checkCollision(nextX2, nextY2)) {
            gameOver(getMultiplayerWinnerMessage(score, score2));
            return;
        }
    }

    // Move the snake
    snake->move(nextX, nextY);
    if (isMultiplayer) snake2->move(nextX2, nextY2);


    // Check Food Collisions
    bool ateFood = false; // Flag to track if we need to spawn food
    for (auto it = activeFoods.begin(); it != activeFoods.end(); ) {

        // see who is eating the food
        Snake* eatingSnake = nullptr;
        Score* activeScore = nullptr;

        // Figure out WHICH snake hit the food
        if (snake->getX() == (*it)->x && snake->getY() == (*it)->y) {
            eatingSnake = snake;
            activeScore = score;
        } else if (isMultiplayer && snake2->getX() == (*it)->x && snake2->getY() == (*it)->y) {
            eatingSnake = snake2;
            activeScore = score2;
        }

        if (eatingSnake != nullptr) {
            
            Food* eatenFood = *it;
            
            eatenFood->applyEffect(*eatingSnake);

            if (!eatenFood->isBomb()) {
                int pointsGained = 10 * eatingSnake->getScoreMultiplier();
                activeScore->increase(pointsGained);
                
                std::cout << "Score: " << activeScore->getScore();

                // snake powerup
                if (eatingSnake->getScoreMultiplier() > 1) {
                    // snake power up
                    audio->playSound(audio->powerupSoundPath.c_str());
                    std::cout << " (BOOST ACTIVE: " << eatingSnake->getScoreMultiplier() << "x!)";
                }

                // snake speed increase
                if (dynamic_cast<SpeedFood*>(eatenFood)) {
                    std::cout << " (SPEED UP!)";
                }

                std::cout << std::endl;
                activeScore -> incrementFoodAmount();
                ateFood = true; // Mark that we need to spawn food
            }
            
            delete eatenFood;
            it = activeFoods.erase(it); // remove eaten food from the list
            break; // snake can only eat one food at a time
        } else {
            it++;
        }
    }
    
    // Check if the snake died from a bomb
    if (snake->isDead()) {
        if (isMultiplayer) {
            gameOver("KABOOM! Player 1 ate a bomb!");
        } else {
            gameOver("KABOOM! You ate a bomb!");
        }
        return;
    }
    if (isMultiplayer && snake2->isDead()) {
        gameOver("KABOOM! Player 2 ate a bomb!");
        return;
    }

    // only spawn food if we ate something (don't want too much food on the map)
    if (ateFood) {
        audio->playSound(audio->eatFoodSoundPath.c_str());
        spawnRandomFood();
    }
}


void Game::render() {
    MapDimensions dims = getDimensions(currentMapSize);
    int mapWidth = dims.width;
    int mapHeight = dims.height;

    if (currentState == GameState::START_SCREEN) {
        // Draw a blue pattern for Start Screen
        int setValue = 6;
        float mult = 4.0;
        if (mapWidth == 36 && mapHeight == 30) {
            setValue = 5; // Fixes so the big map 
            mult = 4.5;
        }
        if (mapWidth == 12 && mapHeight== 10) {
            mult = 3.0; // Fixes so the small map 
        }
        for(int x = mapWidth / setValue; x < (mapWidth / setValue) * mult; x++) {
            for(int y = mapHeight / setValue; y < (mapHeight / setValue) * 4; y++) {
                worldRenderer->drawQuad(x, y, 0.0f, 0.4f, 0.8f, currentMapSize); // Colors blue
            }
        }

        // draw text over the background
        textRenderer->render("SNAKE", 320.0f, 50.0f, 2.5f, 0.0f, 1.0f, 0.0f); // Big Green Text

        // --- MAP SIZE SETTINGS ---
        textRenderer->render("Map size:", 20.0f, 30.0f, 1.0f, 1.0f, 1.0f, 0.0f); 
        
        textRenderer->render("Press 1: Small",  30.0f, 60.0f, 1.0f, 1.0f, (currentMapSize == MapSize::SMALL ? 0.4f : 1.0f), (currentMapSize == MapSize::SMALL ? 0.7f : 1.0f));
        textRenderer->render("Press 2: Medium", 30.0f, 85.0f, 1.0f, 1.0f, (currentMapSize == MapSize::MEDIUM ? 0.4f : 1.0f), (currentMapSize == MapSize::MEDIUM ? 0.7f : 1.0f));
        textRenderer->render("Press 3: Large",  30.0f, 110.0f, 1.0f, 1.0f, (currentMapSize == MapSize::LARGE ? 0.4f : 1.0f), (currentMapSize == MapSize::LARGE ? 0.7f : 1.0f));

        // --- WALL MODE SETTINGS ---
        textRenderer->render("Wall Mode:", 530.0f, 30.0f, 1.0f, 1.0f, 1.0f, 0.0f); 
        textRenderer->render("Press B : Bounded Walls", 550.0f, 55.0f, 1.0f, 1.0f, (!wrapAround ? 0.4f : 1.0f), (!wrapAround ? 0.7f : 1.0f));
        textRenderer->render("(Die on hit)", 610.0f, 75.0f, 1.0f, 1.0f, (!wrapAround ? 0.4f : 1.0f), (!wrapAround ? 0.7f : 1.0f));
        textRenderer->render("Press L : Loop Walls",   550.0f, 100.0f, 1.0f, 1.0f, (wrapAround ? 0.4f : 1.0f),  (wrapAround ? 0.7f : 1.0f));
        textRenderer->render("(Wrap around)",   610.0f, 120.0f, 1.0f, 1.0f, (wrapAround ? 0.4f : 1.0f),  (wrapAround ? 0.7f : 1.0f));

        // --- MULTIPLAYER SETTINGS ---
        textRenderer->render("Multiplayer:", 20.0f, 140.0f, 1.0f, 1.0f, 1.0f, 0.0f);
        textRenderer->render("Press M to Toggle", 30.0f, 160.0f, 1.0f, 1.0f, (isMultiplayer ? 0.4f : 1.0f), (isMultiplayer ? 0.7f : 1.0f));


        // --- INSTRUCTIONS ---
        textRenderer->render("Press T to see Top Scores", 240.0f, 500.0f, 1.2f, 1.0f, 1.0f, 0.0f); // Yellow
        textRenderer->render("Press SPACE to Start", 250.0f, 550.0f, 1.5f, 1.0f, 1.0f, 1.0f); // White
        textRenderer->render("Press Q to Quit", 330.0f, 580.0f, 1.0f, 1.0f, 1.0f, 1.0f); // White
    }


    else if (currentState == GameState::COUNTDOWN) {
        // Draw Map dynamically
        for (int x = 0; x < mapWidth; x++) {
            for (int y = 0; y < mapHeight; y++) {
                if ((x+y) % 2 == 0) {
                    worldRenderer->drawQuad(x, y, 0.20f, 0.20f, 0.20f, currentMapSize); 
                } else {
                    worldRenderer->drawQuad(x, y, 0.25f, 0.25f, 0.25f, currentMapSize); 
                }
            } 
        }

        // Draw the big countdown number
        int displayNum = (int)countdownTimer;
        if (displayNum > 0) {
            // Big yellow number
            textRenderer->render(std::to_string(displayNum), 380.0f, 300.0f, 5.0f, 1.0f, 1.0f, 0.0f);
        }
    }

    else if (currentState == GameState::PLAYING) {
        // Draw Map dynamically based on width and height
        for (int x = 0; x < mapWidth; x++) {
            for (int y = 0; y < mapHeight; y++) {
                if ((x+y) % 2 == 0) {
                    worldRenderer->drawQuad(x, y, 0.20f, 0.20f, 0.20f, currentMapSize); // Differend hues of gray
                }
                else {
                    worldRenderer->drawQuad(x, y, 0.25f, 0.25f, 0.25f, currentMapSize); // used to see grid better
                }
                
            } 
        }
        // ------------------ Draw Food ------------------
        for (Food* f : activeFoods) {
            worldRenderer->drawQuad(f->x, f->y, f->r, f->g, f->b, currentMapSize);
        }

        // ------------------draw snake ------------
        auto body = snake->getBody();
        int totalSegments = body.size();

        // draw snake body
        for (int i = 0; i < totalSegments; ++i) {
            float taper = 1.0f;
            
            // last 3 segments (the tail) will be skinnier
            if (i > totalSegments - 4) taper = 1.0f - ((float)(i - (totalSegments - 4)) * 0.25f);

            // adjust color and size based on how close we are to the tail
            worldRenderer->drawQuad(body[i].first, body[i].second, 
                    snake->getR() * taper, 
                    snake->getG() * taper, 
                    snake->getB() * taper, currentMapSize);

        // draw snake head (lighter than body)
        float headX = snake->getX();
        float headY = snake->getY();
        float r = snake->getR() + 0.2f;
        float g = snake->getG() + 0.2f;
        float b = snake->getB() + 0.2f;

        worldRenderer->drawQuad(headX, headY, r, g, b, currentMapSize);
        }

        if (isMultiplayer) {
            auto body2 = snake2->getBody();
            int totalSegments2 = body2.size();
            for (int i = 0; i < totalSegments2; ++i) {
                float taper = 1.0f;
                if (i > totalSegments2 - 4) taper = 1.0f - ((float)(i - (totalSegments2 - 4)) * 0.25f);
                worldRenderer->drawQuad(body2[i].first, body2[i].second, 
                        snake2->getR() * taper, 
                        snake2->getG() * taper, 
                        snake2->getB() * taper, currentMapSize);
            }
            float headX2 = snake2->getX();
            float headY2 = snake2->getY();
            float r = snake2->getR() + 0.2f;
            float g = snake2->getG() + 0.2f;
            float b = snake2->getB() + 0.2f;
            worldRenderer->drawQuad(headX2, headY2, r, g, b, currentMapSize);
        }
        // ------------------------------------------------------------------

    
        // ------------------ draw Score (top left) ------------------
        textRenderer->render("SCORE: " + std::to_string(score->getScore()), 20.0f, 40.0f, 1.0f, 1.0f, 1.0f, 1.0f);
        
        // if boost acitve, show boost timer (bottom middle)
        if (snake->getScoreMultiplier() > 1) {
            textRenderer->render("SCORE BOOST ACTIVE: " + std::to_string(snake->getBoostTime()), 20.0f, 70.0f, 1.0f, 1.0f, 0.5f, 1.0f); // Pink
        }
    }

    else if (currentState == GameState::GAME_OVER) {
        // Draw red Game Over screen
        for(int x = 0; x < mapWidth; x++) {
            for(int y = 0; y < mapHeight; y++) {
                worldRenderer->drawQuad(x, y, 0.5f, 0.0f, 0.0f, currentMapSize);
            }
        }

        // DRAW GAME OVER TEXT
                                        // x, y, scale, r, g, b, a
        textRenderer->render("GAME OVER", 0.0f, 180.0f, 3.0f, 1.0f, 1.0f, 0.0f, 1); // Big Yellow Text

        // small death msg
        textRenderer->render(deathMessage, 0.0f, 220.0f, 0.8f, 1.0f, 1.0f, 1.0f, 1); // White text

        // game stats
        if (isMultiplayer && score2) {
            textRenderer->render("P1 Score: " + std::to_string(score->getScore()),      0.0f, 310.0f, 1.0f, 0.0f, 1.0f, 0.0f, 1);
            textRenderer->render("P1 Fruit: " + std::to_string(score->getFoodAmount()), 0.0f, 340.0f, 1.0f, 0.0f, 1.0f, 0.0f, 1);
            textRenderer->render("P2 Score: " + std::to_string(score2->getScore()),      0.0f, 370.0f, 1.0f, 0.0f, 0.5f, 1.0f, 1);
            textRenderer->render("P2 Fruit: " + std::to_string(score2->getFoodAmount()), 0.0f, 400.0f, 1.0f, 0.0f, 0.5f, 1.0f, 1);
        } else {
            textRenderer->render("Final Score: " + std::to_string(score->getScore()),      0.0f, 310.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1);
            textRenderer->render("Fruit Eaten: " + std::to_string(score->getFoodAmount()), 0.0f, 340.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1);
        }
                // controls
                textRenderer->render("Press S to Save Score", 0.0f, 450.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1); // White
                textRenderer->render("Press SPACE to Try Again", 0.0f, 500.0f, 1.5f, 1.0f, 1.0f, 1.0f, 1); // White
                textRenderer->render("Press Q to Quit", 0.0f, 550.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1); // White
            }
        }

void Game::spawnRandomFood(bool forcePositive) {
    int fx, fy;
    map->spawnFood(fx, fy); // get location on map for new food
    
    int chance = randEngine() % 100; // custome random engine
    

    if (!forcePositive && chance < 15) {
        // 15% change
        activeFoods.push_back(new BombFood(fx, fy)); 
        spawnRandomFood(true);  // force a good food to spawn alongside it
    }

    else if (chance < 35) {
        // 35-15 = 20% chance
        activeFoods.push_back(new SpeedFood(fx, fy)); // cyan color
    } 
    else if (chance < 60) {
        // 60-35 = 25% chance
        activeFoods.push_back(new ScoreIncreaseFood(fx, fy, 1.0f, 0.4f, 0.7f)); // snake turns pink when active
    }
    else {
        activeFoods.push_back(new NormalFood(fx, fy));
    }
}

void Game::scoreCalculator() {
    // Calls all necisery functions from Score file to calculate final score
    score -> endTime();
    score -> calculateTime();
    score -> timeAddedToFinal();
}



void Game::startCountdown() {
    // clean up old objects if restarting
    delete map; delete snake; delete score; 
    delete snake2; delete score2;
    for (Food* f : activeFoods) { delete f; }
    activeFoods.clear();

    // Create ONLY the map so the user can see it
    map = new Map(currentMapSize);

    // Set the timer to just under 4 seconds (so it shows 3, then 2, then 1)
    countdownTimer = 3.99f;
    lastCountdownSecond = 4; // Used to track when to play the beep sound

    currentState = GameState::COUNTDOWN;
}

void Game::startGameplay() {
    MapDimensions dims = getDimensions(currentMapSize);
    score = new Score(0);
    score->startTime();
    spawnRandomFood();

    // --- init snake ---
    if (!isMultiplayer) {
        // --- single player mode ---
        snake = new Snake(dims.width / 2, dims.height / 2); // middle of map
        dx = 1; dy = 0;
    }
    else { 
        // --- multiplayer mode ---

        // spawn P1 on the right, moving right (away from P2)
        snake = new Snake((dims.width / 3) * 2, dims.height / 2);
        snake->setColor(0.0f, 0.8f, 0.0f); // Green
        dx = 1; dy = 0;

        // Spawn P2 on the right, moving right (away from P1)
        snake2 = new Snake(dims.width / 3, dims.height / 2);
        snake2->setColor(0.0f, 0.5f, 0.9f); // Blue
        dx2 = -1; dy2 = 0;

        score2 = new Score(0);
        score2 -> startTime();
    }
    
    // Final sound to say "GO!"
    if (audio) audio->playSound(audio->powerupSoundPath.c_str()); 
    currentState = GameState::PLAYING;
    std::cout << "\nGame started!" << std::endl;
    if (isMultiplayer) {
        std::cout << "Player 1 is Green, Player 2 is Blue. Good luck!\n" << std::endl;
    }
}
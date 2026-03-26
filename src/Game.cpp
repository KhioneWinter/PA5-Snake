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



    
Game::Game() : currentState(GameState::START_SCREEN), currentMapSize(MapSize::SMALL), wrapAround(false), dx(1), dy(0) {
    // initalize game objects
    map = nullptr;
    snake = nullptr; 
    score = nullptr;

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
    std::cout << " Press 2 : Medium Map (Default)" << std::endl;
    std::cout << " Press 3 : Large Map" << std::endl;
    std::cout << " Press B : Bounded Walls (Die on hit - Default)" << std::endl;
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
        
        // settings bindings
        if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS && currentMapSize != MapSize::SMALL) { currentMapSize  = MapSize::SMALL; std::cout  << "Selected: SMALL MAP \n"; audio->playSound(audio->selectSoundPath.c_str()); }
        if (glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS && currentMapSize != MapSize::MEDIUM) { currentMapSize = MapSize::MEDIUM; std::cout << "Selected: MEDIUM MAP\n"; audio->playSound(audio->selectSoundPath.c_str()); }
        if (glfwGetKey(window, GLFW_KEY_3) == GLFW_PRESS && currentMapSize != MapSize::LARGE) { currentMapSize  = MapSize::LARGE; std::cout  << "Selected: LARGE MAP \n"; audio->playSound(audio->selectSoundPath.c_str()); }
        
        if (glfwGetKey(window, GLFW_KEY_B) == GLFW_PRESS && wrapAround == true)  { wrapAround = false; std::cout << "Selected: BOUNDED WALLS\n"; audio->playSound(audio->selectSoundPath.c_str()); }
        if (glfwGetKey(window, GLFW_KEY_L) == GLFW_PRESS && wrapAround == false) { wrapAround = true;  std::cout << "Selected: LOOP WALLS   \n"; audio->playSound(audio->selectSoundPath.c_str()); }

        // start Game
        if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) {
            delete map; delete snake; delete score; // clean up old game if restarting
            // clean up old food
            for (Food* f : activeFoods) { delete f; }
            activeFoods.clear();


            // initialize new game objects
            map = new Map(currentMapSize);            
            MapDimensions dims = getDimensions(currentMapSize);
            snake = new Snake(dims.width / 2, dims.height / 2);
            score = new Score(0);
            score -> startTime();
            dx = 1; dy = 0;
            spawnRandomFood();
            
            currentState = GameState::PLAYING;
            std::cout << "\nGame started! Use Arrow Keys to move." << std::endl;
        }

        if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS) { 
            glfwSetWindowShouldClose(window, GLFW_TRUE); // Closes openGL game window
        }
    }
    else if (currentState == GameState::PLAYING) {
        // prevent snake from moving into itself == disallowing reverse direction
        if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS && dy != -1)    { dx = 0; dy = 1; }
        if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS && dy != 1)   { dx = 0; dy = -1; }
        if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS && dx != 1)   { dx = -1; dy = 0; }
        if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS && dx != -1) { dx = 1; dy = 0; }
    }
}


void Game::gameOver(std::string message) {
    deathMessage = message;

    scoreCalculator();
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

void Game::update() {
    if (currentState != GameState::PLAYING) return;

    snake->updateBoost(); // decrement boost timer if active

    // calculate where the snake is trying to go
    int nextX = snake->getX() + dx;
    int nextY = snake->getY() + dy;


    // check Map Boundaries
    if (wrapAround) {         // TODO - does not work
        // Wrap logic: if we go out of bounds, appear on the other side
        if (nextX < 0) nextX = map->getWidth() - 1;
        if (nextX >= map->getWidth()) nextX = 0;
        if (nextY < 0) nextY = map->getHeight() - 1;
        if (nextY >= map->getHeight()) nextY = 0;

    } else {
        // Solid wall logic: die if we go out of bounds
        if (!map->isInside(nextX, nextY)) {
            gameOver("Ouch! You hit a wall!");
            return;
        }
    }

    // check Self-Collision
    if (snake->checkCollision(nextX, nextY)) {
        gameOver("Oh no! You ran into yourself!");
        return;
    }

    // Move the snake
    snake->move(dx, dy);


    // Check Food Collisions
    bool ateFood = false; // Flag to track if we need to spawn food
    for (auto it = activeFoods.begin(); it != activeFoods.end(); ) {
        if (snake->getX() == (*it)->x && snake->getY() == (*it)->y) {
            Food* eatenFood = *it;
            
            eatenFood->applyEffect(*snake);

            if (!eatenFood->isBomb()) {
                int pointsGained = 10 * snake->getScoreMultiplier();
                score->increase(pointsGained);
                
                std::cout << "Score: " << score->getScore();
                if (snake->getScoreMultiplier() > 1) {
                    // snake power up
                    audio->playSound(audio->powerupSoundPath.c_str());
                    std::cout << " (BOOST ACTIVE: " << snake->getScoreMultiplier() << "x!)";
                }
                std::cout << std::endl;
                score -> incrementFoodAmount();
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
        gameOver("KABOOM! You ate a bomb!");
    }

    if (ateFood) { // only spawn food if we ate something (don't want too much food on the map)
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
        textRenderer->render("Map size:", 30.0f, 40.0f, 1.0f, 1.0f, 1.0f, 0.0f); 
        
        textRenderer->render("Press 1: Small",  40.0f, 80.0f, 1.0f, 1.0f, (currentMapSize == MapSize::SMALL ? 0.4f : 1.0f), (currentMapSize == MapSize::SMALL ? 0.7f : 1.0f));
        textRenderer->render("Press 2: Medium", 40.0f, 110.0f, 1.0f, 1.0f, (currentMapSize == MapSize::MEDIUM ? 0.4f : 1.0f), (currentMapSize == MapSize::MEDIUM ? 0.7f : 1.0f));
        textRenderer->render("Press 3: Large",  40.0f, 140.0f, 1.0f, 1.0f, (currentMapSize == MapSize::LARGE ? 0.4f : 1.0f), (currentMapSize == MapSize::LARGE ? 0.7f : 1.0f));

        // --- WALL MODE SETTINGS ---
        textRenderer->render("Wall Mode:", 30.0f, 180.0f, 1.0f, 1.0f, 1.0f, 0.0f); 
        textRenderer->render("Press B : Bounded Walls (Die on hit)", 40.0f, 210.0f, 1.0f, 1.0f, (!wrapAround ? 0.4f : 1.0f), (!wrapAround ? 0.7f : 1.0f));
        textRenderer->render("Press L : Loop Walls (Wrap around)",   40.0f, 240.0f, 1.0f, 1.0f, (wrapAround ? 0.4f : 1.0f),  (wrapAround ? 0.7f : 1.0f));

        // --- INSTRUCTIONS ---
        textRenderer->render("Press SPACE to start", 250.0f, 550.0f, 1.5f, 1.0f, 1.0f, 1.0f); // White
        textRenderer->render("Press Q to quit", 330.0f, 580.0f, 1.0f, 1.0f, 1.0f, 1.0f); // White
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
        }

        // draw snake head (lighter than body)
        float headX = snake->getX();
        float headY = snake->getY();
        float r = snake->getR() + 0.2f;
        float g = snake->getG() + 0.2f;
        float b = snake->getB() + 0.2f;

        worldRenderer->drawQuad(headX, headY, r, g, b, currentMapSize);
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
        textRenderer->render("GAME OVER", 230.0f, 180.0f, 3.0f, 1.0f, 1.0f, 0.0f); // Big Yellow Text

        // small death msg
        textRenderer->render(deathMessage, 300.0f, 210.0f, 0.8f, 1.0f, 1.0f, 1.0f); // White text

        // game stats
        textRenderer->render("Final Score: " + std::to_string(score->getScore()), 310.0f, 310.0f, 1.0f, 1.0f, 1.0f, 1.0f);
        textRenderer->render("Fruit Eaten: " + std::to_string(score->getFoodAmount()), 310.0f, 340.0f, 1.0f, 1.0f, 1.0f, 1.0f);
        
        // controls
        textRenderer->render("Press ---- to save score", 260.0f, 450.0f, 1.0f, 1.0f, 1.0f, 1.0f);
        textRenderer->render("Press SPACE to try again", 200.0f, 500.0f, 1.5f, 1.0f, 1.0f, 1.0f);
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

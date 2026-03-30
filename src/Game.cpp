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
static std::string getMultiplayerWinnerMessage(std::string reason, const Score* score1, const Score* score2) {
    if (!score1 || !score2) return reason + " Head to Head! It's a Tie!";
    int s1 = score1->getScore();
    int s2 = score2->getScore();

    std::string winner;
    if (s1 > s2) {
        int diff = s1 - s2;
        winner = "Player 1 Wins by " + std::to_string(diff) + " points!";
    } 
    else if (s2 > s1) {
        int diff = s2 - s1;
        winner = "Player 2 Wins by " + std::to_string(diff) + " points!";
    } 
    else {
        winner = "It's a Tie in points!";
    }
    
    return reason + " " + winner;
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
    bool isSpacePressed = (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS);
    bool spaceJustPressed = (isSpacePressed && !spaceKeyWasPressed);
    spaceKeyWasPressed = isSpacePressed; // save state for next frame



    if (currentState == GameState::ADD_INITIALS) {
        // Connections from initials screen
        // Dont allow input choices until the Initals are filled up
        if (spaceJustPressed) currentState = GameState::START_SCREEN;
        return;
    }

    if (currentState == GameState::SCORE_BOARD) {
        if (spaceJustPressed) currentState = GameState::START_SCREEN;
        return;
    }

    if (currentState == GameState::GAME_OVER) {
         // Allows user to restart game
        if (spaceJustPressed) { start(); }
        // Allows users to quit the game
        if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS) { glfwSetWindowShouldClose(window, GLFW_TRUE); }

        // enter initials for score board
        if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
            playerInitials = "";
            currentState = GameState::ADD_INITIALS;
        }

        // view score board
        if (glfwGetKey(window, GLFW_KEY_T) == GLFW_PRESS) { currentState = GameState::SCORE_BOARD; }
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

        if (glfwGetKey(window, GLFW_KEY_T)) { currentState = GameState::SCORE_BOARD; }

        // --- multiplayer toggle ---
        bool isMPressed = (glfwGetKey(window, GLFW_KEY_M) == GLFW_PRESS);
        // only toggle if it is down now, AND it was NOT down last frame
        if (isMPressed && !mKeyWasPressed) {
            isMultiplayer = !isMultiplayer;
            std::cout << "Multiplayer: " << (isMultiplayer ? "ON" : "OFF") << std::endl;
            if (audio) audio->playSound(audio->selectSoundPath.c_str());
        }
        mKeyWasPressed = isMPressed; // save current state for next frame


        // --- start Game ---
        if (spaceJustPressed) startCountdown();

    }
    else if (currentState == GameState::PLAYING) {
        // prevent snake from moving into itself == disallowing reverse direction

        // Player 1 controls
        if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS && lastDy != -1)    { dx = 0; dy = 1; }
        if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS && lastDy != 1)   { dx = 0; dy = -1; }
        if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS && lastDx != 1)   { dx = -1; dy = 0; }
        if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS && lastDx != -1) { dx = 1; dy = 0; }

        // Player 2 controls
        if (isMultiplayer) {
            if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS && lastDy2 != -1) { dx2 = 0; dy2 = 1; }
            if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS && lastDy2 != 1)  { dx2 = 0; dy2 = -1; }
            if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS && lastDx2 != 1)  { dx2 = -1; dy2 = 0; }
            if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS && lastDx2 != -1) { dx2 = 1; dy2 = 0; }
        }
    }
}


void Game::gameOver(std::string reason) {
    // calculate scores before building message 
    score->endTime();
    score->calculateTime();
    score->timeAddedToFinal();
    
    if (isMultiplayer && score2) {
        score2->endTime();
        score2->calculateTime();
        score2->timeAddedToFinal();
    }

    // build death message with reason and score info
    deathMessage = isMultiplayer ? getMultiplayerWinnerMessage(reason, score, score2) : reason;

    std::cout << "\n---------------------------" << std::endl;
    std::cout << deathMessage << std::endl;
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
    if (currentState != GameState::PLAYING && currentState != GameState::COUNTDOWN) return;


    // --------- COUNTDOWN LOGIC (before game starts) ---------
    if (currentState == GameState::COUNTDOWN) {
        countdownTimer -= deltaTime;
        int currentSecond = (int)countdownTimer;
        
        // Play a sound every time the number changes
        if (currentSecond < lastCountdownSecond && currentSecond > 0) {
            if (audio) audio->playSound(audio->powerupSoundPath.c_str());
            lastCountdownSecond = currentSecond;
        }
        
        // When timer hits 0, start the real game!
        if (countdownTimer <= 0.0f) {
            startGameplay();
        }
        return; // Don't run any snake movement logic while counting down!
    }


    // --------- MAIN GAME LOGIC (game has started) ---------

    // --- decrement booster timer if active ---
    snake->updateBoost(deltaTime);
    if (isMultiplayer) snake2->updateBoost(deltaTime);


    // --- speed up slowly over time (every 5 seconds) ---
    globalSpeedTimer += deltaTime;
    if (globalSpeedTimer >= 5.0f) { 
        globalSpeedTimer = 0.0f; // reset timer
    }



    // ------------------------------------------
    //    MOVEMENT LOGIC and COLLISION CHECKS
    // ------------------------------------------

    // --- add time to movement timers ---
    p1MoveTimer += deltaTime;
    if (isMultiplayer) p2MoveTimer += deltaTime;

    // --- check WHO is moving this frame ---
    bool p1Moved = (p1MoveTimer >= snake->getSpeed());
    bool p2Moved = (isMultiplayer && p2MoveTimer >= snake2->getSpeed());
    if (!p1Moved && !p2Moved) return; // if no one moved

    // reset timers for whoever moved
    if (p1Moved) p1MoveTimer = 0.0f;
    if (p2Moved) p2MoveTimer = 0.0f;


    // --- Calculate Next Positions ---
    int nextX = p1Moved ? snake->getX() + dx : snake->getX();
    int nextY = p1Moved ? snake->getY() + dy : snake->getY();

    int nextX2 = (isMultiplayer) ? snake2->getX() : 0;
    int nextY2 = (isMultiplayer) ? snake2->getY() : 0;
    if (p2Moved) {
        nextX2 = snake2->getX() + dx2;
        nextY2 = snake2->getY() + dy2;
    }


    // --- Rule 1: Head-to-Head Collision ---
        // BOTH snakes moved into the exact same tile on the exact same frame
    if (isMultiplayer && p1Moved && p2Moved && nextX == nextX2 && nextY == nextY2) {
        if (score->getScore() > score2->getScore()) gameOver("Player 1 Wins! Head to Head with higher score.");
        else if (score2->getScore() > score->getScore()) gameOver("Player 2 Wins! Head to Head with higher score.");
        else gameOver("Head to Head! It's a Tie!");
        return;
    }

    // --- Rule 2: Hitting the other snake's body ---
    if (isMultiplayer) {
        if (p1Moved && snake2->checkCollision(nextX, nextY)) {
            gameOver("P1 crashed into P2!");
            return;
        }
        if (p2Moved && snake->checkCollision(nextX2, nextY2)) {
            gameOver("P2 crashed into P1!");
            return;
        }
    }


    // --- Map Boundaries & Self Collision ---
    if (p1Moved) {
        // Wall logic
        if (wrapAround) {
            if (nextX < 0) nextX = map->getWidth() - 1;
            else if (nextX >= map->getWidth()) nextX = 0;
            if (nextY < 0) nextY = map->getHeight() - 1;
            else if (nextY >= map->getHeight()) nextY = 0;
        } else if (!map->isInside(nextX, nextY)) {
            gameOver(isMultiplayer ? "P1 hit a wall!" : "Ouch! You hit a wall!");
            return;
        }
        // Self collision
        if (snake->checkCollision(nextX, nextY)) {
            gameOver(isMultiplayer ? "P1 ran into themselves!" : "Oh no! You ran into yourself!");
            return;
        }
        // move P1
        snake->move(nextX, nextY);
        lastDx = dx;
        lastDy = dy;
    }

    if (p2Moved) {
        // Wall logic
        if (wrapAround) {
            if (nextX2 < 0) nextX2 = map->getWidth() - 1;
            else if (nextX2 >= map->getWidth()) nextX2 = 0;
            if (nextY2 < 0) nextY2 = map->getHeight() - 1;
            else if (nextY2 >= map->getHeight()) nextY2 = 0;
        } else if (!map->isInside(nextX2, nextY2)) {
            gameOver("P2 hit a wall!");
            return;
        }
        // Self collision
        if (snake2->checkCollision(nextX2, nextY2)) {
            gameOver("P2 ran into themselves!");
            return;
        }
        // move P2
        snake2->move(nextX2, nextY2);
        lastDx2 = dx2;
        lastDy2 = dy2;
    }


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
                
                // snake powerup
                if (eatingSnake->getScoreMultiplier() > 1) {
                    // snake power up
                    audio->playSound(audio->powerupSoundPath.c_str());
                    std::cout << " (BOOST ACTIVE: " << eatingSnake->getScoreMultiplier() << "x!)";
                }

                // snake speed increase
                if (dynamic_cast<SpeedFood*>(eatenFood)) {
                    std::cout << " (SPEED UP! +30 bonus points!)";
                    pointsGained += 30;
                }

                eatingSnake->increaseSpeed(0.02f); // every food increases speed a little bit

                activeScore->increase(pointsGained);
                std::cout << "Score: " << activeScore->getScore() << std::endl;

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
            gameOver("KABOOM! P1 ate a bomb!");
        } else {
            gameOver("KABOOM! You ate a bomb!");
        }
        return;
    }
    if (isMultiplayer && snake2->isDead()) {
        gameOver("KABOOM! P2 ate a bomb!");
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
            // draw snake 2 body
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
        // P1 score - left side (green)
        textRenderer->render("P1: " + std::to_string(score->getScore()), 740.0f, 40.0f, 1.0f, 0.0f, 1.0f, 0.0f);

        // P2 score - right side (blue), only in multiplayer
        if (isMultiplayer && score2) {
            textRenderer->render("P2: " + std::to_string(score2->getScore()), 20.0f, 40.0f, 1.0f, 0.0f, 0.5f, 1.0f);
        }

        // if boost acitve, show boost timer (bottom middle)
        if (snake->getBoostTime() > 0.0f) {
            textRenderer->render("P1 SCORE BOOST: " + std::to_string((int)snake->getBoostTime()) + "s", 653.0f, 70.0f, 1.0f, 1.0f, 0.4f, 0.8f);
        }
        if (isMultiplayer && snake2 && snake2->getBoostTime() > 0.0f) {
            textRenderer->render("P2 SCORE BOOST: " + std::to_string((int)snake2->getBoostTime()) + "s", 20.0f, 70.0f, 1.0f, 1.0f, 0.4f, 0.8f);
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
        textRenderer->render(deathMessage, 0.0f, 220.0f, 0.6f, 1.0f, 1.0f, 1.0f, 1); // White text

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

    else if (currentState == GameState::ADD_INITIALS) {
        // Black screen
        for(int x = 0; x < mapWidth; x++) {
            for(int y = 0; y < mapHeight; y++) {
                worldRenderer->drawQuad(x, y, 0.0f, 0.0f, 0.0f, currentMapSize);
            }
        }

        // Text Desplayed
        textRenderer->render("ADD INITIALS", 250.0f, 50.0f, 2.5f, 0.0f, 1.0f, 0.0f); // Big Green Text
        textRenderer->render("3 letters only", 300.0f, 80.0f, 1.5f, 0.0f, 1.0f, 0.0f); // White
        // adding inital virkni --------------------- Þá adda í txt skjal eða annað sambærilegt

        std::string display = "";
        for (int i = 0; i < 3; i++) {
            display += (i < (int)playerInitials.length())? playerInitials[i] : '_';
            if (i < 2) display +=" ";
        }
        textRenderer->render("Initals: " + display, 270.0f, 300.0f, 2.0f, 1.0f, 1.0f, 1.0f); // White

        textRenderer->render("Press SPACE to Continue", 280.0f, 520.0f, 1.0f, 1.0f, 1.0f, 1.0f); // White
    }

    else if (currentState == GameState::SCORE_BOARD) {
        // Black screen
        for(int x = 0; x < mapWidth; x++) {
            for(int y = 0; y < mapHeight; y++) {
                worldRenderer->drawQuad(x, y, 0.0f, 0.0f, 0.0f, currentMapSize);
            }
        }

        // Text Desplayed
        textRenderer->render("TOP SCORES", 250.0f, 50.0f, 2.5f, 0.0f, 1.0f, 0.0f); // Big Green Text
        // adding top scores virkni --------------------- Sína alla top x marga stig
        showCurrTopScores();

        textRenderer->render("Press SPACE to go to Start Screen", 200.0f, 500.0f, 1.2f, 1.0f, 1.0f, 1.0f); // White

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

void Game::addInitialsToFile() {
    std::vector<ScoreEntry> topScores;
    std::string line;
    std::ifstream scoreFile("TopScores.txt");

    while (getline(scoreFile, line)) {
        size_t spacePosition = line.find(" ");
        if (spacePosition == std::string::npos) continue;
        std::string initials = line.substr(0, spacePosition);
        int s = std::stoi(line.substr(spacePosition + 1));
        topScores.push_back({initials, s});
    }
    scoreFile.close();

    // Add current player's score
    topScores.push_back({playerInitials, score->getScore()});

    std::sort(topScores.begin(), topScores.end(), [](const ScoreEntry& a, const ScoreEntry& b) {
        return a.score > b.score;
    });

    std::ofstream scoreFile2("TopScores.txt");
    for (int i = 0; i < std::min((int)topScores.size(), 10); i++) {
        scoreFile2 << topScores[i].initials << " " << topScores[i].score << "\n";
    }
}

void Game::showCurrTopScores() {
    std::string line;
    std::ifstream scoreFile("TopScores.txt");

    for (int i = 1; i < 6; i++) {
        if (!getline(scoreFile, line)) break; // stop if file has fewer than 5 entries
        textRenderer->render(std::to_string(i) + " " + line, 270.0f, ((i * 2) * 20) + 100.0f, 1.2f, 1.0f, 1.0f, 1.0f);
    }
}

void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    (void)scancode;
    (void)mods;

    if (action != GLFW_PRESS) return;

    Game* game = (Game*)glfwGetWindowUserPointer(window);
    if (game->currentState != GameState::ADD_INITIALS) return;
    if (game->playerInitials.length() >= 3) return;

    if (key >= GLFW_KEY_A && key <= GLFW_KEY_Z) {
        game->playerInitials += toupper(key);
        if (game->playerInitials.length() == 3) {
            game->addInitialsToFile();
        }
    }
}
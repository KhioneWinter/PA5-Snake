#include "Game.hpp"
#include "Map.hpp"
#include "Snake.hpp"
#include "Score.hpp"
#include "NormalFood.hpp"
#include "SpecialFood.hpp"
#include <cstdlib>
#include <iostream>


// Very basic shaders to draw solid colored squares
const char* vertexShaderSource = "#version 330 core\n"
    "layout (location = 0) in vec2 aPos;\n"
    "uniform vec2 offset;\n"
    "uniform vec2 size;\n"
    "void main()\n"
    "{\n"
    "   gl_Position = vec4(-1.0 + (aPos.x * size.x) + offset.x, -1.0 + (aPos.y * size.y) + offset.y, 0.0, 1.0);\n"
    "}\0";

const char* fragmentShaderSource = "#version 330 core\n"
    "out vec4 FragColor;\n"
    "uniform vec3 color;\n"
    "void main()\n"
    "{\n"
    "   FragColor = vec4(color, 1.0f);\n"
    "}\n\0";





    
Game::Game() : currentState(GameState::START_SCREEN), mapWidth(20), mapHeight(20), wrapAround(false), dx(1), dy(0) {
    // initalize game objects
    map = nullptr;
    snake = nullptr; 
    score = nullptr;

    // random seed
    randEngine.seed(std::chrono::system_clock::now().time_since_epoch().count());
}

Game::~Game() {
    // clean up memory
    delete map;
    delete snake;
    delete score;
}

void Game::start() {
    setupOpenGL();
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
    if (currentState == GameState::START_SCREEN || currentState == GameState::GAME_OVER) {
        
        // settings bindings
        if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS && mapWidth != 10) { mapWidth = 12; mapHeight = 10; std::cout << "Selected: SMALL MAP\n"; }
        if (glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS && mapWidth != 20) { mapWidth = 24; mapHeight = 20; std::cout << "Selected: MEDIUM MAP\n"; }
        if (glfwGetKey(window, GLFW_KEY_3) == GLFW_PRESS && mapWidth != 30) { mapWidth = 36; mapHeight = 30; std::cout << "Selected: LARGE MAP\n"; }
        
        if (glfwGetKey(window, GLFW_KEY_B) == GLFW_PRESS && wrapAround == true)  { wrapAround = false; std::cout << "Selected: BOUNDED WALLS\n"; }
        if (glfwGetKey(window, GLFW_KEY_L) == GLFW_PRESS && wrapAround == false) { wrapAround = true; std::cout << "Selected: LOOP WALLS\n"; }

        // start Game
        if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) {
            delete map; delete snake; delete score; // clean up old game if restarting
            
            map = new Map(mapWidth, mapHeight);
            snake = new Snake(mapWidth / 2, mapHeight / 2); // Start exactly in the middle!
            score = new Score(0);
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


void Game::update() {
    if (currentState != GameState::PLAYING) return;

    snake->updateBoost(); // decrement boost timer if active

    // calculate where the snake is trying to go
    int nextX = snake->getX() + dx;
    int nextY = snake->getY() + dy;

    // check Map Boundaries
    if (wrapAround) {
        // Wrap logic: if we go out of bounds, appear on the other side
        if (nextX < 0) nextX = map->getWidth() - 1;
        if (nextX >= map->getWidth()) nextX = 0;
        if (nextY < 0) nextY = map->getHeight() - 1;
        if (nextY >= map->getHeight()) nextY = 0;
    } else {
        // Solid wall logic: die if we go out of bounds
        if (!map->isInside(nextX, nextY)) {
            std::cout << "Game Over! Hit a wall. Score: " << score->getScore();
            std::cout << "\n---------------------------";
            std::cout << "\n Press SPACE to start again or Q to quit!" << std::endl;
            currentState = GameState::GAME_OVER; 
            return;
        }
    }

    // check Self-Collision
    if (snake->checkCollision(nextX, nextY)) {
        std::cout << "Game Over! You bit yourself. Score: " << score->getScore();
        std::cout << "\n---------------------------";
        std::cout << "\n Press SPACE to start again or Q to quit!" << std::endl;
        currentState = GameState::GAME_OVER; 
        return;
    }

    // Move the snake
    snake->move(dx, dy);

    // Check Food Collisions
    for (auto it = activeFoods.begin(); it != activeFoods.end(); ) {
        if (snake->getX() == (*it)->x && snake->getY() == (*it)->y) {
            Food* eatenFood = *it;
            eatenFood->applyEffect(*snake); 
            
            if (eatenFood->isBomb()) {
                std::cout << "KABOOM! You ate a bomb!" << std::endl;
            } else {
                int pointsGained = 10 * snake->getScoreMultiplier();
                score->increase(pointsGained);
                
                std::cout << "Score: " << score->getScore();
                if (snake->getScoreMultiplier() > 1) {
                    std::cout << " (BOOST ACTIVE: " << snake->getScoreMultiplier() << "x!)";
                }
                std::cout << std::endl;

                spawnRandomFood(); // Spawn new regular food
            }
            
            delete eatenFood;
            it = activeFoods.erase(it);
        } else {
            ++it;
        }
    }
    
    // Check if the snake died from a bomb
    if (snake->isDead()) {
        std::cout << "\n---------------------------";
        std::cout << "\n Press SPACE to start again or Q to quit!" << std::endl;
        currentState = GameState::GAME_OVER;
        return;
    }

}


void Game::render() {
    if (currentState == GameState::START_SCREEN) {
        // Draw a blue pattern for Start Screen
        int setValue = 6;
        if (mapWidth == 30 && mapHeight == 30) {
            setValue = 5; // Fixes so the big map is bigger then the medium map
        }
        for(int x = mapWidth / setValue; x < (mapWidth / setValue) * 4; x++) {
            for(int y = mapHeight / setValue; y < (mapHeight / setValue) * 4; y++) {
                drawQuad(x, y, 0.0f, 0.4f, 0.8f); // Colors blue
            }
        }
    }
    else if (currentState == GameState::PLAYING) {
        // Draw Map dynamically based on width and height
        for (int x = 0; x < mapWidth; x++) {
            for (int y = 0; y < mapHeight; y++) {
                if ((x+y) % 2 == 0) {
                    drawQuad(x, y, 0.20f, 0.20f, 0.20f); // Differend hues of gray
                }
                else {
                    drawQuad(x, y, 0.25f, 0.25f, 0.25f); // used to see grid better
                }
                
            } 
        }
        // Draw Food
        // drawQuad(currentFoodX, currentFoodY, 1.0f, 0.0f, 0.0f); 
        // draw all food
        for (Food* f : activeFoods) {
            drawQuad(f->x, f->y, f->r, f->g, f->b);
        }

        // draw Snake Body
        for (const auto& segment : snake->getBody()) {
            drawQuad(segment.first, segment.second, snake->getR(), snake->getG(), snake->getB()); 
        }

        // for (const auto& segment : snake->getBody()) {
        //     drawQuad(segment.first, segment.second, 0.0f, 0.8f, 0.0f); 
        // }
        // draw Snake Head  (lighter than body)
        drawQuad(snake->getX(), snake->getY(), snake->getR() + 0.2f, snake->getG() + 0.2f, snake->getB() + 0.2f);

        // drawQuad(snake->getX(), snake->getY(), 0.0f, 1.0f, 0.0f); 
    } 
    else if (currentState == GameState::GAME_OVER) {
        // Draw red Game Over screen
        for(int x = 0; x < mapWidth; x++) {
            for(int y = 0; y < mapHeight; y++) {
                drawQuad(x, y, 0.5f, 0.0f, 0.0f);
            }
        }
    }
}


void Game::setupOpenGL() {
    // Compile Vertex Shader
    unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    glCompileShader(vertexShader);

    // Compile Fragment Shader
    unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);

    // Link Shaders into a Program
    shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    // Define a 1x1 square
    float vertices[] = {
        0.0f, 0.0f,
        1.0f, 0.0f,
        1.0f, 1.0f,
        1.0f, 1.0f,
        0.0f, 1.0f,
        0.0f, 0.0f
    };

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
}


void Game::drawQuad(int gridX, int gridY, float r, float g, float b) {
    glUseProgram(shaderProgram);

    // Calculate dynamic cell sizes based on the chosen map dimensions
    float cellWidth = 2.0f / mapWidth;
    float cellHeight = 2.0f / mapHeight;

    // Calculate offsets based on grid position
    float offsetX = gridX * cellWidth;
    float offsetY = gridY * cellHeight;

    // Pass uniforms to the shader
    glUniform2f(glGetUniformLocation(shaderProgram, "size"), cellWidth, cellHeight);
    glUniform2f(glGetUniformLocation(shaderProgram, "offset"), offsetX, offsetY);
    glUniform3f(glGetUniformLocation(shaderProgram, "color"), r, g, b);

    glBindVertexArray(VAO);
    glDrawArrays(GL_TRIANGLES, 0, 6);
}


void Game::spawnRandomFood(bool forcePositive) {
    int fx, fy;
    map->spawnFood(fx, fy); // get location on map for new food
    
    int chance = randEngine() % 100; // custome random engine
    

    if (!forcePositive && chance < 15) {
        activeFoods.push_back(new BombFood(fx, fy)); 
        
        // RULE: There MUST always be a positive food on the board
        spawnRandomFood(true);  // immediatly spawn positive food
    }

    else if (chance < 30) {
        activeFoods.push_back(new SpeedFood(fx, fy)); // cyan color
    } 
    else if (chance < 45) {
        activeFoods.push_back(new ScoreIncreaseFood(fx, fy, 1.0f, 0.4f, 0.7f)); // pink color
    }
    else if (chance < 60) {
        // spawn color food with random color
        float colors[5][3] = {
            {0.0f, 0.0f, 1.0f}, // Blue
            {0.5f, 0.0f, 0.5f}, // Purple
            {1.0f, 0.4f, 0.7f}, // Pink
            {1.0f, 1.0f, 0.0f}  // Yellow
        };
        int c = randEngine() % 5;
        activeFoods.push_back(new ColorFood(fx, fy, colors[c][0], colors[c][1], colors[c][2]));
    } 
    else {
        activeFoods.push_back(new NormalFood(fx, fy)); // default food red
    }
}
#include "Game.hpp"
#include "Map.hpp"
#include "Snake.hpp"
#include "Score.hpp"
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
    std::cout << " Press SPACE to start!     " << std::endl;
}

void Game::handleInput(GLFWwindow* window) {
    if (currentState == GameState::START_SCREEN || currentState == GameState::GAME_OVER) {
        
        // settings bindings
        if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS && mapWidth != 10) { mapWidth = 10; mapHeight = 10; std::cout << "Selected: SMALL MAP\n"; }
        if (glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS && mapWidth != 20) { mapWidth = 20; mapHeight = 20; std::cout << "Selected: MEDIUM MAP\n"; }
        if (glfwGetKey(window, GLFW_KEY_3) == GLFW_PRESS && mapWidth != 30) { mapWidth = 30; mapHeight = 30; std::cout << "Selected: LARGE MAP\n"; }
        
        if (glfwGetKey(window, GLFW_KEY_B) == GLFW_PRESS && wrapAround == true)  { wrapAround = false; std::cout << "Selected: BOUNDED WALLS\n"; }
        if (glfwGetKey(window, GLFW_KEY_L) == GLFW_PRESS && wrapAround == false) { wrapAround = true; std::cout << "Selected: LOOP WALLS\n"; }

        // start Game
        if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) {
            delete map; delete snake; delete score; // clean up old game if restarting
            
            map = new Map(mapWidth, mapHeight);
            snake = new Snake(mapWidth / 2, mapHeight / 2); // Start exactly in the middle!
            score = new Score(0);
            dx = 1; dy = 0;
            map->spawnFood(currentFoodX, currentFoodY);
            
            currentState = GameState::PLAYING;
            std::cout << "\nGame started! Use Arrow Keys to move." << std::endl;
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
            std::cout << "Game Over! Hit a wall. Score: " << score->getScore() << std::endl;
            currentState = GameState::GAME_OVER; 
            return;
        }
    }

    // check Self-Collision
    if (snake->checkCollision(nextX, nextY)) {
        std::cout << "Game Over! You bit yourself. Score: " << score->getScore() << std::endl;
        currentState = GameState::GAME_OVER; 
        return;
    }

    // Move the snake
    snake->move(dx, dy);

    // Check Food Collision
    if (snake->getX() == currentFoodX && snake->getY() == currentFoodY) {
        snake->grow();
        score->increase(10);
        map->spawnFood(currentFoodX, currentFoodY); 
        std::cout << "Score: " << score->getScore() << std::endl;
    }
}


void Game::render() {
    if (currentState == GameState::START_SCREEN) {
        // Draw a blue pattern for Start Screen
        for(int x = mapWidth / 4; x < (mapWidth / 4) * 3; x++) {
            for(int y = mapHeight / 4; y < (mapHeight / 4) * 3; y++) {
                drawQuad(x, y, 0.0f, 0.4f, 0.8f); 
            }
        }
    } 
    else if (currentState == GameState::PLAYING) {
        // Draw Map dynamically based on width and height
        for (int x = 0; x < mapWidth; x++) {
            for (int y = 0; y < mapHeight; y++) {
                drawQuad(x, y, 0.2f, 0.2f, 0.2f);
            }
        }

        drawQuad(currentFoodX, currentFoodY, 1.0f, 0.0f, 0.0f); 

        for (const auto& segment : snake->getBody()) {
            drawQuad(segment.first, segment.second, 0.0f, 0.8f, 0.0f); 
        }

        drawQuad(snake->getX(), snake->getY(), 0.0f, 1.0f, 0.0f); 
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

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





    
Game::Game() : currentState(GameState::START_SCREEN), dx(1), dy(0) {
    // initalize game objects
    map = new Map(20, 20);      // create a 20x20 grid map
    snake = new Snake(10, 10);  // start snake in the middle
    score = new Score(0);       // start score at 0
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
    std::cout << " Press SPACE to start!     " << std::endl;
}


void Game::handleInput(GLFWwindow* window) {
    if (currentState == GameState::START_SCREEN || currentState == GameState::GAME_OVER) {

        // start the game when SPACE is pressed
        if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) {
            
            // reset the game objects for a fresh start
            delete snake;
            delete score;
            snake = new Snake(10, 10);
            score = new Score(0);
            dx = 1; dy = 0;
            map->spawnFood(currentFoodX, currentFoodY);
            
            currentState = GameState::PLAYING;
            std::cout << "Game started! Use Arrow Keys to move." << std::endl;
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
    if (!map->isInside(nextX, nextY)) {
        std::cout << "Game Over! Hit a wall. Score: " << score->getScore() << std::endl;
        std::cout << "Press SPACE to play again." << std::endl;
        currentState = GameState::GAME_OVER; 
        return;
    }

    // check Self-Collision
    if (snake->checkCollision(nextX, nextY)) {
        std::cout << "Game Over! You bit yourself. Score: " << score->getScore() << std::endl;
        std::cout << "Press SPACE to play again." << std::endl;
        currentState = GameState::GAME_OVER; 
        return;
    }

    // Move the snake
    snake->move(dx, dy);

    // Check Food Collision
    if (snake->getX() == currentFoodX && snake->getY() == currentFoodY) {
        snake->grow();
        score->increase(10);
        map->spawnFood(currentFoodX, currentFoodY); // Spawn new food
        std::cout << "Score: " << score->getScore() << std::endl;
    }
}


void Game::render() {
    if (currentState == GameState::START_SCREEN) {


        // d a simple pattern for the Start Screen
        for(int x = 5; x < 15; x++) {
            for(int y = 5; y < 15; y++) {
                drawQuad(x, y, 0.0f, 0.4f, 0.8f); // Nice Blue color
            }
        }
    } 

    else if (currentState == GameState::PLAYING) {
        // Draw Map (Dark grey squares)
        for (int x = 0; x < 20; x++) {
            for (int y = 0; y < 20; y++) {
                drawQuad(x, y, 0.2f, 0.2f, 0.2f);
            }
        }

        // Draw Food (Red square)
        drawQuad(currentFoodX, currentFoodY, 1.0f, 0.0f, 0.0f); 

        // Draw Snake Body (Light green squares)
        for (const auto& segment : snake->getBody()) {
            drawQuad(segment.first, segment.second, 0.0f, 0.8f, 0.0f); 
        }

        // Draw Snake Head (Bright green square)
        drawQuad(snake->getX(), snake->getY(), 0.0f, 1.0f, 0.0f); 
    } 
    
    else if (currentState == GameState::GAME_OVER) {
        // Draw a red screen
        for(int x = 0; x < 20; x++) {
            for(int y = 0; y < 20; y++) {
                drawQuad(x, y, 0.5f, 0.0f, 0.0f); // Dark Red color
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

    // Assuming a 20x20 map, each cell is 2.0/20.0 = 0.1 in OpenGL Coordinates
    float cellWidth = 2.0f / 20.0f;
    float cellHeight = 2.0f / 20.0f;

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


#define STB_TRUETYPE_IMPLEMENTATION // for text rendering

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

// shaders for text rendering
const char* textVertexShaderSource = "#version 330 core\n"
    "layout (location = 0) in vec4 vertex;\n" // <vec2 pos, vec2 tex>
    "out vec2 TexCoords;\n"
    "uniform vec2 screenSize;\n"
    "void main()\n"
    "{\n"
    "   float ndcX = (vertex.x / screenSize.x) * 2.0 - 1.0;\n"
    "   float ndcY = 1.0 - (vertex.y / screenSize.y) * 2.0;\n" // Invert Y 
    "   gl_Position = vec4(ndcX, ndcY, 0.0, 1.0);\n"
    "   TexCoords = vertex.zw;\n"
    "}\0";

const char* textFragmentShaderSource = "#version 330 core\n"
    "in vec2 TexCoords;\n"
    "out vec4 FragColor;\n"
    "uniform sampler2D text;\n"
    "uniform vec3 textColor;\n"
    "void main()\n"
    "{\n"
    "   vec4 sampled = vec4(1.0, 1.0, 1.0, texture(text, TexCoords).r);\n" 
    "   FragColor = vec4(textColor, 1.0) * sampled;\n"
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
    setupTextRendering();
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
        if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS && mapWidth != 10) { mapWidth = 12; mapHeight = 10; std::cout << "Selected: SMALL MAP\n"; }
        if (glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS && mapWidth != 20) { mapWidth = 24; mapHeight = 20; std::cout << "Selected: MEDIUM MAP\n"; }
        if (glfwGetKey(window, GLFW_KEY_3) == GLFW_PRESS && mapWidth != 30) { mapWidth = 36; mapHeight = 30; std::cout << "Selected: LARGE MAP\n"; }
        
        if (glfwGetKey(window, GLFW_KEY_B) == GLFW_PRESS && wrapAround == true)  { wrapAround = false; std::cout << "Selected: BOUNDED WALLS\n"; }
        if (glfwGetKey(window, GLFW_KEY_L) == GLFW_PRESS && wrapAround == false) { wrapAround = true; std::cout << "Selected: LOOP WALLS\n"; }

        // start Game
        if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) {
            delete map; delete snake; delete score; // clean up old game if restarting
            // clean up old food
            for (Food* f : activeFoods) { delete f; }
            activeFoods.clear();


            // initialize new game objects
            map = new Map(mapWidth, mapHeight);
            snake = new Snake(mapWidth / 2, mapHeight / 2); // Start exactly in the middle!
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
            scoreCalculator();
            std::cout << "Game Over! Hit a wall. Score: " << score->getScore();
            std::cout << "\n---------------------------";
            std::cout << "\n Press SPACE to start again or Q to quit!" << std::endl;
            currentState = GameState::GAME_OVER; 
            return;
        }
    }

    // check Self-Collision
    if (snake->checkCollision(nextX, nextY)) {
        scoreCalculator();
        std::cout << "Game Over! You bit yourself. Score: " << score->getScore();
        std::cout << "\n---------------------------";
        std::cout << "\n Press SPACE to start again or Q to quit!" << std::endl;
        currentState = GameState::GAME_OVER; 
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
            
            if (eatenFood->isBomb()) {
                std::cout << "\nKABOOM! You ate a bomb!" << std::endl;
            } else {
                int pointsGained = 10 * snake->getScoreMultiplier();
                score->increase(pointsGained);
                
                std::cout << "Score: " << score->getScore();
                if (snake->getScoreMultiplier() > 1) {
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
        std::cout << "\n---------------------------";
        std::cout << "\n Press SPACE to start again or Q to quit!" << std::endl;
        currentState = GameState::GAME_OVER;
        return;
    }

    if (ateFood) { spawnRandomFood();  } // only spawn food if we ate something (don't want too much food on the map)

}


void Game::render() {
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
                drawQuad(x, y, 0.0f, 0.4f, 0.8f); // Colors blue
            }
        }

        // draw text over the background
        renderText("SNAKE", 320.0f, 50.0f, 2.5f, 0.0f, 1.0f, 0.0f); // Big Green Text
        // controls
        renderText("Map size:", 10.0f, 30.0f, 1.0f, 1.0f, 1.0f, 0.0f); // Yellow
        renderText("Press 1 for Small", 10.0f, 55.0f, 0.8f, 1.0f, 1.0f, 0.0f); 
        renderText("Press 2 for Medium", 10.0f, 80.0f, 0.8f, 1.0f, 1.0f, 0.0f);
        renderText("Press 3 for Large", 10.0f, 105.0f, 0.8f, 1.0f, 1.0f, 0.0f);

        // instructions
        renderText("Press SPACE to start", 250.0f, 550.0f, 1.5f, 1.0f, 1.0f, 1.0f); // White
        renderText("Press Q to quit", 330.0f, 580.0f, 1.0f, 1.0f, 1.0f, 1.0f); // White
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
        // ------------------ Draw Food ------------------
        for (Food* f : activeFoods) {
            drawQuad(f->x, f->y, f->r, f->g, f->b);
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
            drawQuad(body[i].first, body[i].second, 
                    snake->getR() * taper, 
                    snake->getG() * taper, 
                    snake->getB() * taper);
        }

        // draw snake head (lighter than body)
        float headX = snake->getX();
        float headY = snake->getY();
        float r = snake->getR() + 0.2f;
        float g = snake->getG() + 0.2f;
        float b = snake->getB() + 0.2f;

        drawQuad(headX, headY, r, g, b);
        // ------------------------------------------------------------------

    
        // ------------------ draw Score (top left) ------------------
        renderText("SCORE: " + std::to_string(score->getScore()), 20.0f, 40.0f, 1.0f, 1.0f, 1.0f, 1.0f);
        
        // if boost acitve, show boost timer (bottom middle)
        if (snake->getScoreMultiplier() > 1) {
            renderText("SCORE BOOST ACTIVE: " + std::to_string(snake->getBoostTime()), 20.0f, 70.0f, 1.0f, 1.0f, 0.5f, 1.0f); // Pink
        }
    } 

    else if (currentState == GameState::GAME_OVER) {
        // Draw red Game Over screen
        for(int x = 0; x < mapWidth; x++) {
            for(int y = 0; y < mapHeight; y++) {
                drawQuad(x, y, 0.5f, 0.0f, 0.0f);
            }
        }

        // draw Game Over Text
        renderText("GAME OVER", 230.0f, 200.0f, 3.0f, 1.0f, 1.0f, 0.0f); // Big Yellow Text
        renderText("Final Score: " + std::to_string(score->getScore()), 310.0f, 230.0f, 1.0f, 1.0f, 1.0f, 1.0f); // Final Score in white
        renderText("Fruit Eaten: " + std::to_string(score -> getFoodAmount()), 310.0f, 260.0f, 1.0f, 1.0f, 1.0f, 1.0f);
        renderText("Press ---- to save score", 260.0f, 450.0f, 1.0f, 1.0f, 1.0f, 1.0f);
        renderText("Press SPACE to try again", 200.0f, 500.0f, 1.5f, 1.0f, 1.0f, 1.0f);
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

void Game::setupTextRendering() {
    // compile Shaders
    unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &textVertexShaderSource, NULL);
    glCompileShader(vertexShader);

    unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &textFragmentShaderSource, NULL);
    glCompileShader(fragmentShader);

    textShaderProgram = glCreateProgram();
    glAttachShader(textShaderProgram, vertexShader);
    glAttachShader(textShaderProgram, fragmentShader);
    glLinkProgram(textShaderProgram);

    glDeleteShader(vertexShader); glDeleteShader(fragmentShader);

    // Load Font and Bake Texture
    unsigned char* ttf_buffer = new unsigned char[1<<20];
    FILE* f = fopen("Micro5-Regular.ttf", "rb");
    if (!f) {
        std::cout << "CRITICAL ERROR: Could not find font.ttf in your root folder!" << std::endl;
    } else {
        fread(ttf_buffer, 1, 1<<20, f);
        fclose(f);
        
        unsigned char* temp_bitmap = new unsigned char[512*512];
        stbtt_BakeFontBitmap(ttf_buffer, 0, 32.0, temp_bitmap, 512, 512, 32, 96, cdata);
        
        glGenTextures(1, &fontTexture);
        glBindTexture(GL_TEXTURE_2D, fontTexture);
        glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, 512, 512, 0, GL_RED, GL_UNSIGNED_BYTE, temp_bitmap);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        
        delete[] temp_bitmap;
    }
    delete[] ttf_buffer;

    // Setup VAO/VBO
    glGenVertexArrays(1, &textVAO);
    glGenBuffers(1, &textVBO);
    glBindVertexArray(textVAO);
    glBindBuffer(GL_ARRAY_BUFFER, textVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6 * 4, NULL, GL_DYNAMIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), 0);
    glBindBuffer(GL_ARRAY_BUFFER, 0); glBindVertexArray(0);
}

void Game::renderText(const std::string& text, float x, float y, float scale, float r, float g, float b) {
    glUseProgram(textShaderProgram);
    glUniform3f(glGetUniformLocation(textShaderProgram, "textColor"), r, g, b);

    
    // dynamically get the real screen size for Mac
    GLint vp[4];
    glGetIntegerv(GL_VIEWPORT, vp);
    glUniform2f(glGetUniformLocation(textShaderProgram, "screenSize"), (float)vp[2], (float)vp[3]); 
    
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, fontTexture);
    glBindVertexArray(textVAO);
    
    // Enable blending for transparent text backgrounds
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    for (char raw_c : text) {
        unsigned char c = (unsigned char)raw_c; // Prevents the Mac ARM warning
        if (c >= 32 && c < 128) {
            stbtt_aligned_quad q;
            float dummyX = 0; float dummyY = 0;
            
            stbtt_GetBakedQuad(cdata, 512, 512, c - 32, &dummyX, &dummyY, &q, 1);
            
            float x0 = x + q.x0 * scale; float y0 = y + q.y0 * scale;
            float x1 = x + q.x1 * scale; float y1 = y + q.y1 * scale;
            
            float vertices[6][4] = {
                { x0, y0, q.s0, q.t0 }, { x1, y0, q.s1, q.t0 }, { x1, y1, q.s1, q.t1 },
                { x1, y1, q.s1, q.t1 }, { x0, y1, q.s0, q.t1 }, { x0, y0, q.s0, q.t0 }
            };
            
            x += dummyX * scale; // Move cursor right
            
            glBindBuffer(GL_ARRAY_BUFFER, textVBO);
            glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
            glDrawArrays(GL_TRIANGLES, 0, 6);
        }
    }
    glBindVertexArray(0);
    glDisable(GL_BLEND);
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

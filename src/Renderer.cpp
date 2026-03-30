#include "Renderer.hpp"
#include <iostream>


// -------------- SHADERS --------------
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

// ----------------------------------------------------------------------



Renderer::Renderer() : shaderProgram(0), VAO(0), VBO(0) {}

Renderer::~Renderer() {
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteProgram(shaderProgram);
}

void Renderer::init() {
    if (VAO != 0) return; // Already initialized!

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

void Renderer::drawQuad(int gridX, int gridY, float r, float g, float b, MapSize currentSize) {
    glUseProgram(shaderProgram);

    MapDimensions dims = getDimensions(currentSize);
    int mapWidth = dims.width;
    int mapHeight = dims.height;

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
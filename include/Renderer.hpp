#ifndef RENDERER_HPP
#define RENDERER_HPP

#include <glad/glad.h>
#include "Map.hpp"

class Renderer {
private:
    unsigned int shaderProgram;
    unsigned int VAO, VBO;

public:
    Renderer();
    ~Renderer();
    void init();
    void drawQuad(int gridX, int gridY, float r, float g, float b, MapSize currentSize);
};

#endif
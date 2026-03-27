#ifndef TEXT_RENDERER_HPP
#define TEXT_RENDERER_HPP

#include <glad/glad.h>
#include <string>
#include "stb_truetype.h"

class TextRenderer {
private:
    unsigned int shaderProgram;
    unsigned int VAO, VBO;
    unsigned int fontTexture;
    stbtt_bakedchar cdata[96];

public:
    TextRenderer();
    ~TextRenderer();
    void init(const char* fontPath);
    void render(const std::string& text, float x, float y, float scale, float r, float g, float b, int alignment = 0);
};

#endif
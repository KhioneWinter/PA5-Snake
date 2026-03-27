#define STB_TRUETYPE_IMPLEMENTATION // for text rendering

#include "TextRenderer.hpp"
#include <iostream>

// -------------- SHADERS --------------
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
// ----------------------------------------------------------------------


TextRenderer::TextRenderer() : shaderProgram(0), VAO(0), VBO(0), fontTexture(0) {}

TextRenderer::~TextRenderer() {
    // Cleanup OpenGL resources if they were created
    if (fontTexture) {
        glDeleteTextures(1, &fontTexture);
    }
    if (VAO) {
        glDeleteVertexArrays(1, &VAO);
    }
    if (VBO) {
        glDeleteBuffers(1, &VBO);
    }
    if (shaderProgram) {
        glDeleteProgram(shaderProgram);
    }
}


// Load font file, bake bitmap, and setup texture/VAO
void TextRenderer::init(const char* fontPath) {
    // compile Shaders
    unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &textVertexShaderSource, NULL);
    glCompileShader(vertexShader);

    unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &textFragmentShaderSource, NULL);
    glCompileShader(fragmentShader);

    shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);

    glDeleteShader(vertexShader); glDeleteShader(fragmentShader);

    // Load Font and Bake Texture
    unsigned char* ttf_buffer = new unsigned char[1<<20];
    FILE* f = fopen(fontPath, "rb");
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
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6 * 4, NULL, GL_DYNAMIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), 0);
    glBindBuffer(GL_ARRAY_BUFFER, 0); glBindVertexArray(0);
}


void TextRenderer::render(const std::string& text, float x, float y, float scale, float r, float g, float b, int alignment) {
    glUseProgram(shaderProgram);
    glUniform3f(glGetUniformLocation(shaderProgram, "textColor"), r, g, b);


    // get the CURRENT actual window size in pixels
    GLint vp[4];
    glGetIntegerv(GL_VIEWPORT, vp);
    float screenWidth = (float)vp[2];
    float screenHeight = (float)vp[3];
    glUniform2f(glGetUniformLocation(shaderProgram, "screenSize"), screenWidth, screenHeight);

    // --- responsive UI scaling ---
        // 800x600 screen
    float scaleX = screenWidth / 800.0f;
    float scaleY = screenHeight / 600.0f;
    x *= scaleX;
    y *= scaleY;
    scale *= std::min(scaleX, scaleY); // multiply font scale

    // Calculate text width for alignment
    float totalWidth = 0.0f;
    for (char raw_c : text) {
        unsigned char c = (unsigned char)raw_c;
        if (c >= 32 && c < 128) {
            stbtt_aligned_quad q;
            float dummyX = 0; float dummyY = 0;
            stbtt_GetBakedQuad(cdata, 512, 512, c - 32, &dummyX, &dummyY, &q, 1);
            totalWidth += dummyX * scale;
        }
    }

    // Adjust x for alignment
    if (alignment == 1) { // center
        x = (screenWidth / 2.0f) - (totalWidth / 2.0f);
    } else if (alignment == 2) { // right
        x = screenWidth - totalWidth - x; // assuming x was the margin from right
    }
    // alignment 0 is left, no change


    // bind texture and VAO
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, fontTexture);
    glBindVertexArray(VAO);
    
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
            
            glBindBuffer(GL_ARRAY_BUFFER, VBO);
            glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
            glDrawArrays(GL_TRIANGLES, 0, 6);
        }
    }
    glBindVertexArray(0);
    glDisable(GL_BLEND);
}
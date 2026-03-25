#ifndef SNAKE_HPP
#define SNAKE_HPP


#include <vector>

class Snake {
private:
    int x, y; // head position
    int length;
    int speed;

    int directionX, directionY; // current movement direction

    // TODO i dont know -- fix
    std::vector<std::pair<int, int>> body;

    bool dead = false;
    float r = 0.0f, g = 0.8f, b = 0.0f; // default color : green
public:

    Snake(int x, int y) : x(x), y(y), length(1), speed(1) {}
    ~Snake() = default;

    void move(int dx, int dy); // TODO implement
    void grow(); // TODO implement
    bool checkCollision(int newX, int newY); // TODO implement

    void update();
    void render();

    int getX() const { return x; }
    int getY() const { return y; }
    std::vector<std::pair<int, int>> getBody() const { return body; } // need for rendering


    void kill() { dead = true; }
    bool isDead() const { return dead; }
    
    void increaseSpeed() { speed++; }
    
    void setColor(float newR, float newG, float newB) { r = newR; g = newG; b = newB; }
    float getR() const { return r; }
    float getG() const { return g; }
    float getB() const { return b; }
};

#endif // SNAKE_HPP
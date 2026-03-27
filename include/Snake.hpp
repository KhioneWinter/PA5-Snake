#ifndef SNAKE_HPP
#define SNAKE_HPP


#include <vector>

class Snake {
private:
    int x, y; // head position
    int length;
    float speed;

    int scoreMultiplier = 1; // default 1 point per food
    float boostTimer = 0;      // How many ticks the boost lasts

    int directionX, directionY; // current movement direction

    std::vector<std::pair<int, int>> body;

    bool dead = false;
    float r = 0.0f, g = 0.8f, b = 0.0f; // default color : green
public:

    Snake(int x, int y) : x(x), y(y), length(1), speed(0.15f) {}
    ~Snake() = default;

    void update();
    void render();

    // movement and growth
    void move(int newX, int newY);
    void grow();
    bool checkCollision(int newX, int newY);

    // get pos
    int getX() const { return x; }
    int getY() const { return y; }
    std::vector<std::pair<int, int>> getBody() const { return body; } // need for rendering
    float getSpeed() { return speed; }

    // death management
    void kill() { dead = true; }
    bool isDead() const { return dead; }
    
    // booost management
    void increaseSpeed(float amount = 0.01f) {
        if (amount < 0) return; if (speed < 0.05f) speed = 0.05f;
        speed -= amount;
        if (speed < 0.05f) speed = 0.05f; // cap max speed
    }
    void applyScoreBoost(int multiplier, int duration) { scoreMultiplier = multiplier; boostTimer = duration; }
    int getBoostTime() const { return boostTimer; }
    int getScoreMultiplier() const { return scoreMultiplier; }
    void updateBoost(float deltaTime);
    
    // color management
    void setColor(float newR, float newG, float newB) { r = newR; g = newG; b = newB; }
    float getR() const { return (scoreMultiplier > 1) ? 1.0f : r; }
    float getG() const { return (scoreMultiplier > 1) ? 0.4f : g; }
    float getB() const { return (scoreMultiplier > 1) ? 0.7f : b; }
};

#endif // SNAKE_HPP
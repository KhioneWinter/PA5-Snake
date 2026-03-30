#ifndef SNAKE_HPP
#define SNAKE_HPP


#include <vector>

class Snake {
private:
    int x, y; // head position
    int length;
    float speed;

    int directionX, directionY; // current movement direction
    std::vector<std::pair<int, int>> body;

    bool dead = false;
    float r = 0.0f, g = 0.8f, b = 0.0f; // default color : green

    // --- for ScoreIncreaseFood ---
    int scoreMultiplier = 1; // default 1 point per food
    float boostTimer = 0;      // How many ticks the boost lasts

    // --- for SpeedFood  ---
    float speedBoostTimer = 0.0f;
    float currentSpeedBoost = 0.0f;

public:

    Snake(int x, int y) : x(x), y(y), length(1), speed(0.28f) {}
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
    float getSpeed() { 
        float effectiveSpeed = speed - currentSpeedBoost;
        return (effectiveSpeed < 0.04f) ? 0.04f : effectiveSpeed; // speed cap
    }

    
    // --- death management ---
    void kill() { dead = true; }
    bool isDead() const { return dead; }
    
    // --- boost management ---
    void increaseSpeed(float amount = 0.01f) {
        if (amount < 0) return; 
        speed -= amount;
        if (speed < 0.04f) speed = 0.04f; // cap max speed
    }
    void applyScoreBoost(int multiplier, int duration) {
        scoreMultiplier = multiplier; boostTimer = duration;
    }
    void applySpeedBoost(float boostAmount, float duration) {
        currentSpeedBoost = boostAmount;
        speedBoostTimer = duration;
    }

    int getBoostTime() const { return boostTimer; }
    int getSpeedBoostTime() const { return (int) speedBoostTimer; }
    int getScoreMultiplier() const { return scoreMultiplier; }
    void updateBoost(float deltaTime);
    
    // color management
    void setColor(float newR, float newG, float newB) { r = newR; g = newG; b = newB; }
    float getR() const {
        if (scoreMultiplier > 1 && speedBoostTimer > 0.0f) return 0.8f; // White if BOTH active
        if (scoreMultiplier > 1) return 1.0f;                           // Pink for score boost
        if (speedBoostTimer > 0.0f) return 0.0f;                        // Cyan for speed boost
        return r;
    }
    float getG() const {
        if (scoreMultiplier > 1 && speedBoostTimer > 0.0f) return 0.8f; // White if BOTH active
        if (scoreMultiplier > 1) return 0.4f;                           // Pink for score boost
        if (speedBoostTimer > 0.0f) return 0.8f;                        // Cyan for speed boost
        return g;
    }
    float getB() const {
        if (scoreMultiplier > 1 && speedBoostTimer > 0.0f) return 0.8f; // White if BOTH active
        if (scoreMultiplier > 1) return 1.0f;                           // Pink for score boost
        if (speedBoostTimer > 0.0f) return 0.8f;                        // Cyan for speed boost
        return b;
    }
};

#endif // SNAKE_HPP
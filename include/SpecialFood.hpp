#ifndef SPECIAL_FOOD_HPP
#define SPECIAL_FOOD_HPP


#include "Food.hpp"
class Snake;

class SpecialFood : public Food {
public:
    SpecialFood(int x, int y) : Food(x, y) {}
    void applyEffect(Snake& snake) override; // TODO implement
};


// Bomb - kills the snake on collision (black color)
class BombFood : public Food {
public:
    BombFood(int x, int y) : Food(x, y, 0.0f, 0.0f, 0.0f) {}
    void applyEffect(Snake& snake) override;
    bool isBomb() const override { return true; }
};

// Speed Food - increase snake speed (Cyan)
class SpeedFood : public Food {
public:
    SpeedFood(int x, int y) : Food(x, y, 0.0f, 1.0f, 1.0f) {}
    void applyEffect(Snake& snake) override;
};

// Score increase food - increases score by X points for Y time (pink)
class ScoreIncreaseFood : public Food {
public:
    int increaseAmount; // how much score to increase
    int duration;       // how long the effect lasts in game ticks

    ScoreIncreaseFood(int x, int y, float r, float g, float b) : Food(x, y, r, g, b) {}
    void applyEffect(Snake& snake) override;
};

// Color Food - changes the snake's color (Dynamic Color)
class ColorFood : public Food {
public:
    ColorFood(int x, int y, float r, float g, float b) : Food(x, y, r, g, b) {}
    void applyEffect(Snake& snake) override;
};



#endif // SPECIAL_FOOD_HPP
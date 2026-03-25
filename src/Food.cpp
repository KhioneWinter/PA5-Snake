#include "SpecialFood.hpp"
#include "NormalFood.hpp"
#include "Snake.hpp"

// Normal Food -  all food makes snake grow
void NormalFood::applyEffect(Snake& snake) {
    snake.grow();
}

// Bomb - kills you instantly
void BombFood::applyEffect(Snake& snake) {
    snake.kill();
}

// Speed Food - increases your speed multiplier
void SpeedFood::applyEffect(Snake& snake) {
    snake.grow();
    snake.increaseSpeed();
}

// Score increase food - increases score that fruit give you for Y time
void ScoreIncreaseFood::applyEffect(Snake& snake) {
    snake.grow();
    snake.increaseScore(this->increaseAmount);
}

// Color food - changes your body color
void ColorFood::applyEffect(Snake& snake) {
    snake.grow();
    snake.setColor(this->r, this->g, this->b); // set snake to the fruit's color!
}
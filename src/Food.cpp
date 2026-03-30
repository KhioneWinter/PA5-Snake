#include "SpecialFood.hpp"
#include "NormalFood.hpp"
#include "Snake.hpp"

// Normal Food -  all food makes snake grow
void NormalFood::applyEffect(Snake& snake) {
    snake.grow();
    snake.increaseSpeed(0.01f); // onlly LITTLE bit
}

// Bomb - kills you instantly
void BombFood::applyEffect(Snake& snake) {
    snake.kill();
}

// Speed Food - increases your speed multiplier
void SpeedFood::applyEffect(Snake& snake) {
    snake.grow();
    snake.applySpeedBoost(0.09f, 10.0f); // 0.06f boost for 10 seconds
}

// Score increase food - increases score multiplier for Y time
void ScoreIncreaseFood::applyEffect(Snake& snake) {
    snake.grow();
    // Apply a 3x multiplier that lasts for 50 frames!
    snake.applyScoreBoost(increaseAmount, duration);
    snake.increaseSpeed(0.01f); // onlly LITTLE bit
}

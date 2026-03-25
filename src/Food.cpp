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

// Score increase food - increases score multiplier for Y time
void ScoreIncreaseFood::applyEffect(Snake& snake) {
    snake.grow();
    // Apply a 3x multiplier that lasts for 50 frames!
    snake.applyScoreBoost(increaseAmount, duration); 
}

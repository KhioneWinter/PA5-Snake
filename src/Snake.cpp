#include "Snake.hpp"
#include <cstddef>  // for size_t


void Snake::move(int newX, int newY) {
    // 1. insert the current head position into the front of the body list
    body.insert(body.begin(), {x, y});

    // 2. update the head position to the new coordinates
    x = newX;
    y = newY;

    // 3. if the snake hasn't grown, remove the tail segment so it visually moves forward
    if (body.size() > static_cast<size_t>(length)) body.pop_back();
}

void Snake::grow() {
    // increase snake length
    length++;
}

bool Snake::checkCollision(int newX, int newY) {
    // loop through the body segments to see if the new head position hits them
    for (const auto& segment : body) {
        if (segment.first == newX && segment.second == newY) {
            return true; // collision detected
        }
    }
    return false;
}


void Snake::updateBoost(float deltaTime) {
    // --- for Increase Score boost ---
    if (boostTimer > 0.0f) {
        boostTimer -= deltaTime; // subtract actual seconds passed
        if (boostTimer <= 0.0f) {
            boostTimer = 0.0f;
            scoreMultiplier = 1; 
            // reset snake color if needed
            setColor(0.0f, 0.8f, 0.0f);  // default green
        }
    }

    // --- for Speed boost ---
    if (speedBoostTimer > 0.0f) {
        speedBoostTimer -= deltaTime;
        if (speedBoostTimer <= 0.0f) {
            speedBoostTimer = 0.0f;
            currentSpeedBoost = 0.0f; // turn off speed boost
            setColor(0.0f, 0.8f, 0.0f);  // default green
        }
    }
}
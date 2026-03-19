#include "Snake.hpp"

void Snake::move(int dx, int dy) {
    // 1. insert the current head position into the front of the body list
    body.insert(body.begin(), {x, y});

    // 2. update the head position to the new coordinates
    x += dx;
    y += dy;

    // 3. if the snake hasn't grown, remove the tail segment so it visually moves forward
    if (body.size() > (size_t)length) body.pop_back();
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

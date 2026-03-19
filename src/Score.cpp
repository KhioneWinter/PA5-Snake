#include "Score.hpp"

Score::Score(int s) : score(s) {}

int Score::getScore() const { 
    return score; 
}

void Score::increase(int amount) { 
    score += amount; 
}

void Score::reset() { 
    score = 0; 
}
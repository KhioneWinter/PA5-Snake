#include "Score.hpp"


Score::Score(int s) : score(s) {}

void Score::startTime() {
    // Gets time when game starts
    start = time(NULL);
}

void Score::endTime() {
    // Gets time after game finishes
    end = time(NULL);
}

void Score::calculateTime() {
    // Calculates the added multiplyer to the players score
    double timeTaken = difftime(end, start);
    float maxTimeMin = 30.0f; // Might have to change for the longer games played
    float ratio = 1.0f - (float)(timeTaken / maxTimeMin);
    if (ratio < 0.0f) {
        ratio = 0.0f;
    }
    multiplyer = 1.0f + ratio;
    std::cout << "\nTime multiplyer added to score: "<< multiplyer << "\n";
}

int Score::getScore() const { 
    // Gets current score
    return score; 
}

void Score::increase(int amount) { 
    // Increases current score by chosen amount
    score += amount; 
}

void Score::timeAddedToFinal() {
    // Multiplyer added to the users final score
    score = (unsigned int)(score * multiplyer);
}

void Score::reset() { 
    // Resets score for new game
    score = 0; 
}

int Score::getFoodAmount() {
    return foodAmount;
}

void Score::incrementFoodAmount() {
    foodAmount++;
}

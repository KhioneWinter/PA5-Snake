#ifndef SCORE_HPP
#define SCORE_HPP
#include <ctime>
#include <iostream>


class Score {
private:
    int score;
    time_t start;
    time_t end;
    float multiplyer;
    int foodAmount = 0;

public:
    Score(int s);
    void startTime();
    void endTime();
    int getScore() const;
    void calculateTime();
    void increase(int amount);
    void reset();
    void timeAddedToFinal();
    int getFoodAmount();
    void incrementFoodAmount();
};


#endif // SCORE_HPP
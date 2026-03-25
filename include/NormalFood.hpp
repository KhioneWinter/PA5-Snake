#ifndef NORMAL_FOOD_HPP
#define NORMAL_FOOD_HPP


#include "Food.hpp"
class Snake;

class NormalFood : public Food {
public:
    // Normal food is red and increases score by 10 points
    NormalFood(int x, int y) : Food(x, y, 1.0f, 0.0f, 0.0f) {}
    void applyEffect(Snake& snake) override;
};


#endif // NORMAL_FOOD_HPP
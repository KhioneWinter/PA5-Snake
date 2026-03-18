#ifndef NORMAL_FOOD_HPP
#define NORMAL_FOOD_HPP


#include "Food.hpp"
class Snake;

class NormalFood : public Food {
public:
    NormalFood(int x, int y) : Food(x, y) {}
    void applyEffect(Snake& snake) override; // TODO implement
};


#endif // NORMAL_FOOD_HPP
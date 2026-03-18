#ifndef SPECIAL_FOOD_HPP
#define SPECIAL_FOOD_HPP


#include "Food.hpp"
class Snake;

class SpecialFood : public Food {
public:
    SpecialFood(int x, int y) : Food(x, y) {}
    void applyEffect(Snake& snake) override; // TODO implement
};

#endif // SPECIAL_FOOD_HPP
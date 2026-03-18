#ifndef FOOD_HPP
#define FOOD_HPP


class Snake;

class Food {
public:
    int x, y;


    Food(int x, int y) : x(x), y(y) {}
    ~Food() = default;

    virtual void applyEffect(Snake& snake) = 0;
};


#endif // FOOD_HPP
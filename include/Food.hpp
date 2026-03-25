#ifndef FOOD_HPP
#define FOOD_HPP


class Snake;

class Food {
public:
    int x, y;
    float r, g, b; // color for rendering


    Food(int x, int y, float r, float g, float b) : x(x), y(y), r(r), g(g), b(b) {}
    virtual ~Food() = default;

    virtual void applyEffect(Snake& snake) = 0;

    virtual bool isBomb() const { return false; }
};


#endif // FOOD_HPP
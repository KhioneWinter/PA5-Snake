#ifndef SNAKE_HPP
#define SNAKE_HPP


#include <vector>

class Snake {
private:
    int x, y; // head position
    int length;
    int speed;

    int directionX, directionY; // current movement direction

    // TODO i dont know -- fix
    std::vector<std::pair<int, int>> body;
public:

    Snake(int x, int y) : x(x), y(y), length(1), speed(1) {}
    ~Snake() = default;

    void move(int dx, int dy); // TODO implement
    void grow(); // TODO implement
    bool checkCollision(int newX, int newY); // TODO implement

    void update();
    void render();

    int getX() const { return x; }
    int getY() const { return y; }
    std::vector<std::pair<int, int>> getBody() const { return body; } // need for rendering
};

#endif // SNAKE_HPP
#ifndef SNAKE_SEGMENT_HPP
#define SNAKE_SEGMENT_HPP

class SnakeSegment {
private:
    int x, y;

public:
    SnakeSegment(int x, int y) : x(x), y(y) {}

    int getX() const { return x; }
    int getY() const { return y; }
};


#endif // SNAKE_SEGMENT_HPP
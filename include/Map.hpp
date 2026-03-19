#ifndef MAP_HPP
#define MAP_HPP


class Map {
private:
    int width;
    int height;

public:
    Map(int w, int h) : width(w), height(h) {}
    ~Map() = default;

    int getWidth() const { return width; }
    int getHeight() const { return height; }

    bool isInside(int x, int y);
    bool spawnFood(int &x, int &y);
    bool removeFood(int x, int y);
};


#endif // MAP_HPP
#ifndef MAP_HPP
#define MAP_HPP


class Map {
private:
    int width;
    int height;
public:
    Map(int w, int h) : width(w), height(h) {}
    ~Map() = default;

    bool isInside(int x, int y); // TODO implement
    bool spawnFood(int &x, int &y); // TODO implement
    bool removeFood(int x, int y); // TODO implement
};


#endif // MAP_HPP
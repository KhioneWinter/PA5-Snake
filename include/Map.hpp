#ifndef MAP_HPP
#define MAP_HPP


enum class MapSize {
    SMALL,
    MEDIUM,
    LARGE
};

struct MapDimensions {
    int width;
    int height;
};

// link size and dimensions
inline MapDimensions getDimensions(MapSize size) {
    if (size == MapSize::SMALL)  return {12, 10};
    if (size == MapSize::MEDIUM) return {24, 20};
    if (size == MapSize::LARGE) return {36, 30};
    return {36, 30}; // default LARGE
}

class Map {
private:
    int width;
    int height;
    MapSize size;

public:
    Map(MapSize s);
    ~Map() = default;

    int getWidth() const { return width; }
    int getHeight() const { return height; }
    MapSize getSize() const { return size; }

    bool isInside(int x, int y);
    bool spawnFood(int &x, int &y);
    bool removeFood(int x, int y);
};


#endif // MAP_HPP
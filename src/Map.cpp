#include "Map.hpp"
#include <cstdlib> // for std::rand() # TODO ekki nota modulus

bool Map::isInside(int x, int y) {
    // return true if the coordinates are within the grid bounds
    return (x >= 0 && x < width && y >= 0 && y < height);
}

bool Map::spawnFood(int &x, int &y) {
    // set x and y to random coordinates within the map bounds
    x = std::rand() % width;
    y = std::rand() % height;
    return true;
}

bool Map::removeFood(int x, int y) {
    (void)x;
    (void)y;    
    return true;
}
#include <iostream>
#include <string>
#include <vector>

const std::string HEADER = "#Life 1.06";

std::vector<Grid*> grids;

// TODO if grids are next to each other, fuse them

class Vec2d {
    public:
        long long x, y;
        Vec2d(long long x, long long y) : x(x), y(y){};
        Vec2d(std::string s) {
            const std::string delimiter {", "};
            const auto pos {s.find(delimiter)};
            x = stoll(s.substr(1, pos));
            y = stoll(s.substr(pos + 2));
        }
};

class Grid {
    static const int GRID_SIZE {64}; // arbitrary, but must be factor of 2**64
    bool grid[GRID_SIZE][GRID_SIZE] {false};
    public:
        // returns if cell was added to grid
        bool addCell(const Vec2d cell) {

        }
};

// could optimize checking for existing grid using BSP trees instead of big list

void addToGrid(Vec2d cell) {
    // if cell already in grid
    for (auto g : grids) {
        ;
    }
    // add to existing subgrid
    // else
    // create new subgrid and add to it
}

// TODO add checks for max boundaries

int main() {
    // read from stdin
    std::string line;
    while (getline(std::cin, line)) {
        Vec2d cell(line);
        addToGrid(cell);
    }

    // for each subgrid (remember to check perimeter for alive ones too)
    //  for each cell
    //   get neighbors
    //   if cell is alive in next state, write coords to string vector
    // feed coords back to self and repeat algorithm for 10 gens (can probably optimize this)
    // or write coords to stdout if done

    return 0;
}
#include <iostream>
#include <string>
#include <vector>

const std::string HEADER = "#Life 1.06";

// TODO add checks for max/min Int64 value
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
    long long x_start, y_start, x_end, y_end, size;
    std::vector<std::vector<bool>> grid;
    public:
        // returns if cell was added to grid
        bool addCell(const Vec2d cell) {
            if (inBounds(cell)) {
                grid[cell.y % size][cell.x % size] = true;
                return true;
            }
            return false;
        }

        bool inBounds(const Vec2d cell) const {
            if (x_start <= cell.x && cell.x < x_end) {
                if (y_start <= cell.y && cell.y < y_end) {
                    return true;
                }
            }
            return false;
        }

        Grid(long long x_start, long long y_start, long long size) : x_start(x_start), y_start(y_start), size(size), x_end(x_start + size), y_end(y_start + size) {
            grid = std::vector<std::vector<bool>>(size);
        }
};

std::vector<Grid*> grids;

// could optimize checking for existing grid using BSP trees instead of big list
bool gridExists(Vec2d cell) {
    for (auto g : grids) {
        if (g->addCell(cell)) {
           return true;
        }
    }
    return false;
}

void addToGrid(Vec2d cell) {
    // check if subgrid already exists
    if (!gridExists(cell)) {
        // TODO create new subgrid and add to it
        // could optimize here by fusing neighboring subgrids together
    }
}

int main() {
    // read from stdin
    std::string line;
    while (getline(std::cin, line) && !line.empty()) {
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
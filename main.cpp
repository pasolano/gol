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
        friend std::ostream& operator<< (std::ostream &out, const Vec2d &v);
};

std::ostream& operator<< (std::ostream &out, const Vec2d &v) {
    out << v.x << ' ' << v.y;
    return out;
}

class Grid {
    const static int GRID_SIZE = 64;
    long long x_start, y_start, x_end, y_end, size;
    std::vector<std::vector<bool>> grid;
    public:
        // returns if cell was added to grid
        bool addCell(const Vec2d cell) {
            if (inBounds(cell)) {
                grid[cell.x % size][cell.y % size] = true;
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

        bool isCellAlive(long long x, long long y) {
            return grid[x][y];
        }

        // https://stackoverflow.com/questions/18454793/checking-a-grid-throwing-out-of-bounds-exceptions
        char coalesceNeighbors(long long x, long long y) {
            char tot = 0;
            
            int min_row = std::max((long long) 0, x - 1);
            int max_row = std::min(size - 1, x + 1);
            int min_col = std::max((long long) 0, y - 1);
            int max_col = std::min(size - 1, y + 1);

            for (int row = min_row; row <= max_row; row++) {
                for (int col = min_col; col <= max_col; col++) {
                    if (row != x || col != y) {
                        if (grid[row][col]) {
                            tot++;
                            if (tot > 3) return tot; // always dead over 3
                        }
                    }
                }
            }

            return tot;
        }

        bool getCellUpdate(long long x, long long y) {
            char neighbors = coalesceNeighbors(x, y);
            if (neighbors == 3) return true;
            else if (inBounds(Vec2d(x, y)) && isCellAlive(x, y)) {
                if (neighbors >= 2 && neighbors <= 3)
                    return true;
            }
            return false;
        }

        long long getSize() {
            return size;
        }

        Grid(long long x_start, long long y_start, long long size = GRID_SIZE) : x_start(x_start), y_start(y_start), size(size), x_end(x_start + size), y_end(y_start + size) {
            grid = std::vector<std::vector<bool>>(size);
            for (int i = 0; i < size; i++)
                grid[i] = std::vector<bool>(size);
        }

        // TODO does this arithmetic work with negative values?
        Grid(const Vec2d cell, long long size = GRID_SIZE) : size(size) {
            grid = std::vector<std::vector<bool>>(size);
            for (int i = 0; i < size; i++)
                grid[i] = std::vector<bool>(size);

            x_start = cell.x - (cell.x % size);
            y_start = cell.y - (cell.y % size);
            x_end = x_start + size;
            y_end = y_start + size;

            addCell(cell);
        }
};

std::vector<Grid*> grids;

// could optimize checking for existing grid using BSP trees instead of big list
bool gridExists(Vec2d cell) {
    for (auto &g : grids) {
        if (g->addCell(cell)) {
           return true;
        }
    }
    return false;
}

void addToGrid(Vec2d cell) {
    // check if subgrid already exists
    if (!gridExists(cell)) {
        // create new grid and add cell to it
        grids.push_back(new Grid(cell));
        // could optimize here by fusing neighboring subgrids together
    }
}

int main() {
    const int GENERATIONS {10};

    std::vector<Vec2d> next_gen;

    // read from stdin
    // can optimize this
    std::string line;
    while (getline(std::cin, line) && !line.empty()) {
        next_gen.push_back(line);
    }

    for (int i = 0; i < GENERATIONS; i++) {
        for (auto pair : next_gen) {
            Vec2d cell(pair);
            addToGrid(cell);
        }

        next_gen.clear();

        // for each subgrid (remember to check perimeter for alive ones too)
        for (Grid* grid : grids) {
            const long long s {grid->getSize()};

            // NOTE: check perimeter outside of subgrid for born cells
            for (int x = -1; x <= s; x++) {
                for (int y = -1; y <= s; y++) {
                    if (grid->getCellUpdate(x, y)) {
                        next_gen.push_back(Vec2d(x, y));
                    }
                }
            }
        }

        for (auto &ptr : grids) {
            delete ptr;
        }
        grids.clear();
    }

    std::cout << HEADER << std::endl;
    for (Vec2d vec : next_gen) {
        std::cout << vec << std::endl;
    }

    // system("clear");

    // feed coords back to self and repeat algorithm for 10 gens (can probably optimize this)
    return 0;
}
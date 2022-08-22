#include <iostream>
#include <string>
#include <vector>

const std::string HEADER = "#Life 1.06";

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
    long long size;
    std::vector<std::vector<bool>> grid;
    public:
        long long x_start, y_start, x_end, y_end;

        // returns if cell was added to grid
        bool addCell(const Vec2d cell) {
            if (inGlobalBounds(cell)) {
                long long x = cell.x % size;
                long long y = cell.y % size;
                if (x < 0) x += size;
                if (y < 0) y += size;
                grid[x][y] = true;
                return true;
            }
            return false;
        }

        bool inGlobalBounds(const Vec2d cell) const {
            if (x_start <= cell.x && cell.x < x_end) {
                if (y_start <= cell.y && cell.y < y_end) {
                    return true;
                }
            }
            return false;
        }

        bool inLocalBounds(long long x, long long y) const {
            if (0 <= x && x < size) {
                if (0 <= y && y < size) {
                    return true;
                }
            }
            return false;
        }

        bool isCellAlive(long long x, long long y) {
            return grid[x][y];
        }

        char coalesceNeighbors(long long x, long long y) {
            char tot = 0;
            
            int min_row = std::max((long long) 0, x - 1);
            int max_row = std::min(size - 1, x + 1);
            int min_col = std::max((long long) 0, y - 1);
            int max_col = std::min(size - 1, y + 1);

            for (int row = min_row; row <= max_row; row++) {
                for (int col = min_col; col <= max_col; col++) {
                    if (row != x || col != y) {
                        if (isCellAlive(row, col)) {
                            tot++;
                            if (tot > 3) return tot; // always dead over 3
                        }
                    }
                }
            }

            return tot;
        }

        bool getCellUpdate(long long x, long long y) {
            int i = 0;
            char neighbors = coalesceNeighbors(x, y);
            if (neighbors == 3) return true;
            else if (inLocalBounds(x, y) && isCellAlive(x, y)) {
                if (neighbors >= 2 && neighbors <= 3)
                    return true;
            }
            return false;
        }

        long long getSize() {
            return size;
        }

        Grid(const Vec2d cell, long long size = GRID_SIZE) : size(size) {
            grid = std::vector<std::vector<bool>>(size);
            for (int i = 0; i < size; i++)
                grid[i] = std::vector<bool>(size);

            x_start = cell.x - (cell.x % size);
            y_start = cell.y - (cell.y % size);
            x_end = x_start + size;
            y_end = y_start + size;

            if (cell.x < 0) {
                x_start -= size;
                x_end -= size;
            }
            if (cell.y < 0) {
                y_start -= size;
                y_end -= size;
            }

            addCell(cell);
        }
};

std::vector<Grid*> grids;

// CONSIDER optimize checking for existing grid using BSP trees instead of big list
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

        // CONSIDER could optimize here by fusing neighboring subgrids together
    }
}

int main() {
    // CONSIDER adding command line option for number of generations
    const int GENERATIONS {10};

    std::vector<Vec2d> next_gen;

    // CONSIDER could create grids during this step instead of doing I/O text data

    // read from stdin
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
                        next_gen.push_back(Vec2d(grid->x_start + x, grid->y_start + y));
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

    // CONSIDER adding ASCII board option
    // system("clear");

    return 0;
}
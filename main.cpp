#include <iostream>
#include <string>
#include <vector>
#include <stack>

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
    const static int GRID_SIZE = 4;
    std::vector<std::vector<bool>> grid;
    public:
        long long x_start, y_start, x_end, y_end; // TODO change to Vec2d

        long long getXSize() const {
            return abs(x_end - x_start);
        }

        long long getYSize() const {
            return abs(y_end - y_start);
        }

        // returns if cell was added to grid

        bool addCell(const Vec2d cell) {
            if (inGlobalBounds(cell)) {
                long long x = cell.x - x_start;
                long long y = cell.y - y_start;
                if (x < 0) x += getXSize();
                if (y < 0) y += getYSize();

                grid[x][y] = true;
                return true;
            }
            return false;
        }

        // TODO make sure this doesnt convert some locals
        bool inGlobalBounds(const Vec2d cell) const {
            if (x_start <= cell.x && cell.x < x_end) {
                if (y_start <= cell.y && cell.y < y_end) {
                    return true;
                }
            }
            return false;
        }

        bool inLocalBounds(long long x, long long y) const {
            if (0 <= x && x < getXSize()) {
                if (0 <= y && y < getYSize()) {
                    return true;
                }
            }
            return false;
        }

        bool isCellAlive(long long x, long long y) const {
            return grid[x][y];
        }

        char coalesceNeighbors(long long x, long long y) {
            char tot = 0;
            
            long long min_row = std::max((long long) 0, x - 1);
            long long max_row = std::min(getXSize() - 1, x + 1);
            long long min_col = std::max((long long) 0, y - 1);
            long long max_col = std::min(getYSize() - 1, y + 1);

            for (long long row = min_row; row <= max_row; row++) {
                for (long long col = min_col; col <= max_col; col++) {
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

        void includeGrid(Grid& absorb) {
            long long old_x_start = x_start;
            long long old_y_start = y_start;

            // change range to include other grid
            x_start = std::min(x_start, absorb.x_start);
            y_start = std::min(y_start, absorb.y_start);
            x_end = std::max(x_end, absorb.x_end);
            y_end = std::max(y_end, absorb.y_end);

            // expand representation of grid
            auto new_grid = std::vector<std::vector<bool>>(getXSize());
            for (int i = 0; i < new_grid.size(); i++) {
                new_grid[i] = std::vector<bool>(getYSize());
            }

            // calculate local cell offsets
            long long x_offset = old_x_start - x_start;
            long long y_offset = old_y_start - y_start;
            
            // translate old cells to new local position
            for (long long x = 0; x < grid.size(); x++) {
                for (long long y = 0; y < grid[0].size(); y++) {
                    if (grid[x][y]) {
                        new_grid[x + x_offset][y + y_offset] = true;
                    }
                }
            }

            // calculate offsets for neighbor cell
            x_offset = absorb.x_start - x_start;
            y_offset = absorb.y_start - y_start;

            // translate absorbed cells to new local position
            for (long long x = 0; x < absorb.getXSize(); x++) {
                for (long long y = 0; y < absorb.getYSize(); y++) {
                    if (absorb.isCellAlive(x, y)) {
                        new_grid[x + x_offset][y + y_offset] = true;
                    }
                }
            }

            grid = new_grid;
        }

        Grid(const Vec2d cell, long long size = GRID_SIZE) {
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

        friend std::ostream& operator<< (std::ostream &out, const Grid &grid);
};

std::ostream& operator<< (std::ostream &out, const Grid &grid) {
    std::stack<std::string> res;
    std::string buf;
    for (int x = 0; x < grid.getXSize(); x++) {
        for (int y = 0; y < grid.getYSize(); y++) {
            if(grid.isCellAlive(x, y)) {
                buf += "X";
            }
            else {
                buf += ".";
            }
        }
        res.push(buf);
        buf = "";
    }
    while (!res.empty()) {
        out << res.top() << '\n';
        res.pop();
    }
    out << std::endl;

    return out;
}

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

// returns true if grids are touching (including being nested)
bool areNeighbors(const Grid& g1, const Grid& g2) {
    bool x_overlap = std::max(g1.x_start, g2.x_start) <= std::min(g1.x_end, g2.x_end);
    bool y_overlap = std::max(g1.y_start, g2.y_start) <= std::min(g1.y_end, g2.y_end);

    return x_overlap && y_overlap;
}

void addGrid(const Vec2d cell) {
    Grid grid(cell);
 
    int i;
    for (i = 0; i < grids.size(); i++) {
        if (areNeighbors(grid, *grids[i])) {
            // expand grid to absorb other, absorb cells
            grid.includeGrid(*grids[i]);

            // delete other grid and remove from list
            delete grids[i];
            grids.erase(grids.begin() + i);

            // repeat loop check from start
            i = 0;
        }
    }

    grids.push_back(new Grid(grid));
}

void addToGrid(Vec2d cell) {
    // check if subgrid already exists
    if (!gridExists(cell)) {
        addGrid(cell);
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
            std::cout << *grid << std::endl;
            // NOTE: checks perimeter outside of subgrid for born cells
            for (int x = -1; x <= grid->getXSize(); x++) {
                for (int y = -1; y <= grid->getYSize(); y++) {
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
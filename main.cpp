#include <iostream>
#include <string>
#include <vector>
#include <stack>
#include <chrono>

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
    std::vector<std::vector<bool>> grid;
    public:
        long long x_start, y_start, x_end, y_end;

        long long getXSize() const {
            return abs(x_end - x_start);
        }

        long long getYSize() const {
            return abs(y_end - y_start);
        }

        // returns if cell was added to grid
        bool addCell(const Vec2d cell) {
            if (withinGlobalBounds(cell)) {
                long long x = cell.x - x_start;
                long long y = cell.y - y_start;
                if (x < 0) x += getXSize();
                if (y < 0) y += getYSize();

                grid[x][y] = true;
                return true;
            }
            return false;
        }

        bool withinGlobalBounds(const Vec2d cell) const {
            if (x_start <= cell.x && cell.x < x_end) {
                if (y_start <= cell.y && cell.y < y_end) {
                    return true;
                }
            }
            return false;
        }

        bool withinLocalBounds(long long x, long long y) const {
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

        // returns how many neighbors of passed cell are alive
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

        // returns whether the passed cell will be alive or dead in the next frame
        // true is alive, false is dead
        bool getCellUpdate(long long x, long long y) {
            int i = 0;
            char neighbors = coalesceNeighbors(x, y);
            if (neighbors == 3) return true;
            else if (withinLocalBounds(x, y) && isCellAlive(x, y)) {
                if (neighbors >= 2 && neighbors <= 3)
                    return true;
            }
            return false;
        }

        // Grid this is called on is modified to add the properties of the passed Grid
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

        // create subgrid based on cell placement and grid size
        Grid(const Vec2d cell, long long size = GRID_SIZE) {
            // initialize grid container
            grid = std::vector<std::vector<bool>>(size);
            for (int i = 0; i < size; i++)
                grid[i] = std::vector<bool>(size);

            // calculate start of subgrid
            // should be last multiple of size before our cell
            x_start = cell.x - (cell.x % size);
            y_start = cell.y - (cell.y % size);

            // move range further back if the value is negative
            if (x_start != cell.x && cell.x < 0) {
                x_start -= size;
            }
            if (y_start != cell.y && cell.y < 0) {
                y_start -= size;
            }

            x_end = x_start + size;
            y_end = y_start + size;

            addCell(cell);
        }

        friend std::ostream& operator<< (std::ostream &out, const Grid &grid);
};


// pretty print for grid
std::ostream& operator<< (std::ostream &out, const Grid &grid) {
    // use stack so that the rows are printed bottom to top
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

// add appropriate grid for our cell to the grids list
// ensures that all possible neighbors are combined into current grid
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

// add cell to a grid
// checks if an appropriate grid exists
void addToGrid(Vec2d cell) {
    // check if subgrid already exists
    if (!gridExists(cell)) {
        addGrid(cell);
    }
}

int main() {
    // not const so demo can change this value
    int GENERATIONS {10};

    std::vector<Vec2d> next_gen;

    // CONSIDER could create grids during this step instead of saving text

    // read from stdin
    std::string line;
    while (getline(std::cin, line) && !line.empty()) {
        next_gen.push_back(line);
    }

    for (int i = 0; i < GENERATIONS; i++) {
        // generate current state from text input
        for (auto pair : next_gen) {
            Vec2d cell(pair);
            addToGrid(cell);
        }

        // INFO this is a visualizer with ASCII graphics -- sleeping the main thread is hacky, but it works
        // GENERATIONS = 100;
        // const auto before = std::chrono::system_clock::now();
        // while (std::chrono::system_clock::now() - before < std::chrono::milliseconds(100)) {
        //     ;
        // }
        // system("clear");
        // for (auto g : grids) {
        //     std::cout << *g << std::endl;
        // }

        next_gen.clear();

        // calculate next state and save cells to string vector
        // not the most efficient, but reuses code and makes code flow more readable
        for (Grid* grid : grids) {
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

    // once states are 
    std::cout << HEADER << std::endl;
    for (Vec2d vec : next_gen) {
        std::cout << vec << std::endl;
    }

    return 0;
}
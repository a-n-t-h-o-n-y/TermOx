#include "importers.hpp"

#include <fstream>
#include <string>
#include <vector>

#include "coordinate.hpp"

namespace gol {

std::vector<Coordinate> get_life_1_05(const std::string& filename) {
    return {};
}

std::vector<Coordinate> get_life_1_06(const std::string& filename) {
    std::ifstream file{filename};
    if (file.fail()) {
        return {};
    }
    std::vector<Coordinate> cells;
    std::string first_line;
    std::getline(file, first_line);
    while (file) {
        int x{0};
        file >> x;
        if (!file) {
            break;
        }
        int y{0};
        file >> y;
        if (!file.good()) {
            break;
        }
        cells.push_back({x, y});
    }
    return cells;
}

std::vector<Coordinate> get_plaintext(const std::string& filename) {
    return {};
}

std::vector<Coordinate> get_RLE(const std::string& filename) {
    return {};
}

}  // namespace gol

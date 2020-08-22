#include "get_life_1_06.hpp"
#include <fstream>
#include <string>
#include <vector>

#include "coordinate.hpp"

namespace gol {

std::vector<Coordinate> get_life_1_06(const std::string& filename)
{
    auto file = std::ifstream{filename};
    if (file.fail())
        return {};

    auto cells      = std::vector<Coordinate>{};
    auto first_line = std::string{};
    std::getline(file, first_line);
    while (file) {
        auto x = 0;
        file >> x;
        if (!file)
            break;
        auto y = 0;
        file >> y;
        if (!file.good())
            break;
        cells.push_back({x, y});
    }
    return cells;
}
}  // namespace gol

#include "get_life_1_06.hpp"

#include <fstream>
#include <string>

#include "coordinate.hpp"
#include "pattern.hpp"

namespace gol {

auto get_life_1_06(std::string const& filename) -> Pattern
{
    auto file = std::ifstream{filename};
    if (file.fail())
        return {};

    auto cells      = Pattern::Cells{};
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
    return {cells, parse_rule_string("B3/S23")};
}

}  // namespace gol

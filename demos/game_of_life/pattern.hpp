#ifndef CPPURSES_DEMOS_GAME_OF_LIFE_PATTERN_HPP
#define CPPURSES_DEMOS_GAME_OF_LIFE_PATTERN_HPP
#include <vector>

#include "coordinate.hpp"
#include "rule.hpp"

namespace gol {

struct Pattern {
    using Cells = std::vector<Coordinate>;

    Cells cells;
    Rule rule;
};

}  // namespace gol
#endif  // CPPURSES_DEMOS_GAME_OF_LIFE_PATTERN_HPP

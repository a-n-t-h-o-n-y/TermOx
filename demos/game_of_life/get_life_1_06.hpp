#ifndef TERMOX_DEMOS_GAME_OF_LIFE_GET_LIFE_1_06_HPP
#define TERMOX_DEMOS_GAME_OF_LIFE_GET_LIFE_1_06_HPP
#include <string>

#include "coordinate.hpp"
#include "pattern.hpp"

namespace gol {

/// Return cell Coordinates from Life 1.06 file.
auto get_life_1_06(std::string const& filename) -> Pattern;

}  // namespace gol
#endif  // TERMOX_DEMOS_GAME_OF_LIFE_GET_LIFE_1_06_HPP

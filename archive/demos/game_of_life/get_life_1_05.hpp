#ifndef TERMOX_DEMOS_GAME_OF_LIFE_GET_LIFE_1_05_HPP
#define TERMOX_DEMOS_GAME_OF_LIFE_GET_LIFE_1_05_HPP
#include <string>

#include "coordinate.hpp"
#include "pattern.hpp"

namespace gol {

/// Return cell Coordinates from Life 1.05 file.
auto get_life_1_05(std::string const& filename) -> Pattern;

}  // namespace gol
#endif  // TERMOX_DEMOS_GAME_OF_LIFE_GET_LIFE_1_05_HPP

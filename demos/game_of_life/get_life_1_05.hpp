#ifndef CPPURSES_DEMOS_GAME_OF_LIFE_GET_LIFE_1_05_HPP
#define CPPURSES_DEMOS_GAME_OF_LIFE_GET_LIFE_1_05_HPP
#include <string>
#include <vector>

#include "coordinate.hpp"

namespace gol {

/// Return cell Coordinates from Life 1.05 file.
std::vector<Coordinate> get_life_1_05(const std::string& filename);

}  // namespace gol
#endif  // CPPURSES_DEMOS_GAME_OF_LIFE_GET_LIFE_1_05_HPP

#ifndef CPPURSES_DEMOS_GAME_OF_LIFE_GET_PLAINTEXT_HPP
#define CPPURSES_DEMOS_GAME_OF_LIFE_GET_PLAINTEXT_HPP
#include <string>
#include <vector>

#include "coordinate.hpp"

namespace gol {

/// Retrieve cell Coordinates from plaintext file.
std::vector<Coordinate> get_plaintext(const std::string& filename);

}  // namespace gol
#endif  // CPPURSES_DEMOS_GAME_OF_LIFE_GET_PLAINTEXT_HPP

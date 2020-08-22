#ifndef CPPURSES_DEMOS_GAME_OF_LIFE_GET_RLE_HPP
#define CPPURSES_DEMOS_GAME_OF_LIFE_GET_RLE_HPP
#include <string>
#include <utility>
#include <vector>

#include "coordinate.hpp"

namespace gol {

/// Return cell Coordinates and rule string from RLE file.
auto get_RLE(const std::string& filename)
    -> std::pair<std::vector<Coordinate>, std::string>;

}  // namespace gol
#endif  // CPPURSES_DEMOS_GAME_OF_LIFE_GET_RLE_HPP

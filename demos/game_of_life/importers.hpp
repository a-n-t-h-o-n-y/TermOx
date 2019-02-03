#ifndef CPPURSES_DEMOS_GAME_OF_LIFE_IMPORTERS_HPP
#define CPPURSES_DEMOS_GAME_OF_LIFE_IMPORTERS_HPP
#include <string>
#include <vector>

#include "coordinate.hpp"

namespace gol {

/// Retrieve cell points from Life 1.05 file.
std::vector<Coordinate> get_life_1_05(const std::string& filename);

/// Retrieve cell points from Life 1.06 file.
std::vector<Coordinate> get_life_1_06(const std::string& filename);

/// Retrieve cell points from plaintext file.
std::vector<Coordinate> get_plaintext(const std::string& filename);

/// Retrieve cell points from RLE file.
std::vector<Coordinate> get_RLE(const std::string& filename);

}  // namespace gol
#endif  // CPPURSES_DEMOS_GAME_OF_LIFE_IMPORTERS_HPP

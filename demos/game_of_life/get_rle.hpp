#ifndef CPPURSES_DEMOS_GAME_OF_LIFE_GET_RLE_HPP
#define CPPURSES_DEMOS_GAME_OF_LIFE_GET_RLE_HPP
#include <istream>
#include <string>

#include "pattern.hpp"

namespace gol {

/// Opens the file by name and parses the contents, returning a Pattern.
auto get_RLE(std::string const& filename) -> Pattern;

/// Parsing function will work with any std::istream type.
auto parse_rle(std::istream& is) -> Pattern;

}  // namespace gol
#endif  // CPPURSES_DEMOS_GAME_OF_LIFE_GET_RLE_HPP

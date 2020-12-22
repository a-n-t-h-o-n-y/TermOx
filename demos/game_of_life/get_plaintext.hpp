#ifndef TERMOX_DEMOS_GAME_OF_LIFE_GET_PLAINTEXT_HPP
#define TERMOX_DEMOS_GAME_OF_LIFE_GET_PLAINTEXT_HPP
#include <string>

#include "coordinate.hpp"
#include "pattern.hpp"

namespace gol {

/// Return cell Coordinates from plaintext file.
auto get_plaintext(std::string const& filename) -> Pattern;

}  // namespace gol
#endif  // TERMOX_DEMOS_GAME_OF_LIFE_GET_PLAINTEXT_HPP

#ifndef CPPURSES_DEMOS_GAME_OF_LIFE_IMPORTERS_HPP
#define CPPURSES_DEMOS_GAME_OF_LIFE_IMPORTERS_HPP
#include <string>

#include "game_of_life_engine.hpp"

namespace gol {

/// Import Life 1.05 file into \p engine.
void import_as_life_1_05(const std::string& filename,
                         Game_of_life_engine& engine);

/// Import Life 1.06 file into \p engine.
void import_as_life_1_06(const std::string& filename,
                         Game_of_life_engine& engine);

/// Import plaintext file into \p engine.
void import_as_plaintext(const std::string& filename,
                         Game_of_life_engine& engine);

/// Import RLE file into \p engine.
void import_as_rle(const std::string& filename, Game_of_life_engine& engine);

}  // namespace gol
#endif  // CPPURSES_DEMOS_GAME_OF_LIFE_IMPORTERS_HPP

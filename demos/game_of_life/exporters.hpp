#ifndef TERMOX_DEMOS_GAME_OF_LIFE_EXPORTERS_HPP
#define TERMOX_DEMOS_GAME_OF_LIFE_EXPORTERS_HPP
#include <string>

#include "game_of_life_engine.hpp"

namespace gol {

/// Export \p engine state as Life 1.05 file.
void export_as_life_1_05(std::string const& filename,
                         Game_of_life_engine const& engine);

/// Export \p engine state as Life 1.06 file.
void export_as_life_1_06(std::string const& filename,
                         Game_of_life_engine const& engine);

/// Export \p engine state as plaintext file.
void export_as_plaintext(std::string const& filename,
                         Game_of_life_engine const& engine);

/// Export \p engine state as RLE file.
void export_as_rle(std::string const& filename,
                   Game_of_life_engine const& engine);

}  // namespace gol
#endif  // TERMOX_DEMOS_GAME_OF_LIFE_EXPORTERS_HPP

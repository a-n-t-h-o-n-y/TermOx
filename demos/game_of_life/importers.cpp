#include "importers.hpp"

#include <fstream>
#include <string>

#include "game_of_life_engine.hpp"

namespace gol {

void import_as_life_1_05(const std::string& filename,
                         Game_of_life_engine& engine) {}

void import_as_life_1_06(const std::string& filename,
                         Game_of_life_engine& engine) {
    std::ifstream file{filename};
    if (file.fail()) {
        return;
    }
    std::string first_line;
    std::getline(file, first_line);
    while (file) {
        int x{0};
        file >> x;
        if (!file) {
            break;
        }
        int y{0};
        file >> y;
        if (!file.good()) {
            break;
        }
        engine.give_life({x, y});
    }
}

void import_as_plaintext(const std::string& filename,
                         Game_of_life_engine& engine) {}

void import_as_rle(const std::string& filename, Game_of_life_engine& engine) {}

}  // namespace gol

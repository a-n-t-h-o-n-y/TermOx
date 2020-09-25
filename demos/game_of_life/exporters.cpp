#include "exporters.hpp"

#include <fstream>
#include <string>

#include "game_of_life_engine.hpp"

namespace gol {

void export_as_life_1_05(const std::string& /* filename */,
                         const Game_of_life_engine& /* engine */)
{
    // std::ofstream file{filename};
    // file << "#Life 1.05\n";
    // file << "#R 23/3\n";
    // auto first = true;
    // for (auto x = std::size_t{0}; x < engine.width(); ++x) {
    //     for (auto y = std::size_t{0}; y < engine.height(); ++y) {
    //         if (engine.is_alive_at({x, y})) {
    //             if (first) {
    //                 file << "#P " << x - static_cast<int>(engine.width()) / 2
    //                      << ' ' << y - static_cast<int>(engine.height()) / 2
    //                      << '\n';
    //                 first = false;
    //                 // file << std::string(x, '.');
    //             }
    //             file << '*';
    //         } else {
    //             if (!first) {
    //                 file << '.';
    //             }
    //         }
    //     }
    //     file << '\n';
    // }
}

void export_as_life_1_06(const std::string& filename,
                         const Game_of_life_engine& engine)
{
    auto file = std::ofstream{filename};
    file << "#Life 1.06\n";
    for (auto const cell : engine) {
        if (cell.is_alive)
            file << cell.coordinate.x << ' ' << cell.coordinate.y << '\n';
    }
}

void export_as_plaintext(const std::string& /* filename */,
                         const Game_of_life_engine& /* engine */)
{}

void export_as_rle(const std::string& /* filename */,
                   const Game_of_life_engine& /* engine */)
{}
}  // namespace gol

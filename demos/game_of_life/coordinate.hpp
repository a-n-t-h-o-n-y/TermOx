#ifndef CPPURSES_DEMOS_GAME_OF_LIFE_COORDINATE_HPP
#define CPPURSES_DEMOS_GAME_OF_LIFE_COORDINATE_HPP
#include <cstdint>

namespace gol {

/// Holds a coordinate pair(x, y) for placement of cells in a GoL pattern.
class Coordinate {
   public:
    std::int16_t x;
    std::int16_t y;

   public:
    Coordinate(int x_, int y_)
        : x{static_cast<std::int16_t>(x_)}, y{static_cast<std::int16_t>(y_)}
    {}

    Coordinate(std::int16_t x_, std::int16_t y_) : x{x_}, y{y_} {}

    Coordinate(int x_, std::int16_t y_)
        : x{static_cast<std::int16_t>(x_)}, y{y_}
    {}

    Coordinate(std::int16_t x_, int y_)
        : x{x_}, y{static_cast<std::int16_t>(y_)}
    {}
};

/// Increases from left to right, top to bottom.
inline auto operator<(Coordinate lhs, Coordinate rhs) -> bool
{
    return (lhs.y < rhs.y) || (lhs.y == rhs.y && lhs.x < rhs.x);
}

}  // namespace gol
#endif  // CPPURSES_DEMOS_GAME_OF_LIFE_COORDINATE_HPP

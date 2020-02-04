#ifndef CPPURSES_DEMOS_GAME_OF_LIFE_COORDINATE_HPP
#define CPPURSES_DEMOS_GAME_OF_LIFE_COORDINATE_HPP
namespace gol {
/// Holds a coordinate pair(x, y) for placement of cells in a GoL pattern.
struct Coordinate {
    int x;
    int y;
};

/// Increases from left to right, top to bottom.
inline bool operator<(Coordinate lhs, Coordinate rhs)
{
    return (lhs.y < rhs.y) or (lhs.y == rhs.y and lhs.x < rhs.x);
}

}  // namespace gol
#endif  // CPPURSES_DEMOS_GAME_OF_LIFE_COORDINATE_HPP

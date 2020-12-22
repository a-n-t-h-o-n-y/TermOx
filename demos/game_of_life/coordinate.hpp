#ifndef TERMOX_DEMOS_GAME_OF_LIFE_COORDINATE_HPP
#define TERMOX_DEMOS_GAME_OF_LIFE_COORDINATE_HPP
#include <cstdint>
#include <functional>
#include <limits>

namespace gol {

/// Holds a coordinate pair(x, y) for placement of cells in a GoL pattern.
class Coordinate {
   public:
    using Value_t = std::int16_t;

   public:
    static constexpr auto Invalid = std::numeric_limits<Value_t>::max();

   public:
    Value_t x;
    Value_t y;

   public:
    Coordinate(int x_, int y_)
        : x{static_cast<Value_t>(x_)}, y{static_cast<Value_t>(y_)}
    {}

    Coordinate(Value_t x_, Value_t y_) : x{x_}, y{y_} {}

    Coordinate(int x_, Value_t y_) : x{static_cast<Value_t>(x_)}, y{y_} {}

    Coordinate(Value_t x_, int y_) : x{x_}, y{static_cast<Value_t>(y_)} {}
};

/// Increases from left to right, top to bottom.
inline auto operator<(Coordinate lhs, Coordinate rhs) -> bool
{
    return (lhs.y < rhs.y) || (lhs.y == rhs.y && lhs.x < rhs.x);
}

inline auto operator==(Coordinate lhs, Coordinate rhs) -> bool
{
    return (lhs.x == rhs.x) && (lhs.y == rhs.y);
}

}  // namespace gol

/// Custom specialization of std::hash for ox::point.
namespace std {
template <>
struct hash<gol::Coordinate> {
    using argument_type = gol::Coordinate;
    using result_type   = std::size_t;
    auto operator()(argument_type const& c) const noexcept -> result_type
    {
        auto const h1 = std::hash<decltype(c.x)>{}(c.x);
        auto const h2 = std::hash<decltype(c.y)>{}(c.y);
        return h1 ^ (h2 << 1);
    }
};
}  // namespace std
#endif  // TERMOX_DEMOS_GAME_OF_LIFE_COORDINATE_HPP

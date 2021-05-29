#include <termox/widget/bordered.hpp>

#include <optional>

namespace ox {

auto drop_north(Border b) -> Border
{
    b.north = std::nullopt;
    return b;
}

auto drop_south(Border b) -> Border
{
    b.south = std::nullopt;
    return b;
}

auto drop_east(Border b) -> Border
{
    b.east = std::nullopt;
    return b;
}

auto drop_west(Border b) -> Border
{
    b.west = std::nullopt;
    return b;
}

auto take_north(Border b) -> Border
{
    b.south = std::nullopt;
    b.east  = std::nullopt;
    b.west  = std::nullopt;
    return b;
}

auto take_south(Border b) -> Border
{
    b.north = std::nullopt;
    b.east  = std::nullopt;
    b.west  = std::nullopt;
    return b;
}

auto take_east(Border b) -> Border
{
    b.north = std::nullopt;
    b.south = std::nullopt;
    b.west  = std::nullopt;
    return b;
}

auto take_west(Border b) -> Border
{
    b.north = std::nullopt;
    b.south = std::nullopt;
    b.east  = std::nullopt;
    return b;
}
}  // namespace ox

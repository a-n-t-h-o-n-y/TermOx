#ifndef TERMOX_WIDGET_BOUNDARY_HPP
#define TERMOX_WIDGET_BOUNDARY_HPP
#include <cassert>
#include <cmath>

namespace ox {

/// Four point Boundary, edges are inclusive.
template <typename Number_t>
struct Boundary {
    Number_t west  = 0;
    Number_t east  = 1;
    Number_t north = 1;
    Number_t south = 0;
};

/// Return the input Boundary \p b, with each corner scaled by \p amount.
/** Only accepts finite values greater than zero for \p amount. */
template <typename Number_t>
[[nodiscard]] auto scale(Boundary<Number_t> b, double amount)
    -> Boundary<Number_t>
{
    assert(amount > 0. && std::isfinite(amount));
    return {Number_t(b.west * amount), Number_t(b.east * amount),
            Number_t(b.north * amount), Number_t(b.south * amount)};
}

/// Scrolls \p b horizontally left by \p amount.
/** \p amount has to be positive. */
template <typename Number_t>
[[nodiscard]] auto scroll_west(Boundary<Number_t> b, Number_t amount)
    -> Boundary<Number_t>
{
    assert(amount >= 0.);
    b.west -= amount;
    b.east -= amount;
    return b;
}

/// Scrolls \p b horizontally right by \p amount.
/** \p amount has to be positive. */
template <typename Number_t>
[[nodiscard]] auto scroll_east(Boundary<Number_t> b, Number_t amount)
    -> Boundary<Number_t>
{
    assert(amount >= 0.);
    b.west += amount;
    b.east += amount;
    return b;
}

/// Scrolls \p b vertically upwards by \p amount.
/** \p amount has to be positive. */
template <typename Number_t>
[[nodiscard]] auto scroll_north(Boundary<Number_t> b, Number_t amount)
    -> Boundary<Number_t>
{
    assert(amount >= 0.);
    b.north += amount;
    b.south += amount;
    return b;
}

/// Scrolls \p b vertically downwards by \p amount.
/** \p amount has to be positive. */
template <typename Number_t>
[[nodiscard]] auto scroll_south(Boundary<Number_t> b, Number_t amount)
    -> Boundary<Number_t>
{
    assert(amount >= 0.);
    b.north -= amount;
    b.south -= amount;
    return b;
}

/// Subtract cooresponding Boundary members.
template <typename Number_t>
[[nodiscard]] auto operator-(Boundary<Number_t> a, Boundary<Number_t> b)
    -> Boundary<Number_t>
{
    return {a.west - b.west, a.east - b.east, a.north - b.north,
            a.south - b.south};
}

/// Add cooresponding Boundary members.
template <typename Number_t>
[[nodiscard]] auto operator+(Boundary<Number_t> a, Boundary<Number_t> b)
    -> Boundary<Number_t>
{
    return {a.west + b.west, a.east + b.east, a.north + b.north,
            a.south + b.south};
}

}  // namespace ox
#endif  // TERMOX_WIDGET_BOUNDARY_HPP

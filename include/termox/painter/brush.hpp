#ifndef TERMOX_PAINTER_BRUSH_HPP
#define TERMOX_PAINTER_BRUSH_HPP
#include <tuple>

#include <termox/painter/color.hpp>
#include <termox/painter/trait.hpp>

namespace ox {

/**
 * Defines the appearance of any paintable object with Traits and Colors.
 *
 * sizeof(Brush) == 4 Bytes && alignof(Brush) == 2 Bytes.
 */
struct Brush {
    Color background = Color::Background;
    Color foreground = Color::Foreground;
    Traits traits    = Trait::None;
};

/** True if the held traits and (back/fore)ground colors are equal. */
[[nodiscard]] auto constexpr operator==(Brush a, Brush b) -> bool
{
    return std::tie(a.traits, a.background, a.foreground) ==
           std::tie(b.traits, b.background, b.foreground);
}

/** False if the held traits and (back/fore)ground colors are equal. */
[[nodiscard]] auto constexpr operator!=(Brush a, Brush b) -> bool
{
    return !(a == b);
}

/**
 * Merge `ts` into `b`, returning a new `Brush` with the result.
 *
 * @param b The `Brush` to merge into.
 * @param ts The `Traits` to merge from.
 * @return `b` with any traits from `ts` merged in.
 */
[[nodiscard]] auto constexpr operator|(Brush b, Traits ts) -> Brush
{
    b.traits |= ts;
    return b;
}

/**
 * Merge `c` into `b`, returning a new `Brush` with the result.
 *
 * @param b The `Brush` to merge into.
 * @param c The `Background_color` to merge from.
 * @return `b` with any colors from `c` merged in.
 */
[[nodiscard]] auto constexpr operator|(Brush b, Background_color c) -> Brush
{
    b.background = Color{c.value};
    return b;
}

/**
 * Merge `c` into `b`, returning a new `Brush` with the result.
 *
 * @param b The `Brush` to merge into.
 * @param c The `Foreground_color` to merge from.
 * @return `b` with any colors from `c` merged in.
 */
[[nodiscard]] auto constexpr operator|(Brush b, Foreground_color c) -> Brush
{
    b.foreground = Color{c.value};
    return b;
}

/**
 * Merges `secondary` into `primary`, returning a new `Brush` with the result.
 *
 * `primary`'s colors are overwritten by `secondary`'s if `primary`'s color
 * is `Color::Background` or `Color::Foreground`.
 *
 * @param primary The `Brush` to merge into.
 * @param secondary The `Brush` to merge from.
 * @return `primary` with any colors from `secondary` merged in.
 */
[[nodiscard]] auto constexpr merge(Brush primary, Brush secondary) -> Brush
{
    if (primary.background == Color::Background)
        primary.background = secondary.background;

    if (primary.foreground == Color::Foreground)
        primary.foreground = secondary.foreground;

    primary.traits |= secondary.traits;

    return primary;
}

}  // namespace ox
#endif  // TERMOX_PAINTER_BRUSH_HPP

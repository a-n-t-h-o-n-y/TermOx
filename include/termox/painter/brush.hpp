#ifndef TERMOX_PAINTER_BRUSH_HPP
#define TERMOX_PAINTER_BRUSH_HPP
#include <tuple>
#include <utility>

#include <termox/painter/color.hpp>
#include <termox/painter/trait.hpp>

namespace ox {

/// Holds the look of any paintable object with Traits and Colors.
/** sizeof(Brush) == 3 Bytes && alignof(Brush) == 1 Byte. */
class Brush {
   public:
    Color background = Color::Background;
    Color foreground = Color::Foreground;

   private:
    // TODO wrap this in an Traits type, container of traits. then make it
    // public. Brush constructor and class just uses these objects for
    // convinience.
    using Byte_t   = unsigned char;
    Byte_t traits_ = 0;

   public:
    /// Construct a Brush with given Traits and Colors.
    template <typename... Traits>
    constexpr explicit Brush(Traits... traits)
    {
        this->add_traits(std::forward<Traits>(traits)...);
    }

   public:
    /// Add a variable number of Traits or Colors to the brush.
    /** Use the (back/fore)ground_color(Color c) functions to add colors to the
     *  list. */
    template <typename Head, typename... Tail>
    constexpr void add_traits(Head t, Tail... ts)
    {
        this->set_trait(t);
        this->add_traits(ts...);
    }
    constexpr void add_traits() {}

    /// Remove a variable number of Traits from the brush.
    /** Cannot remove foreground or background colors with this. */
    template <typename Head, typename... Tail>
    constexpr void remove_traits(Head t, Tail... ts)
    {
        this->unset_trait(t);
        this->remove_traits(ts...);
    }
    constexpr void remove_traits() {}

    /// Remove all of the set Traits from the brush, not including colors.
    constexpr void clear_traits() { traits_ = 0; }

    /// Provide a check of whether the brush has the provided Trait \p trait.
    constexpr auto has_trait(Trait trait) const -> bool
    {
        auto const mask = 1 << static_cast<Byte_t>(trait);
        return (traits_ & mask) != 0;
    }

    /// Compares if the held traits and (back/fore)ground colors are equal.
    constexpr auto operator==(Brush x) const -> bool
    {
        return std::tie(traits_, background, foreground) ==
               std::tie(x.traits_, x.background, x.foreground);
    }

   private:
    /// Used by add_traits() to set a deail::Background_color.
    constexpr void set_trait(Background_color bg)
    {
        background = Color{bg.value};
    }

    /// Used by add_traits() to set a deail::Foreground_color.
    constexpr void set_trait(Foreground_color fg)
    {
        foreground = Color{fg.value};
    }

    /// Used by add_traits() to set an Trait.
    constexpr void set_trait(Trait t)
    {
        traits_ |= 1 << static_cast<Byte_t>(t);
    }

    /// Remove a specific Trait, if it is set, otherwise no-op.
    constexpr void unset_trait(Trait t)
    {
        traits_ &= ~(1 << static_cast<Byte_t>(t));
    }

   private:
    // TODO remove as friend once Traits are public and their own type.
    friend constexpr auto merge(Brush, Brush) -> Brush;
};

/// Combines the two Brushes, using the primary Brush's Colors, if any, first.
constexpr inline auto merge(Brush primary, Brush secondary) -> Brush
{
    if (primary.background == Color::Background)
        primary.background = secondary.background;
    if (primary.foreground == Color::Foreground)
        primary.foreground = secondary.foreground;
    primary.traits_ |= secondary.traits_;
    return primary;
}

}  // namespace ox
#endif  // TERMOX_PAINTER_BRUSH_HPP

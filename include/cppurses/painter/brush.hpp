#ifndef CPPURSES_PAINTER_BRUSH_HPP
#define CPPURSES_PAINTER_BRUSH_HPP
#include <optional>
#include <tuple>
#include <utility>

#include <cppurses/painter/color.hpp>
#include <cppurses/painter/trait.hpp>

namespace cppurses {

/// Holds the look of any paintable object with Traits and Colors.
class Brush {
   public:
    /// Construct a Brush with given Traits and Colors.
    template <typename... Traits>
    constexpr explicit Brush(Traits... traits)
    {
        this->add_traits(std::forward<Traits>(traits)...);
    }

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

    /// Set the background color of this brush.
    constexpr void set_background(Color color)
    {
        background_color_ = std::optional{color};  // Makes it constexpr in GCC
    }

    /// Set the foreground color of this brush.
    constexpr void set_foreground(Color color)
    {
        foreground_color_ = std::optional{color};  // Makes it constexpr in GCC
    }

    /// Set the background to not have a color, the default state.
    void remove_background() { background_color_ = std::nullopt; }

    /// Set the foreground to not have a color, the default state.
    void remove_foreground() { foreground_color_ = std::nullopt; }

    /// Remove all of the set Traits from the brush, not including colors.
    constexpr void clear_traits() { traits_ = 0; }

    /// Provide a check of whether the brush has the provided Trait \p trait.
    constexpr auto has_trait(Trait trait) const -> bool
    {
        auto const mask = 1 << static_cast<Byte_t>(trait);
        return (traits_ & mask) != 0;
    }

    /// Return the current background as a std::optional object.
    constexpr auto background_color() const -> std::optional<Color> const&
    {
        return background_color_;
    }

    /// Return the current foreground as an std::optional object.
    constexpr auto foreground_color() const -> std::optional<Color> const&
    {
        return foreground_color_;
    }

    /// Compares if the held traits and (back/fore)ground colors are equal.
    constexpr auto operator==(Brush const& x) const -> bool
    {
        return std::tie(traits_, background_color_, foreground_color_) ==
               std::tie(x.traits_, x.background_color_, x.foreground_color_);
    }

   private:
    /// Used by add_traits() to set a deail::Background_color.
    constexpr void set_trait(Background_color bg)
    {
        this->set_background(Color{bg.value});
    }

    /// Used by add_traits() to set a deail::Foreground_color.
    constexpr void set_trait(Foreground_color fg)
    {
        this->set_foreground(Color{fg.value});
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
    using Byte_t   = unsigned char;
    Byte_t traits_ = 0;
    std::optional<Color> background_color_;
    std::optional<Color> foreground_color_;
    friend constexpr void imprint(Brush const&, Brush&);
};

/// Add Traits and Colors from \p from to \p to.
/** Does not overwrite existing colors in \p to. */
constexpr inline void imprint(Brush const& from, Brush& to)
{
    if (!to.background_color_ and from.background_color_)
        to.background_color_ = *from.background_color_;
    if (!to.foreground_color_ and from.foreground_color_)
        to.foreground_color_ = *from.foreground_color_;
    to.traits_ |= from.traits_;
}

}  // namespace cppurses
#endif  // CPPURSES_PAINTER_BRUSH_HPP

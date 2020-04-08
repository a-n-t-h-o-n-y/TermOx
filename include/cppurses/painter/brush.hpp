#ifndef CPPURSES_PAINTER_BRUSH_HPP
#define CPPURSES_PAINTER_BRUSH_HPP
#include <optional>
#include <tuple>
#include <utility>

#include <cppurses/painter/attribute.hpp>
#include <cppurses/painter/color.hpp>

namespace cppurses {

/// Holds the look of any paintable object with Attributes and Colors.
class Brush {
   public:
    /// Construct a Brush with given Attributes and Colors.
    template <typename... Attributes>
    constexpr explicit Brush(Attributes... attrs)
    {
        this->add_attributes(std::forward<Attributes>(attrs)...);
    }

    /// Add a variable number of Attributes or Colors to the brush.
    /** Use the (back/fore)ground_color(Color c) functions to add colors to the
     *  list. */
    template <typename Attr_t, typename... Tail>
    constexpr void add_attributes(Attr_t attribute, Tail... tail)
    {
        this->set_attr(attribute);
        this->add_attributes(tail...);
    }
    constexpr void add_attributes() {}

    /// Remove a variable number of Attributes from the brush.
    /** Cannot remove foreground or background colors with this. */
    template <typename... Attr_t>
    constexpr void remove_attributes(Attr_t... attributes)
    {
        for (Attribute a : {attributes...}) {
            this->unset_attr(a);
        }
    }
    constexpr void remove_attributes() {}

    /// Set the background color of this brush.
    constexpr void set_background(Color color) { background_color_ = color; }

    /// Set the foreground color of this brush.
    constexpr void set_foreground(Color color) { foreground_color_ = color; }

    /// Set the background to not have a color, the default state.
    constexpr void remove_background() { background_color_ = std::nullopt; }

    /// Set the foreground to not have a color, the default state.
    constexpr void remove_foreground() { foreground_color_ = std::nullopt; }

    /// Remove all of the set Attributes from the brush, not including colors.
    constexpr void clear_attributes() { attributes_ = 0; }

    /// Provide a check of whether the brush has the provided Attribute \p attr.
    constexpr auto has_attribute(Attribute attr) const -> bool
    {
        auto const mask = 1 << static_cast<Byte_t>(attr);
        return (attributes_ & mask) != 0;
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

    /// Compares if the held attributes and (back/fore)ground colors are equal.
    constexpr auto operator==(Brush const& x) const -> bool
    {
        return std::tie(attributes_, background_color_, foreground_color_) ==
               std::tie(x.attributes_, x.background_color_,
                        x.foreground_color_);
    }

   private:
    /// Used by add_attributes() to set a deail::BackgroundColor.
    constexpr void set_attr(BackgroundColor bc)
    {
        this->set_background(static_cast<Color>(bc));
    }

    /// Used by add_attributes() to set a deail::ForegroundColor.
    constexpr void set_attr(ForegroundColor fc)
    {
        this->set_foreground(static_cast<Color>(fc));
    }

    /// Used by add_attributes() to set an Attribute.
    constexpr void set_attr(Attribute attr)
    {
        attributes_ |= 1 << static_cast<Byte_t>(attr);
    }

    /// Remove a specific Attribute, if it is set, otherwise no-op.
    constexpr void unset_attr(Attribute attr)
    {
        attributes_ &= ~(1 << static_cast<Byte_t>(attr));
    }

   private:
    using Byte_t       = unsigned char;
    Byte_t attributes_ = 0;
    std::optional<Color> background_color_;
    std::optional<Color> foreground_color_;
    friend constexpr void imprint(Brush const&, Brush&);
};

/// Add Attributes and Colors from \p from to \p to.
/** Does not overwrite existing colors in \p to. */
constexpr inline void imprint(Brush const& from, Brush& to)
{
    if (!to.background_color_ and from.background_color_)
        to.background_color_ = *from.background_color_;
    if (!to.foreground_color_ and from.foreground_color_)
        to.foreground_color_ = *from.foreground_color_;
    to.attributes_ |= from.attributes_;
}

}  // namespace cppurses
#endif  // CPPURSES_PAINTER_BRUSH_HPP

#ifndef CPPURSES_PAINTER_BRUSH_HPP
#define CPPURSES_PAINTER_BRUSH_HPP
#include <bitset>
#include <utility>

#include <optional/optional.hpp>

#include <cppurses/painter/attribute.hpp>
#include <cppurses/painter/color.hpp>

namespace cppurses {

/// Holds the look of any paintable object with Attributes and Colors.
class Brush {
   public:
    /// Construct a Brush with given Attributes and Colors.
    template <typename... Attributes>
    explicit Brush(Attributes... attrs);

    // Base Case
    void add_attributes() {}

    /// Add a variable number of Attributes or Colors to the brush.
    /** Use the (back/fore)ground_color(Color c) functions to add colors to the
     *  list. */
    template <typename T, typename... Others>
    void add_attributes(T attr, Others... others);

    /// Set the background color of this brush.
    void set_background(Color color) { background_color_ = color; }

    /// Set the foreground color of this brush.
    void set_foreground(Color color) { foreground_color_ = color; }

    /// Remove a specific Attribute, if it is set, otherwise no-op.
    void remove_attribute(Attribute attr);

    /// Sets the background to not have a color, the default state.
    void remove_background();

    /// Sets the foreground to not have a color, the default state.
    void remove_foreground();

    /// Removes all of the set Attributes from the brush, not including colors.
    void clear_attributes();

    /// Provides a check if the brush has the provided Attribute \p attr.
    bool has_attribute(Attribute attr) const;

    /// Returns the current background as an opt::Optional object.
    opt::Optional<Color> background_color() const;

    /// Returns the current foreground as an opt::Optional object.
    opt::Optional<Color> foreground_color() const;

    friend bool operator==(const Brush& lhs, const Brush& rhs);

   private:
    /// Used by add_attributes() to set a deail::BackgroundColor.
    void set_attr(detail::BackgroundColor bc);

    /// Used by add_attributes() to set a deail::ForegroundColor.
    void set_attr(detail::ForegroundColor fc);

    /// Used by add_attributes() to set an Attribute.
    void set_attr(Attribute attr);

    // Data Members
    std::bitset<8> attributes_;
    opt::Optional<Color> background_color_;
    opt::Optional<Color> foreground_color_;
};

/// Compares if the held attributes and back/foreground colors are equal.
bool operator==(const Brush& lhs, const Brush& rhs);

// TEMPLATE IMPLEMENTATIONS
template <typename... Attributes>
Brush::Brush(Attributes... attrs) {
    this->add_attributes(std::forward<Attributes>(attrs)...);
}

template <typename T, typename... Others>
void Brush::add_attributes(T attr, Others... others) {
    this->set_attr(attr);
    this->add_attributes(others...);
}

}  // namespace cppurses
#endif  // CPPURSES_PAINTER_BRUSH_HPP

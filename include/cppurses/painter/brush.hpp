#ifndef PAINTER_BRUSH_HPP
#define PAINTER_BRUSH_HPP
#include <array>
#include <bitset>
#include <utility>
#include <vector>

#include <optional/optional.hpp>

#include <cppurses/painter/attribute.hpp>
#include <cppurses/painter/color.hpp>

namespace cppurses {

/// Holds the look of any paintable object with Attributes and Colors.
class Brush {
   public:
    template <typename... Attributes>
    explicit Brush(Attributes... attrs) {
        this->add_attributes(std::forward<Attributes>(attrs)...);
    }

    // Base Case
    void add_attributes() {}

    // Recursive Case
    template <typename T, typename... Others>
    void add_attributes(T attr, Others... others) {
        this->set_attr(attr);
        this->add_attributes(others...);
    }

    void remove_attribute(Attribute attr);
    void clear_attributes() { attributes_.reset(); }

    void set_background(Color color) { background_color_ = color; }
    void set_foreground(Color color) { foreground_color_ = color; }

    void remove_background() { background_color_ = opt::none; }
    void remove_foreground() { foreground_color_ = opt::none; }

    std::vector<Attribute> attributes() const;

    bool has_attribute(Attribute attr) const;

    const opt::Optional<Color>& background_color() const {
        return background_color_;
    }
    opt::Optional<Color>& background_color() { return background_color_; }
    const opt::Optional<Color>& foreground_color() const {
        return foreground_color_;
    }
    opt::Optional<Color>& foreground_color() { return foreground_color_; }

    friend bool operator==(const Brush& lhs, const Brush& rhs);

   private:
    void set_attr(detail::BackgroundColor bc) {
        this->set_background(static_cast<Color>(bc));
    }

    void set_attr(detail::ForegroundColor fc) {
        this->set_foreground(static_cast<Color>(fc));
    }

    void set_attr(Attribute attr);

    // Data Members
    std::bitset<8> attributes_;
    opt::Optional<Color> background_color_;
    opt::Optional<Color> foreground_color_;
};

}  // namespace cppurses
#endif  // PAINTER_BRUSH_HPP

#ifndef BRUSH_HPP
#define BRUSH_HPP

#include "attribute.hpp"
#include "color.hpp"

#include <aml/optional/optional.hpp>

#include <algorithm>
#include <utility>
#include <vector>

namespace mcurses {

class Brush {
   public:
    template <typename... Attributes>
    explicit Brush(Attributes... attrs) {
        this->add_attributes(std::forward<Attributes>(attrs)...);
    }

    // Recursive Case
    template <typename T, typename... Rest>
    void add_attributes(T t, Rest... rest) {
        this->set_attr(t);
        this->add_attributes(rest...);
    }

    // Base Case
    void add_attributes() {}

    void remove_attribute(Attribute attr);
    void clear_attributes() { attributes_.clear(); }

    void set_background(Color color) { background_color_ = color; }
    void set_foreground(Color color) { foreground_color_ = color; }

    std::vector<Attribute> attributes() const { return attributes_; }
    Optional<Color> background_color() const { return background_color_; }
    Optional<Color> foreground_color() const { return foreground_color_; }

    friend bool operator==(const Brush& x, const Brush& y) {
        return (std::is_permutation(
                    std::begin(x.attributes_), std::end(x.attributes_),
                    std::begin(y.attributes_), std::end(y.attributes_)) &&
                x.background_color_ == y.background_color_ &&
                x.foreground_color_ == y.foreground_color_);
    }

   private:
    void set_attr(detail::BackgroundColor bc) {
        this->set_background(static_cast<Color>(bc));
    }

    void set_attr(detail::ForegroundColor fc) {
        this->set_foreground(static_cast<Color>(fc));
    }

    void set_attr(Attribute attr) { this->push_attribute(attr); }

    void push_attribute(Attribute attr);

    std::vector<Attribute> attributes_;
    Optional<Color> background_color_;
    Optional<Color> foreground_color_;
};

}  // namespace mcurses
#endif  // BRUSH_HPP

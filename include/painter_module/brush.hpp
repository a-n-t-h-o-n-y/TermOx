#ifndef BRUSH_HPP
#define BRUSH_HPP

#include "attribute.hpp"
#include "color.hpp"

#include <aml/optional/optional.hpp>

#include <algorithm>
#include <utility>
#include <vector>
#include <bitset>

namespace twf {

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
    void clear_attributes() { attributes_.reset(); }

    void set_background(Color color) { background_color_ = color; }
    void set_foreground(Color color) { foreground_color_ = color; }

    std::vector<Attribute> attributes() const;
    opt::Optional<Color> background_color() const { return background_color_; }
    opt::Optional<Color> foreground_color() const { return foreground_color_; }

    friend bool operator==(const Brush& a, const Brush& b);

   private:
    void set_attr(detail::BackgroundColor bc) {
        this->set_background(static_cast<Color>(bc));
    }

    void set_attr(detail::ForegroundColor fc) {
        this->set_foreground(static_cast<Color>(fc));
    }

    void set_attr(Attribute attr);

    std::bitset<8> attributes_;
    opt::Optional<Color> background_color_;
    opt::Optional<Color> foreground_color_;
};

}  // namespace twf
#endif  // BRUSH_HPP

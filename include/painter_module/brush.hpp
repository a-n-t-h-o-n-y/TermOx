#ifndef PAINTER_MODULE_BRUSH_HPP
#define PAINTER_MODULE_BRUSH_HPP

#include "painter_module/attribute.hpp"
#include "painter_module/color.hpp"
#include <aml/optional/optional.hpp>
#include <algorithm>
#include <bitset>
#include <utility>
#include <vector>

namespace twf {

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

    std::vector<Attribute> attributes() const;
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

    std::bitset<8> attributes_;
    opt::Optional<Color> background_color_;
    opt::Optional<Color> foreground_color_;
};

}  // namespace twf
#endif  // PAINTER_MODULE_BRUSH_HPP

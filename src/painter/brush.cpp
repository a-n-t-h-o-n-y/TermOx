#include <cppurses/painter/brush.hpp>

#include <cstddef>
#include <cstdint>

#include <optional/optional.hpp>

#include <cppurses/painter/attribute.hpp>
#include <cppurses/painter/color.hpp>

namespace cppurses {

void Brush::remove_attribute(Attribute attr) {
    attributes_.set(static_cast<std::int8_t>(attr), false);
}

void Brush::clear_attributes() {
    attributes_.reset();
}

void Brush::remove_background() {
    background_color_ = opt::none;
}

void Brush::remove_foreground() {
    foreground_color_ = opt::none;
}

opt::Optional<Color> Brush::background_color() const {
    return background_color_;
}

opt::Optional<Color> Brush::foreground_color() const {
    return foreground_color_;
}

bool Brush::has_attribute(Attribute attr) const {
    return attributes_[static_cast<std::size_t>(attr)];
}

void Brush::set_attr(detail::BackgroundColor bc) {
    this->set_background(static_cast<Color>(bc));
}

void Brush::set_attr(detail::ForegroundColor fc) {
    this->set_foreground(static_cast<Color>(fc));
}

void Brush::set_attr(Attribute attr) {
    attributes_.set(static_cast<std::int8_t>(attr));
}

bool operator==(const Brush& lhs, const Brush& rhs) {
    return (lhs.attributes_ == rhs.attributes_ &&
            lhs.background_color_ == rhs.background_color_ &&
            lhs.foreground_color_ == rhs.foreground_color_);
}

}  // namespace cppurses

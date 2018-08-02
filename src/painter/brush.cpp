#include <cppurses/painter/brush.hpp>

#include <cstddef>
#include <cstdint>
#include <vector>

#include <optional/optional.hpp>

#include <cppurses/painter/attribute.hpp>

namespace cppurses {

void Brush::remove_attribute(Attribute attr) {
    attributes_.set(static_cast<std::int8_t>(attr), false);
}

std::vector<Attribute> Brush::attributes() const {
    std::vector<Attribute> vec;
    vec.reserve(8);
    for (std::size_t i = 0; i < attributes_.size(); ++i) {
        if (attributes_.test(i)) {
            vec.push_back(static_cast<Attribute>(i));
        }
    }
    return vec;
}

bool Brush::has_attribute(Attribute attr) const {
    return attributes_[static_cast<std::size_t>(attr)];
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

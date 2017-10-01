#include "painter/brush.hpp"
#include "painter/attribute.hpp"
#include <optional/optional.hpp>
#include <cstdint>
#include <vector>

namespace cppurses {

void Brush::remove_attribute(Attribute attr) {
    attributes_.set(static_cast<std::int8_t>(attr), false);
}

std::vector<Attribute> Brush::attributes() const {
    std::vector<Attribute> vec;
    vec.reserve(8);
    for (auto i = 0; i < attributes_.size(); ++i) {
        if (attributes_.test(i)) {
            vec.push_back(static_cast<Attribute>(i));
        }
    }
    return vec;
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

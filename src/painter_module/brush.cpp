#include <painter_module/attribute.hpp>
#include <painter_module/brush.hpp>

#include <cstdint>
#include <vector>

namespace twf {

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

bool operator==(const Brush& a, const Brush& b) {
    return (a.attributes_ == b.attributes_ &&
            a.background_color_ == b.background_color_ &&
            a.foreground_color_ == b.foreground_color_);
}

}  // namespace twf

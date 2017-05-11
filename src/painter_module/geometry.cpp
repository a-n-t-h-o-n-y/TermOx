#include "painter_module/geometry.hpp"

#include "widget_module/widget.hpp"
#include <cstddef>

namespace twf {
struct Size_policy;

void Geometry::set_width_hint(std::size_t hint) {
    width_hint_ = hint;
    this->update_parent();
}

void Geometry::set_height_hint(std::size_t hint) {
    height_hint_ = hint;
    this->update_parent();
}

void Geometry::set_max_width(std::size_t max_width) {
    max_width_ = max_width;
    this->update_parent();
}

void Geometry::set_max_height(std::size_t max_height) {
    max_height_ = max_height;
    this->update_parent();
}

void Geometry::set_min_width(std::size_t min_width) {
    min_width_ = min_width;
    this->update_parent();
}

void Geometry::set_min_height(std::size_t min_height) {
    min_height_ = min_height;
    this->update_parent();
}

void Geometry::set_size_policy(const Size_policy& policy) {
    size_policy_ = policy;
    this->update_parent();
}

void Geometry::update_parent() const {
    auto parent = dynamic_cast<Widget*>(widget_->parent());
    if (parent != nullptr) {
        parent->update();
    }
}

}  // namespace twf

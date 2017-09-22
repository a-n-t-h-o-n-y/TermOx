#include "painter/geometry.hpp"

#include "system/system.hpp"
#include "system/event.hpp"
#include "system/events/child_event.hpp"
#include "widget/widget.hpp"
#include <cstddef>
#include <memory>

namespace cppurses {
struct Size_policy;

void Geometry::set_width_hint(std::size_t hint) {
    width_hint_ = hint;
    this->notify_parent();  // should possibly post child_event
}

void Geometry::set_height_hint(std::size_t hint) {
    height_hint_ = hint;
    this->notify_parent();
}

void Geometry::set_max_width(std::size_t max_width) {
    max_width_ = max_width;
    this->notify_parent();
}

void Geometry::set_max_height(std::size_t max_height) {
    max_height_ = max_height;
    this->notify_parent();
}

void Geometry::set_min_width(std::size_t min_width) {
    min_width_ = min_width;
    this->notify_parent();
}

void Geometry::set_min_height(std::size_t min_height) {
    min_height_ = min_height;
    this->notify_parent();
}

void Geometry::set_size_policy(const Size_policy& policy) {
    size_policy_ = policy;
    this->notify_parent();
}

void Geometry::notify_parent() const {
    auto parent = widget_->parent();
    System::post_event<Child_polished_event>(parent, widget_);
}

}  // namespace cppurses

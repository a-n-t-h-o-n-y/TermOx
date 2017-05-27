#include "painter_module/geometry.hpp"

#include "system_module/system.hpp"
#include "system_module/event.hpp"
#include "system_module/events/child_event.hpp"
#include "widget_module/widget.hpp"
#include <cstddef>
#include <memory>

namespace twf {
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
    auto widg = std::make_unique<Child_event>(Event::ChildPolished, widget_);
    System::post_event(parent, std::move(widg));
}

}  // namespace twf

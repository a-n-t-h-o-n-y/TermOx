#include "widget/size_policy.hpp"
#include "system/events/child_event.hpp"
#include "system/system.hpp"
#include "widget/widget.hpp"
#include <cstddef>

namespace cppurses {

void Size_policy::type(Size_policy::Type type) {
    type_ = type;
    this->notify_parent();
}

Size_policy::Type Size_policy::type() const {
    return type_;
}

void Size_policy::stretch(std::size_t value) {
    stretch_ = value;
    this->notify_parent();
}

std::size_t Size_policy::stretch() const {
    return stretch_;
}

void Size_policy::hint(std::size_t value) {
    hint_ = value;
    this->notify_parent();
}

std::size_t Size_policy::hint() const {
    return hint_;
}

void Size_policy::min(std::size_t value) {
    min_ = value;
    this->notify_parent();
}

std::size_t Size_policy::min() const {
    return min_;
}

void Size_policy::max(std::size_t value) {
    max_ = value;
    this->notify_parent();
}

std::size_t Size_policy::max() const {
    return max_;
}

Size_policy::Size_policy(Widget* owner) : owner_{owner} {}

void Size_policy::notify_parent() const {
    System::post_event<Child_polished_event>(owner_->parent(), owner_);
}

}  // namespace cppurses

#include <cppurses/widget/size_policy.hpp>

#include <cstddef>

#include <cppurses/system/events/child_event.hpp>
#include <cppurses/system/system.hpp>
#include <cppurses/widget/widget.hpp>

namespace cppurses {

void Size_policy::notify_parent() const {
    if (owner_->parent() != nullptr) {
        System::post_event<Child_polished_event>(*(owner_->parent()), *owner_);
    }
}

}  // namespace cppurses

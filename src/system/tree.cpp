#include <cppurses/system/tree.hpp>

#include <cstddef>

#include <cppurses/system/events/resize_event.hpp>
#include <cppurses/system/system.hpp>
#include <cppurses/terminal/terminal.hpp>
#include <cppurses/widget/area.hpp>
#include <cppurses/widget/widget.hpp>

namespace cppurses {

Widget* Tree::head_ = nullptr;

void Tree::set_head(Widget* new_head) {
    if (head_ != nullptr) {
        head_->disable();
    }
    head_ = new_head;
    if (head_ != nullptr) {
        head_->enable();
        System::post_event<Resize_event>(
            *head_, Area{System::terminal.width(), System::terminal.height()});
    }
}

}  // namespace cppurses

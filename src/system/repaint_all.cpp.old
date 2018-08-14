#include <cppurses/system/detail/repaint_all.hpp>

#include <memory>

#include <cppurses/system/system.hpp>
#include <cppurses/widget/widget.hpp>

namespace {
using namespace cppurses;
void update_recursion(Widget* w) {
    if (w == nullptr) {
        return;
    }
    w->update();
    for (const std::unique_ptr<Widget>& wp : w->children()) {
        update_recursion(wp.get());
    }
}
}  // namespace

namespace cppurses {
namespace detail {

void repaint_all() {
    System::paint_buffer().set_repaint_all();
    update_recursion(System::head());
}

void post_repaint_recursive(Widget* w) {
    if (w == nullptr) {
        return;
    }
    System::post_event<Repaint_event>(w);
    for (const std::unique_ptr<Widget>& child : w->children()) {
        post_repaint_recursive(child.get());
    }
}

}  // namespace detail
}  // namespace cppurses

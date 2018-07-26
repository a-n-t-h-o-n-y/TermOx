#include <cppurses/system/detail/repaint_all.hpp>

#include <memory>

#include <cppurses/system/system.hpp>
#include <cppurses/widget/widget.hpp>

namespace {
using namespace cppurses;
void repaint_children(Widget* w) {
    if (w == nullptr) {
        return;
    }
    for (Widget* wp : w->children()) {
        System::post_event(std::make_unique<Repaint_event>(wp));
        repaint_children(wp);
    }
}
}  // namespace

namespace cppurses {
namespace detail {

void repaint_all() {
    repaint_children(System::head());
}

}  // namespace detail
}  // namespace cppurses

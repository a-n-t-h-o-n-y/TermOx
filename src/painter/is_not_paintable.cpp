#include <cppurses/painter/detail/is_not_paintable.hpp>

#include <cppurses/widget/widget.hpp>

namespace cppurses {
namespace detail {

// TODO should be protected with mutex, not this but each function that it
// calls, since these could be modified while flushing.. and this is used while
// flushing.
bool is_not_paintable(const Widget* w) {
    return !w->on_tree() || !w->visible() || (w->width() == 0) ||
           (w->height() == 0);
}

}  // namespace detail
}  // namespace cppurses

#include <cppurses/painter/detail/is_not_paintable.hpp>

#include <cppurses/widget/widget.hpp>

namespace cppurses {
namespace detail {

bool is_not_paintable(const Widget* w) {
    return !w->on_tree() || !w->visible() || (w->width() == 0) ||
           (w->height() == 0);
}

}  // namespace detail
}  // namespace cppurses

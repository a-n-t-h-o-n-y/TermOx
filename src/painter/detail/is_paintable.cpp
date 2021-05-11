#include <termox/painter/detail/is_paintable.hpp>

#include <termox/widget/widget.hpp>

namespace ox::detail {

auto is_paintable(Widget const& w) -> bool
{
    return w.is_enabled() && (w.outer_width() != 0) && (w.outer_height() != 0);
}

}  // namespace ox::detail

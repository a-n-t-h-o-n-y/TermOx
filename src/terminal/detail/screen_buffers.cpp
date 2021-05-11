#include <termox/terminal/detail/screen_buffers.hpp>

#include <termox/terminal/detail/canvas.hpp>
#include <termox/widget/area.hpp>

namespace ox::detail {

Screen_buffers::Screen_buffers(ox::Area a) : current{a}, next{a} {}

void Screen_buffers::resize(ox::Area a)
{
    current.resize(a);
    next.resize(a);
}

auto Screen_buffers::area() const -> Area { return current.area(); }

void Screen_buffers::merge() { ::ox::detail::merge(next, current); }

auto Screen_buffers::merge_and_diff() -> Canvas::Diff const&
{
    ::ox::detail::merge_and_diff(next, current, diff_);
    return diff_;
}

auto Screen_buffers::generate_color_diff(Color c) -> Canvas::Diff const&
{
    ::ox::detail::generate_color_diff(c, current, diff_);
    return diff_;
}

auto Screen_buffers::current_screen_as_diff() -> Canvas::Diff const&
{
    ::ox::detail::generate_full_diff(current, diff_);
    return diff_;
}

}  // namespace ox::detail

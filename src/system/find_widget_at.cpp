#include <cppurses/system/detail/find_widget_at.hpp>

#include <cstddef>
#include <memory>
#include <vector>

#include <cppurses/system/system.hpp>
#include <cppurses/widget/widget.hpp>

namespace {

auto has_coordinates(cppurses::Widget const& w,
                     std::size_t global_x,
                     std::size_t global_y) -> bool
{
    if (!w.is_enabled())
        return false;
    bool const within_west  = global_x >= w.inner_x();
    bool const within_east  = global_x < (w.inner_x() + w.width());
    bool const within_north = global_y >= w.inner_y();
    bool const within_south = global_y < (w.inner_y() + w.height());
    return within_west and within_east and within_north and within_south;
}

}  // namespace

namespace cppurses {
namespace detail {

auto find_widget_at(std::size_t x, std::size_t y) -> Widget*
{
    auto* widg = System::head();
    if (widg == nullptr or not has_coordinates(*widg, x, y))
        return nullptr;
    auto keep_going = true;
    while (keep_going and not widg->get_children().empty()) {
        // TODO pipe::children() | filter(if...) | for_each([&]..);
        // but it has a break, see if there is some algorithm that can be used.
        for (auto& child : widg->get_children()) {
            if (has_coordinates(child, x, y) and child.is_enabled()) {
                widg       = &child;
                keep_going = true;
                break;
            }
            keep_going = false;
        }
    }
    return widg;
}

}  // namespace detail
}  // namespace cppurses

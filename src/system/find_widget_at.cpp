#include <cppurses/system/detail/find_widget_at.hpp>

#include <cstddef>
#include <memory>
#include <vector>

#include <cppurses/system/system.hpp>
#include <cppurses/widget/widget.hpp>

namespace {
bool has_coordinates(const cppurses::Widget& w,
                     std::size_t global_x,
                     std::size_t global_y)
{
    if (!w.enabled()) {
        return false;
    }
    const bool within_west  = global_x >= w.inner_x();
    const bool within_east  = global_x < (w.inner_x() + w.width());
    const bool within_north = global_y >= w.inner_y();
    const bool within_south = global_y < (w.inner_y() + w.height());
    return within_west && within_east && within_north && within_south;
}
}  // namespace

namespace cppurses {
namespace detail {

Widget* find_widget_at(std::size_t x, std::size_t y)
{
    Widget* widg = System::head();
    if (widg == nullptr || !has_coordinates(*widg, x, y))
        return nullptr;
    auto keep_going = true;
    while (keep_going && !widg->get_children().empty()) {
        for (auto& child : widg->get_children()) {
            if (has_coordinates(child, x, y) && child.enabled()) {
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

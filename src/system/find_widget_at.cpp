#include <cppurses/system/detail/find_widget_at.hpp>

#include <cppurses/system/system.hpp>
#include <cppurses/widget/point.hpp>
#include <cppurses/widget/widget.hpp>

namespace {

auto contains(cppurses::Widget const& w, cppurses::Point global) -> bool
{
    bool const within_west  = global.x >= w.inner_x();
    bool const within_east  = global.x < (w.inner_x() + w.width());
    bool const within_north = global.y >= w.inner_y();
    bool const within_south = global.y < (w.inner_y() + w.height());
    return within_west && within_east && within_north && within_south;
}

/// Returns a descendant of w that owns \p p, or nullptr if none found.
auto find_owner_of(cppurses::Widget& w, cppurses::Point p) -> cppurses::Widget*
{
    if (!w.is_enabled() || !contains(w, p))
        return nullptr;
    for (auto& child : w.get_children()) {
        if (cppurses::Widget* owner = find_owner_of(child, p); owner != nullptr)
            return owner;
    }
    return &w;
}

}  // namespace

namespace cppurses::detail {

auto find_widget_at(Point p) -> Widget*
{
    if (auto* head = System::head(); head == nullptr)
        return nullptr;
    else
        return find_owner_of(*head, p);
}

}  // namespace cppurses::detail

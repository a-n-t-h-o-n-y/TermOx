#include <termox/system/detail/find_widget_at.hpp>

#include <termox/system/system.hpp>
#include <termox/widget/point.hpp>
#include <termox/widget/widget.hpp>

namespace {

/// Return true if \p global is within the (non-bordered) area of \p w.
[[nodiscard]] auto contains(ox::Widget const& w, ox::Point global) -> bool
{
    bool const within_west  = global.x >= w.top_left().x;
    bool const within_east  = global.x < (w.top_left().x + w.area().width);
    bool const within_north = global.y >= w.top_left().y;
    bool const within_south = global.y < (w.top_left().y + w.area().height);
    return within_west && within_east && within_north && within_south;
}

/// Returns a descendant of w that owns \p p, or nullptr if none found.
[[nodiscard]] auto find_owner_of(ox::Widget& w, ox::Point p) -> ox::Widget*
{
    if (!w.is_enabled() || !contains(w, p))
        return nullptr;
    for (auto& child : w.get_children()) {
        if (ox::Widget* owner = find_owner_of(child, p); owner != nullptr)
            return owner;
    }
    return &w;
}

}  // namespace

namespace ox::detail {

[[nodiscard]] auto find_widget_at(Point p) -> Widget*
{
    if (auto* head = System::head(); head == nullptr)
        return nullptr;
    else {
        // Some terminals allow clicks outside of term screen, so return head.
        auto* const at = find_owner_of(*head, p);
        return (at == nullptr) ? ox::System::head() : at;
    }
}

}  // namespace ox::detail

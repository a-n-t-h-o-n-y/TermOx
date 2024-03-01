#include <termox/widgets/layout.hpp>

#include <algorithm>
#include <cassert>
#include <functional>
#include <iterator>
#include <numeric>
#include <utility>
#include <vector>

#include <termox/widgets/focus.hpp>

namespace {
using namespace ox::widgets;

enum class Direction : bool { Horizontal, Vertical };

/**
 * Returns a pointer to the child at the given point, or nullptr if there is no
 * child at that point.
 * @tparam D The direction to search in.
 * @param children The children to search.
 * @param p The point to search for.
 * @return A pointer to the child at the given point, or nullptr if there is no
 * child at that point.
 */
template <Direction D>
[[nodiscard]] auto child_at(std::vector<Widget>& children, ox::Point p)
    -> Widget*
{
    if constexpr (D == Direction::Horizontal) {
        auto const iter = std::ranges::lower_bound(
            children, p.x, std::less{}, [](auto const& widg) {
                return widg.properties.at.x + widg.properties.size.width - 1;
            });
        return iter != std::end(children) ? &*iter : nullptr;
    }
    else {
        auto const iter = std::ranges::lower_bound(
            children, p.y, std::less{}, [](auto const& widg) {
                return widg.properties.at.y + widg.properties.size.height - 1;
            });
        return iter != std::end(children) ? &*iter : nullptr;
    }
}

/**
 * If there is a child at the given point, call the given event function with
 * the child and the mouse object.
 * @tparam D The direction to search in.
 * @param layout The layout to search in.
 * @param m The mouse event object.
 * @param event_fn The event function to call with the child and mouse object.
 */
template <Direction D, typename EventFn>
auto any_mouse_event(LinearLayout& layout, ox::Mouse m, EventFn&& event_fn)
    -> void
{
    auto const widg_ptr = child_at<D>(layout.children, m.at);

    if (widg_ptr != nullptr) {
        auto const properties = widg_ptr->properties;

        if (properties.focus_policy == FocusPolicy::Strong ||
            properties.focus_policy == FocusPolicy::Click) {
            Focus::set(*widg_ptr);
        }

        m.at = m.at - properties.at;
        std::forward<EventFn>(event_fn)(*widg_ptr, m);
    }
}

/**
 * Calculate the length of each child in the given total space based on the
 * its SizePolicy and the total space available.
 *
 * @param children The children to distribute the space between.
 * @param total The total space to distribute.
 * @return A vector of the lengths of each child.
 */
[[nodiscard]] auto distribute_length(std::vector<Widget>& children, int total)
    -> std::vector<int>
{
    assert(total >= 0);
    for (auto const& child : children) {
        auto const& [min, max, flex] = child.properties.size_policy;
        assert(min >= 0);
        assert(max >= min);
        assert(flex >= 0);
    }

    auto exact_amounts   = std::vector<float>(children.size(), 0.f);
    auto total_allocated = 0.f;

    // Distribute minimums first
    for (auto i = std::size_t{0}; i < children.size(); ++i) {
        exact_amounts[i] = (float)children[i].properties.size_policy.min;
        total_allocated += exact_amounts[i];
    }

    // Distribute flex space
    auto remaining_space = (float)total - total_allocated;
    while (remaining_space > 0) {
        auto const total_flex = [&] {
            auto x = 0.f;
            for (auto i = std::size_t{0}; i < children.size(); ++i) {
                auto const& policy = children[i].properties.size_policy;
                if (exact_amounts[i] < (float)policy.max) {
                    x += policy.flex;
                }
            }
            return x;
        }();

        if (total_flex == 0.f) {
            break;
        }

        auto space_distributed_this_round = 0.f;
        for (auto i = std::size_t{0}; i < children.size(); ++i) {
            auto const& policy = children[i].properties.size_policy;
            if (exact_amounts[i] < (float)policy.max) {
                float additional_space =
                    std::min(policy.flex / total_flex * (float)remaining_space,
                             (float)policy.max - exact_amounts[i]);
                exact_amounts[i] += additional_space;
                space_distributed_this_round += additional_space;
            }
        }
        remaining_space -= space_distributed_this_round;
        if (space_distributed_this_round == 0) {
            break;
        }
    }
    auto results = std::vector<int>(children.size(), 0);

    // floor values to ints.
    std::ranges::copy(exact_amounts, results.begin());

    // Distribute remaining space from left from flooring.
    int remaining =
        total - std::accumulate(std::begin(results), std::end(results), 0);
    while (remaining > 0) {
        auto space_distributed_this_round = 0;
        for (auto i = std::size_t{0}; i < children.size(); ++i) {
            if (results[i] < children[i].properties.size_policy.max) {
                results[i] += 1;
                remaining -= 1;
                ++space_distributed_this_round;
                if (remaining == 0) {
                    break;
                }
            }
        }
        if (space_distributed_this_round == 0) {
            break;
        }
    }
    return results;
}

}  // namespace

namespace ox::widgets {

// -----------------------------------------------------------------------------

auto paint(LinearLayout const& layout, ox::Canvas c) -> void
{
    for (auto const& child : layout.children) {
        paint(child, {c.at + child.properties.at, child.properties.size});
    }
}

auto timer(LinearLayout& layout, int id) -> void
{
    for (auto& child : layout.children) {
        timer(child, id);
    }
}

auto find_next_tab_focus(LinearLayout& layout,
                         Widget const* current_focus,
                         bool is_active) -> Widget*
{
    for (auto& child : layout.children) {
        auto const focus_policy = child.properties.focus_policy;
        if (is_active && (focus_policy == FocusPolicy::Strong ||
                          focus_policy == FocusPolicy::Tab)) {
            return &child;
        }

        if (&child == current_focus) {
            is_active = !is_active;
        }

        Widget* const next =
            find_next_tab_focus(child, current_focus, is_active);
        if (next != nullptr) {
            return next;
        }
    }
    return nullptr;
}

auto for_each(LinearLayout& layout, std::function<void(Widget&)> const& fn)
    -> void
{
    for (auto& child : layout.children) {
        fn(child);
        for_each(child, fn);
    }
}

auto for_each(LinearLayout const& layout,
              std::function<void(Widget const&)> const& fn) -> void
{
    for (auto const& child : layout.children) {
        fn(child);
        for_each(child, fn);
    }
}

auto find_if(LinearLayout& layout,
             std::function<bool(Widget const&)> const& predicate) -> Widget*
{
    Widget* result = nullptr;
    for_each(layout, [&](Widget& w) {
        if (result == nullptr && predicate(w)) {
            result = &w;
        }
    });
    return result;
}

auto find_if(LinearLayout const& layout,
             std::function<bool(Widget const&)> const& predicate)
    -> Widget const*
{
    Widget const* result = nullptr;
    for_each(layout, [&](Widget const& w) {
        if (result == nullptr && predicate(w)) {
            result = &w;
        }
    });
    return result;
}

// -----------------------------------------------------------------------------

auto mouse_press(HLayout& layout, Mouse m) -> void
{
    ::any_mouse_event<Direction::Horizontal>(
        layout, m, [](auto& w, auto m) { mouse_press(w, m); });
}

auto mouse_release(HLayout& layout, Mouse m) -> void
{
    ::any_mouse_event<Direction::Horizontal>(
        layout, m, [](auto& w, auto m) { mouse_release(w, m); });
}

auto mouse_wheel(HLayout& layout, Mouse m) -> void
{
    ::any_mouse_event<Direction::Horizontal>(
        layout, m, [](auto& w, auto m) { mouse_wheel(w, m); });
}

auto mouse_move(HLayout& layout, Mouse m) -> void
{
    ::any_mouse_event<Direction::Horizontal>(
        layout, m, [](auto& w, auto m) { mouse_move(w, m); });
}

auto resize(HLayout& layout, Area a) -> void
{
    auto x            = 0;
    auto const widths = ::distribute_length(layout.children, a.width);
    for (auto i = std::size_t{0}; i < layout.children.size(); ++i) {
        auto& child           = layout.children[i];
        child.properties.at   = {x, 0};
        child.properties.size = {widths[i], a.height};
        resize(child, child.properties.size);
        x += widths[i];
    }
}

// -----------------------------------------------------------------------------

auto mouse_press(VLayout& layout, Mouse m) -> void
{
    ::any_mouse_event<Direction::Vertical>(
        layout, m, [](auto& w, auto m) { mouse_press(w, m); });
}

auto mouse_release(VLayout& layout, Mouse m) -> void
{
    ::any_mouse_event<Direction::Vertical>(
        layout, m, [](auto& w, auto m) { mouse_release(w, m); });
}

auto mouse_wheel(VLayout& layout, Mouse m) -> void
{
    ::any_mouse_event<Direction::Vertical>(
        layout, m, [](auto& w, auto m) { mouse_wheel(w, m); });
}

auto mouse_move(VLayout& layout, Mouse m) -> void
{
    ::any_mouse_event<Direction::Vertical>(
        layout, m, [](auto& w, auto m) { mouse_move(w, m); });
}

auto resize(VLayout& layout, ox::Area a) -> void
{
    auto y             = 0;
    auto const heights = ::distribute_length(layout.children, a.height);
    for (auto i = std::size_t{0}; i < layout.children.size(); ++i) {
        auto& child           = layout.children[i];
        child.properties.at   = {0, y};
        child.properties.size = {a.width, heights[i]};
        resize(child, child.properties.size);
        y += heights[i];
    }
}

}  // namespace ox::widgets
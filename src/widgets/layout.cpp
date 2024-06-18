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
[[nodiscard]] auto child_at(std::vector<Widget>& children,
                            ox::Point p) -> Widget*
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
auto any_mouse_event(LinearLayout& layout,
                     ox::Mouse m,
                     EventFn&& event_fn) -> void
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
[[nodiscard]] auto distribute_length(std::vector<SizePolicy>& size_policies,
                                     int total) -> std::vector<int>
{
    assert(total >= 0);

    for (auto const& policy : size_policies) {
        assert(policy.minimum >= 0);
        assert(policy.maximum >= policy.minimum);
        assert(policy.flexibility >= 0);
    }

    auto exact_amounts   = std::vector<float>(size_policies.size(), 0.f);
    auto total_allocated = 0.f;

    // Distribute minimums first
    for (auto i = std::size_t{0}; i < size_policies.size(); ++i) {
        exact_amounts[i] = (float)size_policies[i].minimum;
        total_allocated += exact_amounts[i];
    }

    // Distribute flex space
    auto remaining_space = (float)total - total_allocated;
    while (remaining_space > 0) {
        auto const total_flex = [&] {
            auto x = 0.f;
            for (auto i = std::size_t{0}; i < size_policies.size(); ++i) {
                auto const& policy = size_policies[i];
                if (exact_amounts[i] < (float)policy.maximum) {
                    x += policy.flexibility;
                }
            }
            return x;
        }();

        if (total_flex == 0.f) {
            break;
        }

        auto space_distributed_this_round = 0.f;
        for (auto i = std::size_t{0}; i < size_policies.size(); ++i) {
            auto const& policy = size_policies[i];
            if (exact_amounts[i] < (float)policy.maximum) {
                float additional_space = std::min(
                    policy.flexibility / total_flex * (float)remaining_space,
                    (float)policy.maximum - exact_amounts[i]);
                exact_amounts[i] += additional_space;
                space_distributed_this_round += additional_space;
            }
        }
        remaining_space -= space_distributed_this_round;
        if (space_distributed_this_round == 0) {
            break;
        }
    }
    auto results = std::vector<int>(size_policies.size(), 0);

    // floor values to ints.
    std::ranges::copy(exact_amounts, results.begin());

    // Distribute remaining space from left from flooring.
    int remaining =
        total - std::accumulate(std::begin(results), std::end(results), 0);
    while (remaining > 0) {
        auto space_distributed_this_round = 0;
        for (auto i = std::size_t{0}; i < size_policies.size(); ++i) {
            if (results[i] < size_policies[i].maximum) {
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

auto paint(LinearLayout const& layout, ox::Canvas canvas) -> void
{
    for (Widget const& child : layout.children) {
        paint(child, {canvas.at + child.properties.at, child.properties.size});
    }
}

auto timer(LinearLayout& layout, int id) -> void
{
    for (Widget& child : layout.children) {
        timer(child, id);
    }
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
    auto const widths = ::distribute_length(layout.size_policies, a.width);
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
    auto const heights = ::distribute_length(layout.size_policies, a.height);
    for (auto i = std::size_t{0}; i < layout.children.size(); ++i) {
        auto& child           = layout.children[i];
        child.properties.at   = {0, y};
        child.properties.size = {a.width, heights[i]};
        resize(child, child.properties.size);
        y += heights[i];
    }
}

}  // namespace ox::widgets
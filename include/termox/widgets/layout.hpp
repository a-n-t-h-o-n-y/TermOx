#pragma once

#include <algorithm>
#include <cstddef>
#include <functional>
#include <ranges>

#include <termox/widgets/focus.hpp>
#include <termox/widgets/widget.hpp>

namespace ox::widgets {

enum class FocusPolicy : std::uint8_t { None, Tab, Click, Strong };

/**
 * A layout that arranges its children in a line, either horizontally or
 * vertically. Do not use directly, instead use HLayout or VLayout.
 */
struct LinearLayout {
    struct MetaData {
        Point at;
        Area size;
        bool enabled = true;
        // TODO size policy

        FocusPolicy focus_policy = FocusPolicy::None;
    };

    struct Item {
        Widget child;
        MetaData metadata;
    };

    std::vector<Item> children;
};

enum class Direction : bool { Horizontal, Vertical };

// -----------------------------------------------------------------------------

/**
 * Appends a widget to the layout.
 *
 * @param t The widget to append.
 * @param md The metadata to apply to the widget.
 * @return A reference to the appended widget. This reference will remain
 * valid until the widget is removed from the layout. It is safe to store in
 * Signal connections.
 */
template <typename T>
auto append(LinearLayout& layout, T t, LinearLayout::MetaData md = {}) -> T&
{
    return layout.children.emplace_back(std::move(t), std::move(md))
        .child.template data<T>();
}

/**
 * Inserts a widget into the layout at the given index.
 *
 * @param index The index to insert the widget at. If this is greater than the
 * current number of children, the widget will be appended to the end of the
 * layout.
 * @param t The widget to insert.
 * @param md The metadata to apply to the widget.
 * @return A reference to the inserted widget. This reference will remain
 * valid until the widget is removed from the layout. It is safe to store in
 * Signal connections.
 */
template <typename T>
auto insert(LinearLayout& layout,
            std::size_t index,
            T t,
            LinearLayout::MetaData md = {}) -> T&
{
    return layout.children
        .emplace(std::next(std::begin(layout.children), index), std::move(t),
                 std::move(md))
        ->child.template data<T>();
}

// -----------------------------------------------------------------------------

namespace detail {

/**
 * Returns a pointer to the child at the given point, or nullptr if there is no
 * child at that point.
 *
 * @tparam D The direction to search in.
 * @param children The children to search.
 * @param p The point to search for.
 * @return A pointer to the child at the given point, or nullptr if there is no
 * child at that point.
 */
template <Direction D>
[[nodiscard]] auto child_at(std::vector<LinearLayout::Item>& children,
                            ox::Point p) -> LinearLayout::Item*
{
    if constexpr (D == Direction::Horizontal) {
        auto const iter = std::ranges::lower_bound(
            children, p.x, std::less{}, [](auto const& item) {
                return item.metadata.at.x + item.metadata.size.width - 1;
            });
        return iter != std::end(children) ? &*iter : nullptr;
    }
    else {
        auto const iter = std::ranges::lower_bound(
            children, p.y, std::less{}, [](auto const& item) {
                return item.metadata.at.y + item.metadata.size.height - 1;
            });
        return iter != std::end(children) ? &*iter : nullptr;
    }
}

template <Direction D, typename EventFn>
auto any_mouse_event(LinearLayout& layout, Mouse m, EventFn&& event_fn) -> void
{
    auto const item_ptr = child_at<D>(layout.children, m.at);

    if (item_ptr != nullptr) {
        auto& [child, meta] = *item_ptr;

        if (meta.focus_policy == FocusPolicy::Strong ||
            meta.focus_policy == FocusPolicy::Click) {
            Focus::set(child);
        }

        m.at = m.at - meta.at;
        std::forward<EventFn>(event_fn)(child, m);
    }
}

}  // namespace detail

// -----------------------------------------------------------------------------

inline auto paint(LinearLayout const& layout, ox::Canvas c) -> void
{
    for (auto const& [child, meta] : layout.children) {
        paint(child, {c.at + meta.at, meta.size});
    }
}

// -----------------------------------------------------------------------------

struct HLayout : LinearLayout {};

inline auto mouse_press(HLayout& layout, Mouse m) -> void
{
    detail::any_mouse_event<Direction::Horizontal>(
        layout, m, [](auto& w, auto m) { mouse_press(w, m); });
}

inline auto mouse_release(HLayout& layout, Mouse m) -> void
{
    detail::any_mouse_event<Direction::Horizontal>(
        layout, m, [](auto& w, auto m) { mouse_release(w, m); });
}

inline auto mouse_wheel(HLayout& layout, Mouse m) -> void
{
    detail::any_mouse_event<Direction::Horizontal>(
        layout, m, [](auto& w, auto m) { mouse_wheel(w, m); });
}

inline auto mouse_move(HLayout& layout, Mouse m) -> void
{
    detail::any_mouse_event<Direction::Horizontal>(
        layout, m, [](auto& w, auto m) { mouse_move(w, m); });
}

inline auto resize(HLayout& layout, Area a) -> void
{
    if (layout.children.empty()) {
        return;
    }

    auto const child_width = a.width / (int)layout.children.size();
    auto const last_child_width =
        a.width - (child_width * (layout.children.size() - 1));
    auto const child_height = a.height;

    auto x = 0;
    for (auto& [child, meta] : layout.children) {
        meta.at   = {x, 0};
        meta.size = {child_width, child_height};
        resize(child, meta.size);
        x += child_width;
    }
    layout.children.back().metadata.size.width = last_child_width;
}

// -----------------------------------------------------------------------------

struct VLayout : LinearLayout {};

inline auto mouse_press(VLayout& layout, Mouse m) -> void
{
    detail::any_mouse_event<Direction::Vertical>(
        layout, m, [](auto& w, auto m) { mouse_press(w, m); });
}

inline auto mouse_release(VLayout& layout, Mouse m) -> void
{
    detail::any_mouse_event<Direction::Vertical>(
        layout, m, [](auto& w, auto m) { mouse_release(w, m); });
}

inline auto mouse_wheel(VLayout& layout, Mouse m) -> void
{
    detail::any_mouse_event<Direction::Vertical>(
        layout, m, [](auto& w, auto m) { mouse_wheel(w, m); });
}

inline auto mouse_move(VLayout& layout, Mouse m) -> void
{
    detail::any_mouse_event<Direction::Vertical>(
        layout, m, [](auto& w, auto m) { mouse_move(w, m); });
}

inline auto resize(VLayout& vl, ox::Area a) -> void
{
    if (vl.children.empty()) {
        return;
    }

    auto const child_height = a.height / (int)vl.children.size();
    auto const last_child_height =
        a.height - (child_height * (vl.children.size() - 1));
    auto const child_width = a.width;

    auto y = 0;
    for (auto& [child, meta] : vl.children) {
        meta.at   = {0, y};
        meta.size = {child_width, child_height};
        resize(child, meta.size);
        y += child_height;
    }
    vl.children.back().metadata.size.height = last_child_height;
}

// paint - done
// mouse press - done
// key press - no
// key release - no
// mouse release - done
// mouse wheel - done
// mouse move - done

// resize - partial
// focus in / out - add these
// timer event - no - handled in registry / application

}  // namespace ox::widgets
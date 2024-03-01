#pragma once

#include <cstddef>
#include <functional>
#include <iterator>
#include <utility>
#include <vector>

#include <termox/widgets/widget.hpp>

namespace ox::widgets {

/**
 * A layout that arranges its children in a line, either horizontally or
 * vertically. Do not use directly, instead use HLayout or VLayout.
 */
struct LinearLayout {
    std::vector<Widget> children;
};

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
auto append(LinearLayout& layout, T t, Widget::Properties p = {}) -> T&
{
    return layout.children.emplace_back(std::move(t), std::move(p))
        .template data<T>();
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
            Widget::Properties p = {}) -> T&
{
    return layout.children
        .emplace(std::next(std::begin(layout.children), index), std::move(t),
                 std::move(p))
        ->template data<T>();
}

// -----------------------------------------------------------------------------

auto paint(LinearLayout const& layout, ox::Canvas c) -> void;

auto timer(LinearLayout& layout, int id) -> void;

[[nodiscard]] auto find_next_tab_focus(LinearLayout& layout,
                                       Widget const* current_focus,
                                       bool is_active) -> Widget*;

auto for_each(LinearLayout& layout, std::function<void(Widget&)> const& fn)
    -> void;

auto for_each(LinearLayout const& layout,
              std::function<void(Widget const&)> const& fn) -> void;

auto find_if(LinearLayout& layout,
             std::function<bool(Widget const&)> const& predicate) -> Widget*;

auto find_if(LinearLayout const& layout,
             std::function<bool(Widget const&)> const& predicate)
    -> Widget const*;

// -----------------------------------------------------------------------------

struct HLayout : LinearLayout {};

auto mouse_press(HLayout& layout, Mouse m) -> void;

auto mouse_release(HLayout& layout, Mouse m) -> void;

auto mouse_wheel(HLayout& layout, Mouse m) -> void;

auto mouse_move(HLayout& layout, Mouse m) -> void;

auto resize(HLayout& layout, Area a) -> void;

// -----------------------------------------------------------------------------

struct VLayout : LinearLayout {};

auto mouse_press(VLayout& layout, Mouse m) -> void;

auto mouse_release(VLayout& layout, Mouse m) -> void;

auto mouse_wheel(VLayout& layout, Mouse m) -> void;

auto mouse_move(VLayout& layout, Mouse m) -> void;

auto resize(VLayout& layout, ox::Area a) -> void;

}  // namespace ox::widgets
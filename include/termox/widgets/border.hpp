#pragma once

#include <termox/core.hpp>
#include <termox/widgets/widget.hpp>

namespace ox::widgets {

/**
 * A Border display that wraps a child widget.
 *
 * @details The child Widget inherits the FocusPolicy of the Border object, and
 * is sized to fit within the Border.
 */
struct Border {
    Widget child;
    Painter::Box box = {};
};

auto paint(Border const& b, Canvas c) -> void;

auto resize(Border& b, Area new_size) -> void;

auto focus_in(Border& b) -> void;

auto timer(Border& b, int id) -> void;

auto mouse_press(Border& b, Mouse m) -> void;

auto mouse_release(Border& b, Mouse m) -> void;

auto mouse_wheel(Border& b, Mouse m) -> void;

auto mouse_move(Border& b, Mouse m) -> void;

}  // namespace ox::widgets
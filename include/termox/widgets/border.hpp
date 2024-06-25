#pragma once

#include <span>

#include <termox/core.hpp>
#include <termox/widgets/widget.hpp>

namespace ox::widgets {

/**
 * A Border display that wraps a child widget.
 *
 * @details The child Widget inherits the FocusPolicy of the Border object, and is sized
 * to fit within the Border.
 */
struct Border {
    template <typename T>
    explicit Border(T child_, Painter::Box box_ = {})
        : child{Widget{std::move(child_), {}}}, box{std::move(box_)}
    {}

    Widget child;
    Painter::Box box;
};

auto children(Border& b) -> std::span<Widget>;

auto children(Border const& w) -> std::span<Widget const>;

auto paint(Border const& b, Canvas c) -> void;

auto resize(Border& b, Area new_size) -> void;

auto focus_in(Border& b) -> void;

// -------------------------------------------------------------------------------------

struct Divider {
    ox::Glyph line;
};

inline auto paint(Divider const& d, Canvas c) -> void { Painter{c}.fill(d.line); }

}  // namespace ox::widgets
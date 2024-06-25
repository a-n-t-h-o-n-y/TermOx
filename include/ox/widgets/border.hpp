#pragma once

#include <span>

#include <ox/core.hpp>
#include <ox/widgets/widget.hpp>

namespace ox {

/**
 * A Border display that wraps a child widget.
 *
 * @details The child Widget inherits the FocusPolicy of the Border object, and is sized
 * to fit within the Border.
 */
struct Border {
    Widget child;
    Painter::Box box;
};

struct BorderInit {
    Widget child;
    Painter::Box box = {};
};

[[nodiscard]] auto border(BorderInit x) -> Border;

auto paint(Border const& b, Canvas c) -> void;

auto resize(Border& b, Area new_size) -> void;

auto focus_in(Border& b) -> void;

auto children(Border& b) -> std::span<Widget>;

auto children(Border const& w) -> std::span<Widget const>;

// -------------------------------------------------------------------------------------

struct Divider {
    Glyph line;
};

[[nodiscard]] auto divider(Glyph line) -> Divider;

inline auto paint(Divider const& d, Canvas c) -> void { Painter{c}.fill(d.line); }

}  // namespace ox
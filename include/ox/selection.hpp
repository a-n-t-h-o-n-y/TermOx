#pragma once

#include <functional>
#include <optional>
#include <ox/align.hpp>
#include <ox/core/canvas.hpp>
#include <ox/core/text.hpp>
#include <span>
#include <vector>

namespace ox {
/** Sorted row indices with an independent keyboard cursor and range anchor. */
struct RowSelection {
    std::vector<std::size_t> rows;
    std::optional<std::size_t> current;
    std::optional<std::size_t> anchor;
    auto operator==(RowSelection const&) const -> bool = default;
};
/** Single, replace-range, toggle, or cursor-only selection. */
enum class SelectionAction { Replace, Extend, Toggle, Move };
auto select_rows(RowSelection& state,
                 std::size_t row,
                 std::size_t count,
                 SelectionAction action = SelectionAction::Replace,
                 std::function<bool(std::size_t)> const& enabled = {}) -> bool;
auto row_navigation(KeyEvent const&, int page) -> std::optional<int>;
auto row_target(std::optional<std::size_t> current,
                std::size_t count,
                int steps,
                std::function<bool(std::size_t)> const& enabled = {})
    -> std::optional<std::size_t>;
/** Visible content rows, half-open, after clipping. */
auto visible_rows(Canvas, int offset, std::size_t count) -> std::pair<int, int>;
/** Shared Unicode clipping/ellipsis painter; layout storage belongs to the caller. */
void paint_row_text(Canvas, std::string_view, Align, Style, TextLayout&);
void validate_row_text(std::string_view);
}  // namespace ox

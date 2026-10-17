/** @file
 * Row selection state, keyboard navigation, and row text painting.
 */
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
    /** Sorted unique selected row indices; selection commands maintain this invariant. */
    std::vector<std::size_t> rows;
    /** Optional current navigation row, independent of selected rows. */
    std::optional<std::size_t> current;
    /** Optional row from which Extend computes an inclusive enabled-row range. */
    std::optional<std::size_t> anchor;
    /** Compare selected rows, current navigation row, and range anchor. */
    auto operator==(RowSelection const&) const -> bool = default;
};
/** Single, replace-range, toggle, or cursor-only selection. */
enum class SelectionAction {
    Replace, ///< Select only this row and reset the range anchor.
    Extend, ///< Select the inclusive range from the anchor, skipping disabled rows.
    Toggle, ///< Toggle this row and reset the range anchor.
    Move ///< Move the navigation cursor without changing selected rows or anchor.
};
/** Apply one selection action to valid row indices, skipping disabled rows; return
 * whether state changed. Invalid row indices throw.
 */
auto select_rows(RowSelection& state,
                 std::size_t row,
                 std::size_t count,
                 SelectionAction action = SelectionAction::Replace,
                 std::function<bool(std::size_t)> const& enabled = {}) -> bool;
/** Map navigation keys to signed row steps, using page for PageUp/PageDown; return
 * nullopt for unrelated keys or releases.
 */
auto row_navigation(KeyEvent const&, int page) -> std::optional<int>;
/** Find an enabled target after clamped navigation, or nullopt when no enabled row exists. */
auto row_target(std::optional<std::size_t> current,
                std::size_t count,
                int steps,
                std::function<bool(std::size_t)> const& enabled = {})
    -> std::optional<std::size_t>;
/** Visible content rows, half-open, after clipping. */
auto visible_rows(Canvas, int offset, std::size_t count) -> std::pair<int, int>;
/** Shared Unicode clipping/ellipsis painter; layout storage belongs to the caller. */
void paint_row_text(Canvas, std::string_view, Align, Style, TextLayout&);
/** Reject malformed UTF-8, controls, tabs, and line breaks in a printable single-line
 * row label.
 */
void validate_row_text(std::string_view);
}  // namespace ox

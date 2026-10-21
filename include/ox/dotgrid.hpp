/** @file
 * Braille graphics with individual dots and colors shared by each terminal cell.
 */
#pragma once
#include <cstdint>
#include <vector>
#include <ox/widget.hpp>

namespace ox {
/** Foreground for lit dots and background for unlit dots in one terminal cell. */
struct DotColors {
    /** Color of the lit dots; default_color uses the theme foreground. */
    Color foreground = default_color;
    /** Color between dots; default_color uses the theme background. */
    Color background = default_color;
    /** Compare both color channels. */
    auto operator==(DotColors const&) const -> bool = default;
};
/** An owned 2-by-4-dot-per-cell grid. Coordinates start at the top left.
 * Partial edge cells keep their unused dots off. Empty cells paint spaces.
 * Default colors resolve through the widget theme; margins are not painted.
 */
class DotGrid : public Widget {
   public:
    using Widget::handle;
    /** Allocate cleared dots; size is in dots, colors apply to every cell.
     * Negative dimensions, storage overflow, and invalid colors throw.
     */
    explicit DotGrid(Area size = {}, DotColors colors = {});
    /** Transfer identity and storage, leaving a reusable empty source. */
    DotGrid(DotGrid&&);
    /** Transfer identity and storage, leaving a reusable empty source. */
    auto operator=(DotGrid&&) -> DotGrid&;
    /** Source dimensions in dots. */
    auto dot_size() const -> Area { return size_; }
    /** Terminal extent: ceil(width / 2) by ceil(height / 4). */
    auto cell_size() const -> Area;
    /** Read a dot; coordinates outside dot_size() throw std::out_of_range. */
    auto dot(Point at) const -> bool;
    /** Set or clear one dot; return whether it changed and repaint only if changed. */
    auto set_dot(Point at, bool on = true) -> bool;
    /** Read colors at terminal-cell coordinates; invalid coordinates throw. */
    auto cell_colors(Point cell) const -> DotColors;
    /** Set colors shared by all eight dots. Validate before mutation; return whether
     * changed and repaint only if changed. Coordinates are in terminal cells.
     */
    auto set_cell_colors(Point cell, DotColors colors) -> bool;
    /** Clear all dots, preserving colors; return whether any dots changed. */
    auto clear() -> bool;
    /** Replace with cleared dots and uniform cell colors. Validate and allocate before
     * committing; request layout and repaint. Dimensions are in dots.
     */
    void reset(Area size, DotColors colors = {});
    /** Return cell_size(); validate the supplied available extent. */
    auto measure(Area available, esc::WidthPolicy = {}) const -> Area override;
    /** Paint visible cells only. Visible glyphs must occupy one terminal column;
     * incompatible width policies throw before drawing any cells.
     */
    auto paint(Canvas) -> Cursor override;
   private:
    struct Cell { std::uint8_t dots = 0; DotColors colors; };
    Area size_ = {};
    std::vector<Cell> cells_;
    auto index(Point cell) const -> std::size_t;
    void check_dot(Point at) const;
};
} // namespace ox

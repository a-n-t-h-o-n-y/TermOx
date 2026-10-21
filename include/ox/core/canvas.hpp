/** @file
 * Borrowed frame views with clipping and coordinate translation.
 */
#pragma once

#include <ox/core/frame.hpp>

namespace ox {
namespace detail { class SingleColumnPainter; }

/** A non-owning view with local coordinates and an inherited clip.
 * Views retain their geometry after frame resize, intersecting the current bounds.
 * The frame must outlive its canvases and stay at the same address.
 * @code{.cpp}
 * auto frame = ox::Frame{{40, 10}};
 * auto canvas = ox::Canvas{frame};
 * auto panel = canvas.subcanvas({2, 1}, {20, 4});
 * ox::fill(panel, U' ' | ox::bg(ox::RGB{0x182d24}));
 * ox::put(panel, {1, 1}, "Hello", {.foreground = ox::colors::white});
 * @endcode
 * Drawing uses panel-local coordinates and stays inside the inherited clip.
 */
class Canvas {
   public:
    /** Borrow the whole frame as a view; the frame must outlive the canvas and remain at
     * the same address.
     */
    explicit Canvas(Frame& frame);
    /** Borrow a positioned view, inheriting frame clipping; negative size and
     * overflowing coordinates are rejected.
     */
    Canvas(Frame& frame, Point at, Area size);

    /** Return the view's logical extent, independent of its visible clip. */
    [[nodiscard]] auto size() const -> Area { return size_; }
    /** Return the view's origin in frame coordinates. */
    [[nodiscard]] auto origin() const -> Point { return at_; }
    /** Visible rectangle in local coordinates. */
    [[nodiscard]] auto visible() const -> Rect;
    /** Return whether a local point is inside both geometry and the current inherited clip. */
    [[nodiscard]] auto contains(Point point) const -> bool;
    /** Translate local coordinates to the frame with checked arithmetic; the point need
     * not be visible.
     */
    [[nodiscard]] auto to_global(Point point) const -> Point;
    /** Borrow a visible local cell. @throws std::out_of_range The cell is clipped or
     * outside the view.
     */
    [[nodiscard]] auto operator[](Point point) const -> Cell const&;
    /** Create a borrowed local child view preserving parent clipping; its geometry may
     * extend outside the parent.
     */
    [[nodiscard]] auto subcanvas(Point at, Area size) const -> Canvas;
    /** Borrow the underlying frame's current glyph-width assumptions. */
    [[nodiscard]] auto width_policy() const -> esc::WidthPolicy
    { return frame_.width_policy(); }

    /** Place or restyle complete graphemes, never modifying outside the visible clip.
     */
    auto write(Point at, std::u32string_view grapheme, Style style = {}) const -> bool;
    /** Restyle the complete grapheme at a visible local cell; return false if its
     * footprint is clipped.
     */
    auto restyle(Point at, Style style) const -> bool;

   private:
    friend class detail::SingleColumnPainter;
    Frame& frame_;
    Point at_;
    Area size_;
    Rect clip_;

    Canvas(Frame& frame, Point at, Area size, Rect clip);
    auto global_clip() const -> Rect;
};
}  // namespace ox

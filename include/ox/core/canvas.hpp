#pragma once

#include <ox/core/frame.hpp>

namespace ox {

/** A non-owning view with local coordinates and an inherited clip.
 * Views retain their geometry after frame resize, intersecting the current bounds.
 * The frame must outlive its canvases and stay at the same address.
 */
class Canvas {
   public:
    explicit Canvas(Frame& frame);
    Canvas(Frame& frame, Point at, Area size);

    [[nodiscard]] auto size() const -> Area { return size_; }
    [[nodiscard]] auto origin() const -> Point { return at_; }
    /** Visible rectangle in local coordinates. */
    [[nodiscard]] auto visible() const -> Rect;
    [[nodiscard]] auto contains(Point point) const -> bool;
    [[nodiscard]] auto to_global(Point point) const -> Point;
    [[nodiscard]] auto operator[](Point point) const -> Cell const&;
    [[nodiscard]] auto subcanvas(Point at, Area size) const -> Canvas;
    [[nodiscard]] auto width_policy() const -> esc::WidthPolicy
    { return frame_.width_policy(); }

    /** Place or restyle complete graphemes, never modifying outside the visible clip.
     */
    auto write(Point at, std::u32string_view grapheme, Style style = {}) const -> bool;
    auto restyle(Point at, Style style) const -> bool;

   private:
    Frame& frame_;
    Point at_;
    Area size_;
    Rect clip_;

    Canvas(Frame& frame, Point at, Area size, Rect clip);
    auto global_clip() const -> Rect;
};
}  // namespace ox

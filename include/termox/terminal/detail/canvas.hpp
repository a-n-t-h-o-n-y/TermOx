#ifndef TERMOX_TERMINAL_DETAIL_CANVAS_HPP
#define TERMOX_TERMINAL_DETAIL_CANVAS_HPP
#include <iosfwd>
#include <memory>
#include <utility>
#include <vector>

#include <termox/painter/color.hpp>
#include <termox/painter/glyph.hpp>
#include <termox/widget/area.hpp>
#include <termox/widget/point.hpp>

namespace ox::detail {

/// A 2D field of Glyphs, useful as a screen buffer.
/** Used by Painter to write output to, which is eventually written to the
 *  actual terminal screen. */
class Canvas {
   private:
    using Buffer_t = std::vector<Glyph>;

   public:
    /// Type used to model differences between two Canvas objects.
    using Diff = std::vector<std::pair<ox::Point, ox::Glyph>>;

   public:
    /// Construct a new Canvas with Area of \p a.
    Canvas(ox::Area a);

   public:
    /// Return the current size of the Canvas.
    [[nodiscard]] auto area() const -> ox::Area;

    /// Return the Glyph at Point \p p.
    [[nodiscard]] auto at(ox::Point p) const -> ox::Glyph;

    /// Return the Glyph at Point \p p.
    [[nodiscard]] auto at(ox::Point p) -> ox::Glyph&;

   public:
    /// Resize the Canvas to the given Area \p a.
    /** Will throw out any Glyphs from the current Canvas that no longer fit. */
    void resize(ox::Area a);

   public:
    /// Return begin iterator to internal buffer.
    [[nodiscard]] auto begin() -> Buffer_t::iterator;

    /// Return begin iterator to internal buffer.
    [[nodiscard]] auto begin() const -> Buffer_t::const_iterator;

    /// Return end iterator to internal buffer.
    [[nodiscard]] auto end() -> Buffer_t::iterator;

    /// Return end iterator to internal buffer.
    [[nodiscard]] auto end() const -> Buffer_t::const_iterator;

    /// Sets all Glyphs to default construction.
    void reset();

   private:
    Buffer_t buffer_;
    ox::Area area_;

    std::unique_ptr<Canvas> resize_buffer_ = nullptr;

   private:
    // Does not swap resize_buffer_
    void swap(Canvas& x);
};

/// Merge \p next into \p current.
/** A Glyph with null(zero) symbol is considered an untouched cell. */
void merge(Canvas const& next, Canvas& current);

/// Merge \p next into \p current, producing a diff of the changes.
/** The diff is stored into \p diff_out, which is cleared at the start.
 *  diff_out is an out parameter for efficiency, to reduce allocations. A
 *  Glyph with null(zero) symbol is considered an untouched cell. */
void merge_and_diff(Canvas const& next,
                    Canvas& current,
                    Canvas::Diff& diff_out);

/// Generate a Canvas::Diff containing only the items that contain \p color.
/** Added to the diff if \p color can be found in either the Glyph's
 *  brush.foreground or brush.background members. The diff is written to \p
 *  diff_out, to reduce allocations. */
void generate_color_diff(Color color,
                         Canvas const& canvas,
                         Canvas::Diff& diff_out);

/// Writes the entire contents of \p canvas into \p diff_out.
/** Clears diff_out before writing. */
void generate_full_diff(Canvas const& canvas, Canvas::Diff& diff_out);

/// Debug function, print representation of the given Canvas::Diff.
auto print(Canvas::Diff const& diff, std::ostream& os) -> std::ostream&;

}  // namespace ox::detail
#endif  // TERMOX_TERMINAL_DETAIL_CANVAS_HPP

#ifndef TERMOX_TERMINAL_DETAIL_CANVAS_HPP
#define TERMOX_TERMINAL_DETAIL_CANVAS_HPP
#include <cassert>
#include <iterator>
#include <utility>
#include <vector>

#include <termox/painter/glyph.hpp>
#include <termox/widget/area.hpp>
#include <termox/widget/point.hpp>

namespace ox::detail {

/// Return the next Point from \p p, within Area \p a.
/** p.x is incremented until a.width, where it is zeroed and p.y is inc. */
[[nodiscard]] inline auto next(ox::Point p, ox::Area a) -> ox::Point
{
    return (p.x + 1) < a.width ? ox::Point{p.x + 1, p.y}
                               : ox::Point{0, p.y + 1};
}

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
    Canvas(ox::Area a) : buffer_(a.width * a.height, ox::Glyph{}), area_{a} {}

   public:
    /// Return the current size of the Canvas.
    [[nodiscard]] auto area() const -> ox::Area { return area_; }

    /// Return the Glyph at Point \p p.
    [[nodiscard]] auto at(ox::Point p) const -> ox::Glyph const&
    {
        auto const index = p.x + (p.y * area_.width);
        assert(index < (int)buffer_.size());
        return buffer_[index];
    }

    /// Return the Glyph at Point \p p.
    [[nodiscard]] auto at(ox::Point p) -> ox::Glyph&
    {
        return const_cast<ox::Glyph&>(static_cast<Canvas const&>(*this).at(p));
    }

   public:
    /// Resize the Canvas to the given Area \p a.
    /** Will throw out any Glyphs from the current Canvas that no longer fit. */
    void resize(ox::Area a)
    {
        auto new_canvas = Canvas{a};
        auto current    = ox::Point{0, 0};
        for (Glyph g : buffer_) {
            if (is_within(current, a))
                new_canvas.at(current) = g;
            current = next(current, area_);
        }
        this->operator=(std::move(new_canvas));
    }

   public:
    /// Return begin iterator to internal buffer.
    [[nodiscard]] auto begin() -> Buffer_t::iterator
    {
        return std::begin(buffer_);
    }

    /// Return begin iterator to internal buffer.
    [[nodiscard]] auto begin() const -> Buffer_t::const_iterator
    {
        return std::cbegin(buffer_);
    }

    /// Return end iterator to internal buffer.
    [[nodiscard]] auto end() -> Buffer_t::iterator { return std::end(buffer_); }

    /// Return end iterator to internal buffer.
    [[nodiscard]] auto end() const -> Buffer_t::const_iterator
    {
        return std::cend(buffer_);
    }

   private:
    Buffer_t buffer_;
    ox::Area area_;

   private:
    /// Return true if Point \p p is within the bounds of \p a.
    [[nodiscard]] static auto is_within(ox::Point p, ox::Area a) -> bool
    {
        return (p.x < a.width) && (p.y < a.height);
    }
};

/// Merge \p next into \p current, producing a diff of the changes.
/** The diff is stored into \p diff_out, which is cleared at the start.
 *  diff_out is an out parameter for efficiency, to reduce allocations. A
 *  Glyph with null(zero) symbol is considered an untouched cell. */
inline void merge_and_diff(Canvas const& next,
                           Canvas& current,
                           Canvas::Diff& diff_out)
{
    assert(next.area() == current.area());
    diff_out.clear();
    auto next_iter    = std::cbegin(next);
    auto current_iter = std::begin(current);
    auto point        = ox::Point{0, 0};
    for (; next_iter != std::cend(next); ++next_iter, ++current_iter) {
        if (next_iter->symbol != U'\0' && *next_iter != *current_iter) {
            diff_out.push_back({point, *next_iter});
            *current_iter = *next_iter;
        }
        point = ox::detail::next(point, next.area());
    }
}

/// Generate a Canvas::Diff containing only the items that contain \p color.
/** Added to the diff if \p color can be found in either the Glyph's
 *  brush.foreground or brush.background members. The diff is written to \p
 *  diff_out, to reduce allocations. */
inline void generate_color_diff(Color color,
                                Canvas const& canvas,
                                Canvas::Diff& diff_out)
{
    diff_out.clear();
    auto point = ox::Point{0, 0};
    for (Glyph g : canvas) {
        if (g.brush.foreground == color || g.brush.background == color)
            diff_out.push_back({point, g});
        point = next(point, canvas.area());
    }
}

}  // namespace ox::detail
#endif  // TERMOX_TERMINAL_DETAIL_CANVAS_HPP

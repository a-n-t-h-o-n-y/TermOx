#ifndef TERMOX_PAINTER_GLYPH_MATRIX_HPP
#define TERMOX_PAINTER_GLYPH_MATRIX_HPP
#include <cstddef>
#include <vector>

#include <termox/painter/glyph.hpp>
#include <termox/widget/area.hpp>
#include <termox/widget/point.hpp>

namespace ox {

/// Holds a matrix of Glyphs, provides simple access by indices.
class Glyph_matrix {
   public:
    /// Construct with a set width and height, or defaults to 0 for each.
    /** Glyphs default constructed(space char with no colors or traits). */
    explicit Glyph_matrix(Area area)
        : matrix_(area.height, std::vector<Glyph>(area.width, Glyph{U' '}))
    {}

   public:
    /// Resize the width and height of the matrix.
    /** New Glyphs will be default constructed, Glyphs no longer within the
     *  bounds of the matrix will be destructed. */
    void resize(Area area);

    /// Remove all Glyphs from the matrix and set width/height to 0.
    void clear() { matrix_.clear(); }

    /// Return the width of the matrix.
    auto width() const -> int
    {
        return matrix_.empty() ? 0 : matrix_.at(0).size();
    }

    /// Return the height of the matrix.
    auto height() const -> int { return matrix_.size(); }

    /// Glyph access operator. {0, 0} is top left. x grows south and y east.
    /** Provides no bounds checking. */
    auto operator()(Point p) -> Glyph& { return matrix_[p.y][p.x]; }

    /// Glyph access operator. {0, 0} is top left. x grows south and y east.
    /** Provides no bounds checking. */
    auto operator()(Point p) const -> Glyph { return matrix_[p.y][p.x]; }

    /// Glyph access operator. {0, 0} is top left. x grows south and y east.
    /** Has bounds checking and throws std::out_of_range if not within range. */
    auto at(Point p) -> Glyph& { return matrix_.at(p.y).at(p.x); }

    /// Glyph access operator. {0, 0} is top left. x grows south and y east.
    /** Has bounds checking and throws std::out_of_range if not within range. */
    auto at(Point p) const -> Glyph { return matrix_.at(p.y).at(p.x); }

   private:
    std::vector<std::vector<Glyph>> matrix_;
};

}  // namespace ox
#endif  // TERMOX_PAINTER_GLYPH_MATRIX_HPP

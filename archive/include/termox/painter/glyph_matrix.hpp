#ifndef TERMOX_PAINTER_GLYPH_MATRIX_HPP
#define TERMOX_PAINTER_GLYPH_MATRIX_HPP
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
    explicit Glyph_matrix(Area area);

   public:
    /// Resize the width and height of the matrix.
    /** New Glyphs will be default constructed, Glyphs no longer within the
     *  bounds of the matrix will be destructed. */
    void resize(Area area);

    /// Remove all Glyphs from the matrix and set width/height to 0.
    void clear();

    /// Return the width of the matrix.
    [[nodiscard]] auto width() const -> int;

    /// Return the height of the matrix.
    [[nodiscard]] auto height() const -> int;

    /// Glyph access operator. {0, 0} is top left. x grows south and y east.
    /** Provides no bounds checking. */
    [[nodiscard]] auto operator()(Point p) -> Glyph&;

    /// Glyph access operator. {0, 0} is top left. x grows south and y east.
    /** Provides no bounds checking. */
    [[nodiscard]] auto operator()(Point p) const -> Glyph;

    /// Glyph access operator. {0, 0} is top left. x grows south and y east.
    /** Has bounds checking and throws std::out_of_range if not within range. */
    [[nodiscard]] auto at(Point p) -> Glyph&;

    /// Glyph access operator. {0, 0} is top left. x grows south and y east.
    /** Has bounds checking and throws std::out_of_range if not within range. */
    [[nodiscard]] auto at(Point p) const -> Glyph;

   private:
    std::vector<std::vector<Glyph>> matrix_;
};

}  // namespace ox
#endif  // TERMOX_PAINTER_GLYPH_MATRIX_HPP

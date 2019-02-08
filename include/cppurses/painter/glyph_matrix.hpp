#ifndef CPPURSES_PAINTER_GLYPH_MATRIX_HPP
#define CPPURSES_PAINTER_GLYPH_MATRIX_HPP
#include <cstddef>
#include <vector>

#include <cppurses/painter/glyph.hpp>

namespace cppurses {

/// Holds a matrix of Glyphs, provides simple access by indices.
class Glyph_matrix {
   public:
    /// Construct with a set width and height, or defaults to 0 for each.
    /** Glyphs default constructed(space char with no colors or attributes). */
    explicit Glyph_matrix(std::size_t width = 0, std::size_t height = 0)
        : matrix_{height, std::vector<Glyph>(width, Glyph{L' '})} {}

    /// Resize the width and height of the matrix.
    /** New Glyphs will be default constructed, Glyphs no longer within the
     *  bounds of the matrix will be destructed. */
    void resize(std::size_t width, std::size_t height);

    /// Remove all Glyphs from the matrix and set width/height to 0.
    void clear() { matrix_.clear(); }

    /// Return the width of the matrix.
    std::size_t width() const {
        return matrix_.empty() ? 0 : matrix_.at(0).size();
    }

    /// Return the height of the matrix.
    std::size_t height() const { return matrix_.size(); }

    /// Glyph access operator. (0, 0) is top left. x grows south and y east.
    /** Provides no bounds checking. */
    Glyph& operator()(std::size_t x, std::size_t y) { return matrix_[y][x]; }

    /// Glyph access operator. (0, 0) is top left. x grows south and y east.
    /** Provides no bounds checking. */
    const Glyph& operator()(std::size_t x, std::size_t y) const {
        return matrix_[y][x];
    }

    /// Glyph access operator. (0, 0) is top left. x grows south and y east.
    /** Has bounds checking and throws std::out_of_range if not within range. */
    Glyph& at(std::size_t x, std::size_t y) { return matrix_.at(y).at(x); }

    /// Glyph access operator. (0, 0) is top left. x grows south and y east.
    /** Has bounds checking and throws std::out_of_range if not within range. */
    const Glyph& at(std::size_t x, std::size_t y) const {
        return matrix_.at(y).at(x);
    }

   private:
    std::vector<std::vector<Glyph>> matrix_;
};

}  // namespace cppurses
#endif  // CPPURSES_PAINTER_GLYPH_MATRIX_HPP

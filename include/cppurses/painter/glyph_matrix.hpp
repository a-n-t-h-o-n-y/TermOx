#ifndef PAINTER_GLYPH_MATRIX_HPP
#define PAINTER_GLYPH_MATRIX_HPP
#include <cppurses/painter/glyph.hpp>
#include <cppurses/widget/point.hpp>

#include <cstddef>
#include <vector>

namespace cppurses {

class Glyph_matrix {
   public:
    explicit Glyph_matrix(std::size_t width = 0, std::size_t height = 0);

    void resize(std::size_t width, std::size_t height);
    void clear();

    std::size_t width() const;
    std::size_t height() const;

    Glyph& operator()(std::size_t x, std::size_t y);
    const Glyph& operator()(std::size_t x, std::size_t y) const;

    Glyph& at(std::size_t x, std::size_t y);
    const Glyph& at(std::size_t x, std::size_t y) const;

   private:
    std::vector<std::vector<Glyph>> matrix_;
};

}  // namespace cppurses
#endif  // PAINTER_GLYPH_MATRIX_HPP

#ifndef PAINTER_GLYPH_MATRIX_HPP
#define PAINTER_GLYPH_MATRIX_HPP

#include "painter/glyph.hpp"
#include <cstddef>
#include <vector>

#include <fstream>

namespace cppurses {

class Glyph_matrix {
   public:
    explicit Glyph_matrix(std::size_t width = 0, std::size_t height = 0);

    void resize(std::size_t width, std::size_t height);

    std::size_t width() const;
    std::size_t height() const;

    Glyph& at(std::size_t x, std::size_t y);
    const Glyph& at(std::size_t x, std::size_t y) const;

   private:
    // Implementation for both const/non-const versions of at()
    template <typename T>
    static auto at_impl(T& t, std::size_t x, std::size_t y)
        -> decltype(t.at(x, y)) {
        return t.matrix_.at(y).at(x);
    }

    std::vector<std::vector<Glyph>> matrix_;
};

}  // namespace cppurses
#endif  // PAINTER_GLYPH_MATRIX_HPP

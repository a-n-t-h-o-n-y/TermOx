#include <cppurses/painter/glyph_matrix.hpp>
#include <cppurses/painter/glyph.hpp>
#include <cppurses/widget/coordinates.hpp>

#include <cstddef>
#include <vector>

namespace cppurses {
Glyph_matrix::Glyph_matrix(std::size_t width, std::size_t height)
    : matrix_{height, std::vector<Glyph>(width, Glyph(" "))} {}

void Glyph_matrix::resize(std::size_t width, std::size_t height) {
    matrix_.resize(height);
    matrix_.shrink_to_fit();
    for (auto& row : matrix_) {
        row.resize(width, Glyph(" "));
        row.shrink_to_fit();
    }
}

std::size_t Glyph_matrix::width() const {
    return matrix_.empty() ? 0 : matrix_.at(0).size();
}

std::size_t Glyph_matrix::height() const {
    return matrix_.size();
}

Glyph& Glyph_matrix::operator()(std::size_t x, std::size_t y) {
    return matrix_[y][x];
}

const Glyph& Glyph_matrix::operator()(std::size_t x, std::size_t y) const {
    return matrix_[y][x];
}

Glyph& Glyph_matrix::at(std::size_t x, std::size_t y) {
    return matrix_.at(y).at(x);
}

const Glyph& Glyph_matrix::at(std::size_t x, std::size_t y) const {
    return matrix_.at(y).at(x);
}

}  // namespace cppurses

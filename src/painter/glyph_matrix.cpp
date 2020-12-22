#include <termox/painter/glyph_matrix.hpp>

#include <cstddef>

#include <termox/painter/glyph.hpp>

namespace ox {

void Glyph_matrix::resize(std::size_t width, std::size_t height)
{
    matrix_.resize(height);
    matrix_.shrink_to_fit();
    for (auto& row : matrix_) {
        row.resize(width, L' ');
        row.shrink_to_fit();
    }
}

}  // namespace ox

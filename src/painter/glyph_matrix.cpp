#include <termox/painter/glyph_matrix.hpp>

#include <termox/painter/glyph.hpp>
#include <termox/widget/area.hpp>

namespace ox {

void Glyph_matrix::resize(Area area)
{
    matrix_.resize(area.height);
    matrix_.shrink_to_fit();
    for (auto& row : matrix_) {
        row.resize(area.width, U'\0');
        row.shrink_to_fit();
    }
}

}  // namespace ox

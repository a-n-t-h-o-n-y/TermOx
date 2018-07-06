#include <cppurses/widget/widgets/matrix_display.hpp>

#include <cstddef>

#include <cppurses/painter/painter.hpp>

namespace cppurses {

Matrix_display::Matrix_display(Glyph_matrix matrix_)
    : matrix{std::move(matrix_)} {}

Matrix_display::Matrix_display(std::size_t width, std::size_t height)
    : matrix{width, height} {}

bool Matrix_display::paint_event(Painter& p) {
    std::size_t w{matrix.width() > this->width() ? this->width()
                                                 : matrix.width()};
    std::size_t h{matrix.height() > this->height() ? this->height()
                                                   : matrix.height()};
    for (std::size_t x{0}; x < w; ++x) {
        for (std::size_t y{0}; y < h; ++y) {
            p.put(matrix(x, y), x, y);
        }
    }
    return Widget::paint_event(p);
}

}  // namespace cppurses

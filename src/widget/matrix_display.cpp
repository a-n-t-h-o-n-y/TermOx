#include <cppurses/widget/widgets/matrix_display.hpp>

#include <cstddef>

#include <cppurses/painter/painter.hpp>

namespace cppurses {

Matrix_display::Matrix_display(Glyph_matrix matrix_)
    : matrix{std::move(matrix_)} {
    this->set_name("Matrix_display");
}

Matrix_display::Matrix_display(std::size_t width, std::size_t height)
    : matrix{width, height} {
    this->set_name("Matrix_display");
}

bool Matrix_display::paint_event() {
    std::size_t w{matrix.width() > this->width() ? this->width()
                                                 : matrix.width()};
    std::size_t h{matrix.height() > this->height() ? this->height()
                                                   : matrix.height()};
    Painter p{this};
    for (std::size_t y{0}; y < h; ++y) {
        for (std::size_t x{0}; x < w; ++x) {
            p.put(matrix(x, y), x, y);
        }
    }
    return Widget::paint_event();
}

}  // namespace cppurses

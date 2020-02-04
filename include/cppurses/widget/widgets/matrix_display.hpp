#ifndef CPPURSES_WIDGET_WIDGETS_MATRIX_DISPLAY_HPP
#define CPPURSES_WIDGET_WIDGETS_MATRIX_DISPLAY_HPP
#include <cstddef>

#include <cppurses/painter/glyph_matrix.hpp>
#include <cppurses/painter/painter.hpp>
#include <cppurses/widget/widget.hpp>

namespace cppurses {

class Matrix_display : public Widget {
   public:
    Glyph_matrix matrix;

   public:
    explicit Matrix_display(Glyph_matrix matrix_ = Glyph_matrix{})
        : matrix{std::move(matrix_)}
    {}

    Matrix_display(std::size_t width, std::size_t height)
        : matrix{width, height}
    {}

   protected:
    auto paint_event() -> bool override
    {
        auto const width =
            matrix.width() > this->width() ? this->width() : matrix.width();
        auto const height =
            matrix.height() > this->height() ? this->height() : matrix.height();

        Painter p{*this};
        for (auto y = 0uL; y < height; ++y) {
            for (auto x = 0uL; x < width; ++x) {
                p.put(matrix(x, y), x, y);
            }
        }
        return Widget::paint_event();
    }
};

}  // namespace cppurses
#endif  // CPPURSES_WIDGET_WIDGETS_MATRIX_DISPLAY_HPP

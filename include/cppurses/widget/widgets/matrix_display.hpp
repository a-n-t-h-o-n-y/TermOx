#ifndef CPPURSES_WIDGET_WIDGETS_MATRIX_DISPLAY_HPP
#define CPPURSES_WIDGET_WIDGETS_MATRIX_DISPLAY_HPP
#include <cstddef>

#include <cppurses/painter/glyph_matrix.hpp>
#include <cppurses/widget/widget.hpp>

namespace cppurses {

class Matrix_display : public Widget {
   public:
    explicit Matrix_display(Glyph_matrix matrix_ = Glyph_matrix{});
    Matrix_display(std::size_t width, std::size_t height);

    Glyph_matrix matrix;

   protected:
    bool paint_event() override;
};

}  // namespace cppurses
#endif  // CPPURSES_WIDGET_WIDGETS_MATRIX_DISPLAY_HPP

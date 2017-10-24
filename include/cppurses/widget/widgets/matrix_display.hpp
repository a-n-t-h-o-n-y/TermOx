#ifndef CPPURSES_WIDGET_WIDGETS_MATRIX_DISPLAY_HPP
#define CPPURSES_WIDGET_WIDGETS_MATRIX_DISPLAY_HPP
#include <cppurses/painter/glyph_matrix.hpp>
#include <cppurses/widget/widget.hpp>

namespace cppurses {

class Matrix_display : public Widget {
   public:
    Matrix_display(Glyph_matrix matrix_ = Glyph_matrix{});
    Glyph_matrix matrix;

   protected:
    bool paint_event() override;
};

}  // namespace cppurses

#endif  // CPPURSES_WIDGET_WIDGETS_MATRIX_DISPLAY_HPP

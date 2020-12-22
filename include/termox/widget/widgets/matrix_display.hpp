#ifndef TERMOX_WIDGET_WIDGETS_MATRIX_DISPLAY_HPP
#define TERMOX_WIDGET_WIDGETS_MATRIX_DISPLAY_HPP
#include <cstddef>

#include <termox/painter/glyph_matrix.hpp>
#include <termox/painter/painter.hpp>
#include <termox/widget/widget.hpp>

namespace ox {

/// Displays a Glyph_matrix object
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

        auto p = Painter{*this};
        for (auto y = 0uL; y < height; ++y) {
            for (auto x = 0uL; x < width; ++x)
                p.put(matrix(x, y), x, y);
        }
        return Widget::paint_event();
    }
};

/// Helper function to create an instance.
template <typename... Args>
auto matrix_display(Args&&... args) -> std::unique_ptr<Matrix_display>
{
    return std::make_unique<Matrix_display>(std::forward<Args>(args)...);
}

}  // namespace ox
#endif  // TERMOX_WIDGET_WIDGETS_MATRIX_DISPLAY_HPP

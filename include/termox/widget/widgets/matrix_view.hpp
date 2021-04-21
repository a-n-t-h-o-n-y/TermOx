#ifndef TERMOX_WIDGET_WIDGETS_MATRIX_VIEW_HPP
#define TERMOX_WIDGET_WIDGETS_MATRIX_VIEW_HPP
#include <cstddef>

#include <termox/painter/glyph_matrix.hpp>
#include <termox/painter/painter.hpp>
#include <termox/widget/area.hpp>
#include <termox/widget/widget.hpp>

namespace ox {

/// Displays a Glyph_matrix object
class Matrix_view : public Widget {
   public:
    struct Parameters {
        Glyph_matrix matrix;
    };

   public:
    Glyph_matrix matrix;

   public:
    explicit Matrix_view(Glyph_matrix matrix_) : matrix{std::move(matrix_)} {}

    explicit Matrix_view(Parameters parameters)
        : Matrix_view{std::move(parameters.matrix)}
    {}

   protected:
    auto paint_event(Painter& p) -> bool override
    {
        auto const w =
            matrix.width() > this->width() ? this->width() : matrix.width();
        auto const h =
            matrix.height() > this->height() ? this->height() : matrix.height();

        for (auto y = 0; y < h; ++y) {
            for (auto x = 0; x < w; ++x)
                p.put(matrix({x, y}), {x, y});
        }
        return Widget::paint_event(p);
    }
};

/// Helper function to create a Matrix_view instance.
[[nodiscard]] inline auto matrix_view(Glyph_matrix matrix)
    -> std::unique_ptr<Matrix_view>
{
    return std::make_unique<Matrix_view>(std::move(matrix));
}

/// Helper function to create a Matrix_view instance.
[[nodiscard]] inline auto matrix_view(Matrix_view::Parameters parameters)
    -> std::unique_ptr<Matrix_view>
{
    return std::make_unique<Matrix_view>(std::move(parameters));
}

}  // namespace ox
#endif  // TERMOX_WIDGET_WIDGETS_MATRIX_VIEW_HPP

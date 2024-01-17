#include <termox/widget/widgets/matrix_view.hpp>

#include <memory>
#include <utility>

#include <termox/painter/glyph_matrix.hpp>
#include <termox/painter/painter.hpp>
#include <termox/widget/widget.hpp>

namespace ox {

Matrix_view::Matrix_view(Glyph_matrix matrix_) : matrix{std::move(matrix_)} {}

Matrix_view::Matrix_view(Parameters p) : Matrix_view{std::move(p.matrix)} {}

auto Matrix_view::paint_event(Painter& p) -> bool
{
    auto const w = (matrix.width() > this->area().width) ? this->area().width
                                                         : matrix.width();
    auto const h = (matrix.height() > this->area().height) ? this->area().height
                                                           : matrix.height();

    for (auto y = 0; y < h; ++y) {
        for (auto x = 0; x < w; ++x)
            p.put(matrix({x, y}), {x, y});
    }
    return Widget::paint_event(p);
}

auto matrix_view(Glyph_matrix matrix) -> std::unique_ptr<Matrix_view>
{
    return std::make_unique<Matrix_view>(std::move(matrix));
}

auto matrix_view(Matrix_view::Parameters p) -> std::unique_ptr<Matrix_view>
{
    return std::make_unique<Matrix_view>(std::move(p));
}

}  // namespace ox

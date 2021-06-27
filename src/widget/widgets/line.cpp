#include <termox/widget/widgets/line.hpp>

#include <memory>
#include <utility>

#include <termox/widget/layouts/horizontal.hpp>
#include <termox/widget/layouts/vertical.hpp>
#include <termox/widget/pipe.hpp>

namespace ox {

template <template <typename> typename Layout_t>
Line<Layout_t>::Line()
{
    if constexpr (is_vertical)
        *this | pipe::fixed_width(1) | pipe::wallpaper(U'│');
    else
        *this | pipe::fixed_height(1) | pipe::wallpaper(U'─');
}

template class Line<layout::Horizontal>;
template class Line<layout::Vertical>;

template <template <typename> typename Layout_t>
auto line() -> std::unique_ptr<Line<Layout_t>>
{
    return std::make_unique<Line<Layout_t>>();
}

template auto line() -> std::unique_ptr<HLine>;
template auto line() -> std::unique_ptr<VLine>;

auto vline() -> std::unique_ptr<VLine> { return std::make_unique<VLine>(); }

auto hline() -> std::unique_ptr<HLine> { return std::make_unique<HLine>(); }

}  // namespace ox

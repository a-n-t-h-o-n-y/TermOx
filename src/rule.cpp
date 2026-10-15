#include <algorithm>
#include <cmath>
#include <ox/rule.hpp>
#include <stdexcept>
namespace ox {
void paint_rule(Canvas c,
                double start,
                double length,
                Style active,
                Style track,
                bool vertical,
                bool ascii)
{
    if (!std::isfinite(start) || !std::isfinite(length) || length < 0 ||
        !std::isfinite(start + length))
        throw std::invalid_argument{"Invalid rule interval"};
    auto n = vertical ? c.size().height : c.size().width;
    auto clip = c.visible();
    if (!n || !clip.size.width || !clip.size.height ||
        (vertical ? clip.at.x != 0 : clip.at.y != 0))
        return;
    auto first = vertical ? clip.at.y : clip.at.x;
    auto last = first + (vertical ? clip.size.height : clip.size.width);
    auto cap = std::clamp((length - 1.) / 2, 0., .5);
    auto left = start + cap, right = start + length - cap;
    auto horizontal = std::u32string_view{U" ╶╺╴─╼╸╾━"};
    auto upright = std::u32string_view{U" ╷╻╵│╽╹╿┃"};
    for (int i = first; i < std::min(n, last); ++i) {
        auto half = [&](double position, bool endcap) {
            if (length > 0 && position >= left && position < right) return 2;
            return endcap && n > 1 ? 0 : 1;
        };
        auto a = half(i + .25, i == 0), b = half(i + .75, i == n - 1);
        auto selected = a == 2 || b == 2;
        auto glyph =
            ascii ? (selected ? (vertical ? U'#' : U'=') : (vertical ? U'|' : U'-'))
                  : (vertical ? upright : horizontal)[(std::size_t)(a * 3 + b)];
        c.write(vertical ? Point{0, i} : Point{i, 0}, std::u32string_view{&glyph, 1},
                selected ? active : track);
    }
}
}  // namespace ox

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
    auto left = std::round(start);
    auto right = left + std::round(length);
    for (int i = first; i < std::min(n, last); ++i) {
        auto selected = i >= left && i < right;
        auto glyph = vertical ? U'│' : U'─';
        if (ascii)
            glyph = vertical ? U'|' : U'-';
        else if (n > 1 && i == 0)
            glyph = vertical ? U'╷' : U'╶';
        else if (n > 1 && i == n - 1)
            glyph = vertical ? U'╵' : U'╴';
        c.write(vertical ? Point{0, i} : Point{i, 0}, std::u32string_view{&glyph, 1},
                selected ? active : track);
    }
}
}  // namespace ox

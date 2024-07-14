#include <ox/scrollbar.hpp>

#include <algorithm>
#include <cmath>

namespace {

/// Returns a float so precision glyph fitting can be done.
[[nodiscard]] auto bar_top_position(int position,
                                    int canvas_length,
                                    int scrollable_length,
                                    int bar_length) -> float
{
    if (scrollable_length == 0) {
        return 0.f;
    }
    return (float)(canvas_length - bar_length) *
           ((float)position / (float)scrollable_length);
}

[[nodiscard]] auto bar_length(int canvas_length, int scrollable_length) -> int
{
    if (scrollable_length == 0) {
        return 0;
    }
    double ratio = std::min((double)canvas_length / scrollable_length, 1.0);
    return (int)std::ceil(canvas_length * ratio);
}

}  // namespace

namespace ox {

ScrollBar::ScrollBar(int scrollable_length_, int init_position)
    : scrollable_length{scrollable_length_}, position{init_position}
{
    this->size_policy = SizePolicy::fixed(1);
}

void ScrollBar::increment_position(int amount)
{
    position += amount;
    position = std::clamp(0, position, std::max(scrollable_length - 1, 0));
}

void ScrollBar::paint(Canvas c)
{
    auto const bar_len = bar_length(c.size.height, scrollable_length);
    auto const bar_pos =
        bar_top_position(position, c.size.height, scrollable_length, bar_len);

    auto const edge = [](float y) -> Glyph {
        auto const i = (std::size_t)((y - std::floor(y)) * 8);
        return {U"█▇▆▅▄▃▂▁"[i]};
    }(bar_pos);

    auto at = Point{
        .x = 0,
        .y = (int)std::floor(bar_pos),
    };

    c[at] = edge;

    Painter{c}[{.x = 0, .y = (int)bar_pos + 1}]
        << Painter::VLine{.length = std::max(bar_len - 1, 0), .glyph = {U'█'}};

    at.y += bar_len;
    if (at.y < this->size.height) {
        c[at] = edge | Trait::Inverse;
    }
}

void ScrollBar::mouse_press(Mouse m)
{
    if (m.button == Mouse::Button::Left) {
        position = (int)((float)scrollable_length *
                         ((float)m.at.y / (float)std::max(this->size.height - 1, 0)));
        this->on_scroll(position);
    }
}

void ScrollBar::mouse_wheel(Mouse m)
{
    auto const amount = 1;
    if (m.button == Mouse::Button::ScrollDown) {
        this->increment_position(1 * amount);
        this->on_scroll(position);
    }
    else if (m.button == Mouse::Button::ScrollUp) {
        this->increment_position(-1 * amount);
        this->on_scroll(position);
    }
}

}  // namespace ox
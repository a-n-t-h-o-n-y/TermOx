#include <ox/scrollbar.hpp>

#include <algorithm>
#include <chrono>
#include <cmath>

#include <ox/put.hpp>

namespace {

[[nodiscard]]
constexpr auto ease_out_quint(float const x) -> float
{
    return 1.0f - std::pow(1.f - x, 4.f);
}

/// Returns a float so precision glyph fitting can be done.
[[nodiscard]]
auto bar_top_position(int position,
                      int canvas_length,
                      int scrollable_length,
                      int bar_length) -> float
{
    if (scrollable_length == 0) { return 0.f; }
    return (float)(canvas_length - bar_length) *
           ((float)position / (float)scrollable_length);
}

[[nodiscard]]
auto bar_length(int canvas_length, int scrollable_length) -> int
{
    if (scrollable_length == 0) { return 0; }
    double ratio = std::min((double)canvas_length / scrollable_length, 1.0);
    return (int)std::ceil(canvas_length * ratio);
}

}  // namespace

namespace ox {

ScrollBar::Options const ScrollBar::init{};

ScrollBar::ScrollBar(Options x)
    : Widget{FocusPolicy::None, SizePolicy::fixed(1)},
      scrollable_length{std::move(x.scrollable_length)},
      position{std::move(x.position)},
      brush{std::move(x.brush)},
      scroll_settle_time{x.scroll_settle_time}
{}

void ScrollBar::paint(Canvas c)
{
    auto const bar_len = bar_length(c.size.height, scrollable_length);
    auto const bar_pos =
        bar_top_position(position, c.size.height, scrollable_length, bar_len);

    auto const edge = [](float y) -> char32_t {
        auto const i = (std::size_t)((y - std::floor(y)) * 8);
        return U"█▇▆▅▄▃▂▁"[i];
    }(bar_pos);

    auto at = Point{
        .x = 0,
        .y = (int)std::floor(bar_pos),
    };

    fill(c, brush);

    c[at].symbol = edge;

    put(c, {.x = 0, .y = (int)bar_pos + 1},
        shape::VLine{
            .length = std::max(bar_len - 1, 0),
            .symbol = U'█',
        },
        brush.foreground);

    at.y += bar_len;
    if (at.y < this->size.height) { c[at] = edge | brush | Trait::Inverse; }
}

void ScrollBar::mouse_press(Mouse m)
{
    if (m.button == Mouse::Button::Left) {
        time_at_click_ = ClockType::now();
        position_at_click_ = position;
        target_position_ =
            (int)((float)(std::max(scrollable_length - 1, 0)) *
                  ((float)m.at.y / (float)std::max(this->size.height - 1, 0)));
        timer_.start();
    }
}

void ScrollBar::mouse_wheel(Mouse m)
{
    constexpr auto percent_scrolll = 0.15f;
    if (m.button == Mouse::Button::ScrollDown) {
        time_at_click_ = ClockType::now();
        position_at_click_ = position;
        target_position_ = std::min(
            (int)((float)position + (percent_scrolll * (float)scrollable_length)),
            std::max(scrollable_length - 1, 0));
        timer_.start();
    }
    else if (m.button == Mouse::Button::ScrollUp) {
        time_at_click_ = ClockType::now();
        position_at_click_ = position;
        target_position_ = std::max(
            (int)((float)position - (percent_scrolll * (float)scrollable_length)), 0);
        timer_.start();
    }
}

void ScrollBar::timer(int id)
{
    if (id != timer_.id()) { return; };

    if (position == target_position_) {
        timer_.stop();
        return;
    }

    auto const time_since_click = ClockType::now() - time_at_click_;
    auto t =
        (float)std::chrono::duration_cast<std::chrono::milliseconds>(time_since_click)
            .count() /
        (float)scroll_settle_time.count();

    t = std::clamp(t, 0.f, 1.f);

    auto const r = ease_out_quint(t);

    position = (int)std::round(r * (float)(target_position_ - position_at_click_) +
                               (float)position_at_click_);

    this->on_scroll(position);
}

void ScrollBar::increment_position(int amount)
{
    position += amount;
    position = std::clamp(0, position, std::max(scrollable_length - 1, 0));
}

}  // namespace ox
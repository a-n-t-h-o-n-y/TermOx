#include <ox/scrollbar.hpp>

#include <algorithm>
#include <cmath>
#include <utility>

#include <ox/put.hpp>
#include <ox/scroll.hpp>

namespace ox {
ScrollBar::Options const ScrollBar::init{};
ScrollBar::ScrollBar(Options x)
    : Widget{FocusPolicy::None, SizePolicy::fixed(1)},
      content_length{x.content_length},
      viewport_length{x.viewport_length},
      position{clamp_scroll(x.position, x.content_length, x.viewport_length)},
      vertical{x.vertical},
      style{x.style},
      scroll_settle_time{x.scroll_settle_time}
{}
void ScrollBar::set_range(int content, int viewport, int requested)
{
    auto const next = clamp_scroll(requested, content, viewport);
    auto const changed =
        content_length != content || viewport_length != viewport || position != next;
    if (changed) {
        if (timer_) { timer_->stop(); }
        target_ = next;
    }
    content_length = content;
    viewport_length = viewport;
    position = next;
    if (changed) { request_repaint(); }
}
auto ScrollBar::set_position(int requested) -> bool
{
    auto const next = clamp_scroll(requested, content_length, viewport_length);
    if (timer_) { timer_->stop(); }
    target_ = next;
    if (std::exchange(position, next) == next) { return false; }
    request_repaint();
    on_scroll(next);
    return true;
}
auto ScrollBar::paint(Canvas canvas) -> Cursor
{
    fill(canvas, style);
    auto const [offset, length] =
        scrollbar_thumb(position, content_length, viewport_length,
                        vertical ? canvas.size().height : canvas.size().width);
    auto const thumb =
        vertical ? canvas.subcanvas({0, offset}, {canvas.size().width, length})
                 : canvas.subcanvas({offset, 0}, {length, canvas.size().height});
    fill(thumb, U'█' | style);
    return {};
}
auto ScrollBar::move_to(int requested) -> EventResponse
{
    requested = clamp_scroll(requested, content_length, viewport_length);
    if (scroll_settle_time.count() <= 0) {
        auto const changed = set_position(requested);
        return {.handled = true, .repaint = changed};
    }
    if (requested == position) {
        if (timer_) { timer_->stop(); }
        target_ = position;
        return {.handled = true};
    }
    start_ = clamp_scroll(position, content_length, viewport_length);
    target_ = requested;
    started_ = std::chrono::steady_clock::now();
    if (!timer_) {
        timer_.emplace(*this, std::chrono::milliseconds{16},
                       [](ScrollBar& bar) { return bar.tick(); });
    }
    timer_->start();
    return {.handled = true};
}
auto ScrollBar::handle(MousePress const& event) -> EventResponse
{
    if (event.button != MouseButton::Left) { return {}; }
    return move_to(scrollbar_position(vertical ? event.at.y : event.at.x,
                                      content_length, viewport_length,
                                      vertical ? size().height : size().width));
}
auto ScrollBar::handle(MouseWheel const& event) -> EventResponse
{
    auto const delta = vertical ? event.dy : event.dx;
    if (delta == 0) { return {}; }
    return move_to(scroll_by(timer_ && timer_->is_running() ? target_ : position, delta,
                             content_length, viewport_length));
}
auto ScrollBar::tick() -> EventResponse
{
    auto const elapsed = std::chrono::steady_clock::now() - started_;
    auto const progress =
        scroll_settle_time.count() <= 0
            ? 1.
            : std::clamp(std::chrono::duration<double, std::milli>{elapsed}.count() /
                             (double)scroll_settle_time.count(),
                         0., 1.);
    auto const eased = 1. - std::pow(1. - progress, 4.);
    auto const next =
        clamp_scroll((int)std::llround(start_ + eased * ((double)target_ - start_)),
                     content_length, viewport_length);
    if (progress == 1. && timer_) { timer_->stop(); }
    if (std::exchange(position, next) == next) { return {.handled = true}; }
    request_repaint();
    on_scroll(next);
    return {.handled = true, .repaint = true};
}
}  // namespace ox

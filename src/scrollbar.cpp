#include <ox/scrollbar.hpp>

#include <algorithm>
#include <cmath>
#include <cstdint>
#include <limits>
#include <utility>

#include <ox/put.hpp>
#include <ox/scroll.hpp>

namespace ox {
ScrollBar::Options const ScrollBar::init{};
ScrollBar::ScrollBar(Options x)
    : Widget{FocusPolicy::None, x.vertical ? SizePolicy::fixed(1) : SizePolicy::flex(),
             x.vertical ? SizePolicy::flex() : SizePolicy::fixed(1)},
      vertical{x.vertical},
      style{x.style},
      scroll_settle_time{x.scroll_settle_time},
      content_length_{x.content_length},
      viewport_length_{x.viewport_length},
      position_{clamp_scroll(x.position, x.content_length, x.viewport_length)}
{
    movement_.reset(position_);
    movement_.on_frame([lifetime = track(*this)](double value) {
        auto* self = lifetime.try_get();
        if (!self) return EventResponse{};
        if (self->scroll_settle_time.count() <= 0) {
            value = self->movement_.target();
            self->movement_.reset(value);
        }
        auto next = clamp_scroll((int)std::llround(value), self->content_length_,
                                 self->viewport_length_);
        auto changed = std::exchange(self->position_, next) != next;
        if (changed) {
            self->request_repaint();
            self->on_scroll(next);
        }
        return EventResponse{.repaint = changed};
    });
}
void ScrollBar::set_range(int content, int viewport, int requested)
{
    auto const next = clamp_scroll(requested, content, viewport);
    auto const changed =
        content_length_ != content || viewport_length_ != viewport || position_ != next;
    if (changed) { movement_.reset(next); }
    content_length_ = content;
    viewport_length_ = viewport;
    position_ = next;
    if (changed) { request_repaint(); }
}
ScrollBar::ScrollBar(ScrollBar&& other)
    : Widget{std::move(other)},
      vertical{std::move(other.vertical)},
      style{std::move(other.style)},
      scroll_settle_time{std::move(other.scroll_settle_time)},
      on_scroll{std::move(other.on_scroll)},
      content_length_{std::move(other.content_length_)},
      viewport_length_{std::move(other.viewport_length_)},
      position_{std::move(other.position_)},
      drag_offset_{std::move(other.drag_offset_)},
      movement_{std::move(other.movement_)}
{
    other.drag_offset_.reset();
}
auto ScrollBar::operator=(ScrollBar&& other) -> ScrollBar&
{
    if (this == &other) { return *this; }
    Widget::operator=(std::move(other));
    vertical = std::move(other.vertical);
    style = std::move(other.style);
    scroll_settle_time = std::move(other.scroll_settle_time);
    on_scroll = std::move(other.on_scroll);
    content_length_ = std::move(other.content_length_);
    viewport_length_ = std::move(other.viewport_length_);
    position_ = std::move(other.position_);
    drag_offset_ = std::move(other.drag_offset_);
    movement_ = std::move(other.movement_);
    other.drag_offset_.reset();
    return *this;
}
auto ScrollBar::set_position(int requested) -> bool
{
    auto const next = clamp_scroll(requested, content_length_, viewport_length_);
    movement_.reset(next);
    if (std::exchange(position_, next) == next) { return false; }
    request_repaint();
    on_scroll(next);
    return true;
}
auto ScrollBar::paint(Canvas canvas) -> Cursor
{
    auto thumb_style = appearance(style, Surface::Accent);
    auto track_style = appearance({}, Surface::Border);
    fill(canvas, theme().styled ? track_style : style);
    auto const [offset, length] =
        scrollbar_thumb(position_, content_length_, viewport_length_,
                        vertical ? canvas.size().height : canvas.size().width);
    auto const thumb =
        vertical ? canvas.subcanvas({0, offset}, {canvas.size().width, length})
                 : canvas.subcanvas({offset, 0}, {length, canvas.size().height});
    auto glyph = !theme().styled ? U'█' : (theme().ascii ? U'#' : (vertical ? U'┃' : U'━'));
    fill(thumb, glyph | thumb_style);
    return {};
}
auto ScrollBar::move_to(int requested) -> EventResponse
{
    requested = clamp_scroll(requested, content_length_, viewport_length_);
    if (scroll_settle_time.count() <= 0 || !theme().motion.enabled)
        return {.handled = true, .repaint = set_position(requested)};
    movement_.to(requested, {scroll_settle_time, Ease::OutCubic});
    if (!movement_.active())
        return {.handled = true, .repaint = set_position(requested)};
    return {.handled = true};
}
auto ScrollBar::handle(MousePress const& event) -> EventResponse
{
    if (!is_enabled() || event.button != MouseButton::Left) { return {}; }
    auto const track = vertical ? size().height : size().width;
    auto const cell = vertical ? event.at.y : event.at.x;
    auto const [offset, length] =
        scrollbar_thumb(position_, content_length_, viewport_length_, track);
    if (cell >= offset && cell < offset + length) {
        drag_offset_ = cell - offset;
        capture_pointer();
        return {.handled = true};
    }
    return move_to(scrollbar_position(cell, content_length_, viewport_length_, track));
}
auto ScrollBar::handle(MouseMove const& event) -> EventResponse
{
    if (!drag_offset_ ||
        (event.held_button && event.held_button != MouseButton::Left)) {
        return {};
    }
    auto const track = vertical ? size().height : size().width;
    auto const [offset, length] =
        scrollbar_thumb(position_, content_length_, viewport_length_, track);
    (void)offset;
    auto const cell =
        (std::int64_t)(vertical ? event.at.y : event.at.x) - *drag_offset_ + length / 2;
    auto const next = scrollbar_position(
        (int)std::clamp(cell, (std::int64_t)std::numeric_limits<int>::min(),
                        (std::int64_t)std::numeric_limits<int>::max()),
        content_length_, viewport_length_, track);
    auto const changed = set_position(next);
    return {.handled = true, .repaint = changed};
}
auto ScrollBar::handle(MouseRelease const& event) -> EventResponse
{
    if (event.button && event.button != MouseButton::Left) { return {}; }
    auto const dragging = drag_offset_.has_value();
    drag_offset_.reset();
    release_pointer();
    return {.handled = dragging};
}
auto ScrollBar::pointer_cancel() -> EventResponse
{
    drag_offset_.reset();
    release_pointer();
    return {};
}
auto ScrollBar::handle(MouseWheel const& event) -> EventResponse
{
    auto const delta = vertical ? event.dy : event.dx;
    if (delta == 0) { return {}; }
    return move_to(
        scroll_by(movement_.active() ? (int)std::lround(movement_.target()) : position_,
                  delta, content_length_, viewport_length_));
}
auto ScrollBar::tick() -> EventResponse { return movement_.advance(); }
}  // namespace ox

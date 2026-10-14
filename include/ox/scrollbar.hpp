#pragma once

#include <chrono>
#include <optional>

#include <ox/timer.hpp>
#include <ox/widget.hpp>

namespace ox {

/** Scroll positions and lengths use the same units as the controlled content. */
class ScrollBar : public Widget {
   public:
    using Widget::handle;
    struct Options {
        int content_length = 0;
        int viewport_length = 0;
        int position = 0;
        bool vertical = true;
        Style style = {};
        std::chrono::milliseconds scroll_settle_time = std::chrono::milliseconds{0};
    } static const init;

    auto content_length() const -> int { return content_length_; }
    auto viewport_length() const -> int { return viewport_length_; }
    auto position() const -> int { return position_; }
    bool vertical;
    Style style;
    std::chrono::milliseconds scroll_settle_time;
    sl::Signal<void(int)> on_scroll;

    ScrollBar(Options x = init);
    ScrollBar(ScrollBar&& other);
    auto operator=(ScrollBar&& other) -> ScrollBar&;
    /** Synchronize content without emitting on_scroll; cancel pending animation. */
    void set_range(int content, int viewport, int position);
    /** Explicit position changes notify only on change. */
    auto set_position(int position) -> bool;
    auto measure(Area available, esc::WidthPolicy = {}) const -> Area override
    {
        validate(available);
        return {1, 1};
    }
    auto handle(MouseMove const&) -> EventResponse override;
    auto handle(MouseRelease const&) -> EventResponse override;
    auto pointer_cancel() -> EventResponse override;
    auto paint(Canvas) -> Cursor override;
    auto handle(MousePress const&) -> EventResponse override;
    auto handle(MouseWheel const&) -> EventResponse override;
    auto tick() -> EventResponse;

   private:
    int content_length_;
    int viewport_length_;
    int position_;
    std::optional<int> drag_offset_;
    std::optional<Timer> timer_;
    std::chrono::steady_clock::time_point started_;
    int start_ = 0;
    int target_ = 0;
    auto move_to(int position) -> EventResponse;
};
}  // namespace ox

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

    int content_length;
    int viewport_length;
    int position;
    bool vertical;
    Style style;
    std::chrono::milliseconds scroll_settle_time;
    sl::Signal<void(int)> on_scroll;

    ScrollBar(Options x = init);
    /** Synchronize content without emitting on_scroll; cancel pending animation. */
    void set_range(int content, int viewport, int position);
    /** Explicit position changes notify only on change. */
    auto set_position(int position) -> bool;
    auto paint(Canvas) -> Cursor override;
    auto handle(MousePress const&) -> EventResponse override;
    auto handle(MouseWheel const&) -> EventResponse override;
    auto tick() -> EventResponse;

   private:
    std::optional<Timer> timer_;
    std::chrono::steady_clock::time_point started_;
    int start_ = 0;
    int target_ = 0;
    auto move_to(int position) -> EventResponse;
};
}  // namespace ox

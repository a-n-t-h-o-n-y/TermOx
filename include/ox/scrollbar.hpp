#pragma once

#include <chrono>

#include <ox/core/core.hpp>
#include <ox/timer.hpp>
#include <ox/widget.hpp>

namespace ox {

class ScrollBar : public Widget {
   public:
    struct Options {
        int scrollable_length = 0;
        int position = 0;
        int item_visual_length = 1;
        Brush brush = {};
        std::chrono::milliseconds scroll_settle_time = std::chrono::milliseconds{500};
    } static const init;

   public:
    int scrollable_length;
    int position;  // [0, scrollable_length)
    int item_visual_length;
    Brush brush;
    std::chrono::milliseconds scroll_settle_time;

    /// Emitted on any change to position, and passes position as the parameter.
    sl::Signal<void(int)> on_scroll;

    ScrollBar(Options x = init);

   public:
    void paint(Canvas c) override;

    void mouse_press(Mouse m) override;

    void mouse_wheel(Mouse m) override;

    void timer() override;

   private:
    void increment_position(int amount = 1);

   private:
    using ClockType = std::chrono::steady_clock;
    Timer timer_{*this, std::chrono::milliseconds{16}};
    ClockType::time_point time_at_click_;
    int position_at_click_ = 0;
    int target_position_ = 0;
};

}  // namespace ox
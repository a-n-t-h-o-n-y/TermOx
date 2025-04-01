#pragma once

#include <ox/core/core.hpp>
#include <ox/widget.hpp>

namespace ox {

class ScrollBar : public Widget {
   public:
    struct Options {
        int scrollable_length = 0;
        int position = 0;
        Brush brush = {};
    } static const init;

   public:
    int scrollable_length;
    int position;  // [0, scrollable_length)
    Brush brush;

    /// Emitted on any change to position, and passes position as the parameter.
    sl::Signal<void(int)> on_scroll;

    ScrollBar(Options x = init);

   public:
    void increment_position(int amount = 1);

   public:
    void paint(Canvas c) override;

    void mouse_press(Mouse m) override;

    void mouse_wheel(Mouse m) override;
};

}  // namespace ox
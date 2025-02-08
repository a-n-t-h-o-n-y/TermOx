#pragma once

#include <ox/core/core.hpp>
#include <ox/widget.hpp>

namespace ox {

class ScrollBar : public Widget {
   public:
    /**
     * Emitted on any change to position, and passes position as the parameter.
     */
    sl::Signal<void(int)> on_scroll;

   public:
    int scrollable_length;
    int position;
    Brush brush;

   public:
    struct Options {
        int scrollable_length = 0;
        int position = 0;
        Brush brush = {};
    } static const init;

    ScrollBar(Options x = init);

   public:
    void increment_position(int amount = 1);

   public:
    void paint(Canvas c) override;

    void mouse_press(Mouse m) override;

    void mouse_wheel(Mouse m) override;
};

}  // namespace ox
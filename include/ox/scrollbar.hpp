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

   public:
    ScrollBar(int scrollable_length_ = 0, int init_position = 0);

   public:
    void increment_position(int amount = 1);

   public:
    void paint(Canvas c) override;

    void mouse_press(Mouse m) override;

    void mouse_wheel(Mouse m) override;
};

}  // namespace ox
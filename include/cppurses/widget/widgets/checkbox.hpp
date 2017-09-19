#ifndef WIDGET_WIDGETS_CHECKBOX_HPP
#define WIDGET_WIDGETS_CHECKBOX_HPP

#include "painter/glyph.hpp"
#include "painter/glyph_string.hpp"
#include "widget/widget.hpp"
#include <signals/signals.hpp>

namespace cppurses {
class Mouse_event;
class Paint_event;

class Checkbox : public Widget {
   public:
    Checkbox(Glyph_string title = "", int dist = 3);

    // Slots
    sig::Slot<void()> toggle;
    sig::Slot<void()> check;
    sig::Slot<void()> uncheck;

    // Signals
    sig::Signal<void()> checked;
    sig::Signal<void()> unchecked;
    sig::Signal<void()> toggled;

   protected:
    bool paint_event() override;
    bool mouse_press_event(Mouse_button button,
                           std::size_t global_x,
                           std::size_t global_y,
                           std::size_t local_x,
                           std::size_t local_y,
                           std::uint8_t device_id) override;

    void toggle_();

    Glyph empty_box_{"☐"};
    Glyph checked_box_{"☒"};
    bool checked_{false};
    Glyph_string title_;

   private:
    void initialize();
    int dist_;
};

}  // namespace cppurses
#endif  // WIDGET_WIDGETS_CHECKBOX_HPP

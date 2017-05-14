#ifndef WIDGET_MODULE_WIDGETS_CHECKBOX_HPP
#define WIDGET_MODULE_WIDGETS_CHECKBOX_HPP

#include "painter_module/glyph.hpp"
#include "widget_module/widget.hpp"
#include <aml/signals/signals.hpp>

namespace twf {
class Mouse_event;
class Paint_event;

class Checkbox : public Widget {
   public:
    Checkbox();

    // Signals
    sig::Signal<void()> checked;
    sig::Signal<void()> unchecked;
    sig::Signal<void()> toggled;

   protected:
    bool paint_event(const Paint_event& event) override;
    bool mouse_press_event(const Mouse_event& event) override;

    void toggle();

    Glyph empty_box_{"☐"};
    Glyph checked_box_{"☒"};
    bool checked_{false};
};

}  // namespace twf
#endif  // WIDGET_MODULE_WIDGETS_CHECKBOX_HPP

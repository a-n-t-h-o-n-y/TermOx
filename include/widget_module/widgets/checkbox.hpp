#ifndef CHECKBOX_HPP
#define CHECKBOX_HPP

#include <aml/signals/signals.hpp>

#include "painter_module/glyph.hpp"
#include "widget_module/widget.hpp"
#include "system_module/events/mouse_event.hpp"
#include "system_module/events/paint_event.hpp"
#include "painter_module/attribute.hpp"

namespace twf {

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

    Glyph empty_ = Glyph(U'☐');
    Glyph check_ = Glyph(U'☒');
    bool checked_ = false;
};

}  // namespace twf
#endif  // CHECKBOX_HPP

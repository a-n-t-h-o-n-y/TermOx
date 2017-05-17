#ifndef WIDGET_MODULE_WIDGETS_TEXTBOX_HPP
#define WIDGET_MODULE_WIDGETS_TEXTBOX_HPP

#include "painter_module/glyph_string.hpp"
#include "widget_module/focus_policy.hpp"
#include "widget_module/widgets/textbox_core.hpp"

namespace twf {
class Key_event;
class Mouse_event;

class Textbox : public Textbox_core {
   public:
    explicit Textbox(const Glyph_string& contents = "")
        : Textbox_core{contents} {
        this->set_focus_policy(Focus_policy::Strong);
        this->enable_cursor();
    }

   protected:
    bool key_press_event(const Key_event& event) override;
    bool mouse_press_event(const Mouse_event& event) override;
};

}  // namespace twf
#endif  // WIDGET_MODULE_WIDGETS_TEXTBOX_HPP

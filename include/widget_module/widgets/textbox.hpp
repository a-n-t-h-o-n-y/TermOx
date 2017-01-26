#ifndef TEXTBOX_HPP
#define TEXTBOX_HPP

#include "../widget.hpp"
#include "../../system_module/events/paint_event.hpp"
#include "../../system_module/events/key_event.hpp"
#include "../../painter_module/glyph_string.hpp"

namespace twf {

class Textbox : public Widget {
   public:
    Textbox() {
        this->set_focus_policy(Widget::Focus_policy::StrongFocus);
        this->set_cursor(true);
    }

    bool paint_event(const Paint_event& event) override;

    bool key_press_event(const Key_event& event) override;

   private:
    Glyph_string contents_;
};

}  // namespace twf
#endif  // TEXTBOX_HPP

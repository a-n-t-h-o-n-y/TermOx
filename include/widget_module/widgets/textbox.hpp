#ifndef TEXTBOX_HPP
#define TEXTBOX_HPP

#include "textbox_core.hpp"
#include "../../system_module/events/paint_event.hpp"
#include "../../system_module/events/key_event.hpp"
#include "../../painter_module/glyph_string.hpp"
#include "painter_module/glyph_string.hpp"

#include <cstddef>
#include <array>

namespace twf {

class Textbox : public Textbox_core {
   public:
    Textbox(const Glyph_string& contents = "") : Textbox_core{contents} {
        this->set_focus_policy(Widget::Focus_policy::StrongFocus);
        this->set_cursor(true);
    }

    Glyph_string text() const { return contents_; }

   protected:
    bool key_press_event(const Key_event& event) override;
    bool mouse_press_event(const Mouse_event& event) override;
};

}  // namespace twf
#endif  // TEXTBOX_HPP

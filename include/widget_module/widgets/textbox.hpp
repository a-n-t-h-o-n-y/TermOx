#ifndef TEXTBOX_HPP
#define TEXTBOX_HPP

#include "../widget.hpp"
#include "../../system_module/events/paint_event.hpp"
#include "../../system_module/events/key_event.hpp"
#include "../../painter_module/glyph_string.hpp"

#include <cstddef>
#include <array>

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

    // rename these 2
    std::array<std::size_t, 2> cursor_position_2d(
        std::size_t index);  // change to Coordinate.
    std::size_t index_from_position(std::size_t x, std::size_t y);
    std::size_t cursor_index_ = 0;

    void cursor_up();
    void cursor_down();
};

}  // namespace twf
#endif  // TEXTBOX_HPP

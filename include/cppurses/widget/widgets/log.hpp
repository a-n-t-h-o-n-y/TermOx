#ifndef WIDGET_WIDGETS_LOG_HPP
#define WIDGET_WIDGETS_LOG_HPP
#include "widget/widgets/textbox.hpp"
#include "painter/glyph_string.hpp"
#include "system/key.hpp"

#include <signals/slot.hpp>

namespace cppurses {

class Log : public Textbox {
   public:
    void post_message(Glyph_string message);

   protected:
    bool key_press_event(Key key, char symbol) override;

    using Text_display::set_text;
    using Text_display::insert;
    using Text_display::append;
    using Text_display::erase;
    using Text_display::pop_back;
};

namespace slot {

sig::Slot<void(Glyph_string)> post_message(Log& log);
sig::Slot<void()> post_message(Log& log, const Glyph_string& message);

}  // namespace slot

}  // namespace cppurses

#endif  // WIDGET_WIDGETS_LOG_HPP

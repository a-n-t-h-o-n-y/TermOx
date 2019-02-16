#ifndef CPPURSES_WIDGET_WIDGETS_LOG_HPP
#define CPPURSES_WIDGET_WIDGETS_LOG_HPP
#include <signals/slot.hpp>

#include <cppurses/system/events/key.hpp>
#include <cppurses/widget/widgets/text_display.hpp>
#include <cppurses/widget/widgets/textbox.hpp>

namespace cppurses {
class Glyph_string;

class Log : public Textbox {
   public:
    void post_message(Glyph_string message);

   protected:
    bool key_press_event(const Key::State& keyboard) override;

    using Text_display::append;
    using Text_display::erase;
    using Text_display::insert;
    using Text_display::pop_back;
    using Text_display::set_contents;
};

namespace slot {

sig::Slot<void(Glyph_string)> post_message(Log& log);
sig::Slot<void()> post_message(Log& log, const Glyph_string& message);

}  // namespace slot
}  // namespace cppurses
#endif  // CPPURSES_WIDGET_WIDGETS_LOG_HPP

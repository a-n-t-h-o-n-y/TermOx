#ifndef CPPURSES_WIDGET_WIDGETS_LINE_EDIT_HPP
#define CPPURSES_WIDGET_WIDGETS_LINE_EDIT_HPP
#include <cstdint>
#include <functional>
#include <string>
#include <utility>

#include <signals/signals.hpp>

#include <cppurses/painter/color.hpp>
#include <cppurses/painter/glyph_string.hpp>
#include <cppurses/system/keyboard_data.hpp>
#include <cppurses/system/mouse_data.hpp>
#include <cppurses/widget/widgets/textbox.hpp>

namespace cppurses {
struct Point;

class Line_edit : public Textbox {
   public:
    explicit Line_edit(Glyph_string initial_text = "");

    void set_validator(std::function<bool(char)> validator);

    void clear_on_enter(bool enable = true);
    void invisible_input(bool enabled = true);
    void underline(bool enabled = true);
    void set_ghost_color(Color c);

    // Signals
    sig::Signal<void(std::string)> editing_finished;

   protected:
    bool key_press_event(const Keyboard_data& keyboard) override;
    bool mouse_press_event(const Mouse_data& mouse) override;
    bool focus_in_event() override;

   private:
    bool clear_on_enter_{false};
    bool on_initial_{true};
    std::function<bool(char)> validator_{[](char) { return true; }};
};

}  // namespace cppurses
#endif  // CPPURSES_WIDGET_WIDGETS_LINE_EDIT_HPP

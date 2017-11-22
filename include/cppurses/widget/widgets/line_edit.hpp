#ifndef WIDGET_WIDGETS_LINE_EDIT_HPP
#define WIDGET_WIDGETS_LINE_EDIT_HPP
#include <cppurses/painter/color.hpp>
#include <cppurses/painter/glyph_string.hpp>
#include <cppurses/system/key.hpp>
#include <cppurses/system/mouse_button.hpp>
#include <cppurses/widget/widgets/textbox.hpp>

#include <signals/signals.hpp>

#include <cstddef>
#include <cstdint>
#include <string>

namespace cppurses {

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
    bool key_press_event(Key key, char symbol) override;
    bool mouse_press_event(Mouse_button button,
                           Point global,
                           Point local,
                           std::uint8_t device_id) override;
    bool focus_in_event() override;

   private:
    bool clear_on_enter_{false};
    bool on_initial_{true};
    std::function<bool(char)> validator_{[](char) { return true; }};
};

}  // namespace cppurses

#endif  // WIDGET_WIDGETS_LINE_EDIT_HPP

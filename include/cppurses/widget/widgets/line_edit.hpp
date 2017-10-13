#ifndef WIDGET_WIDGETS_LINE_EDIT_HPP
#define WIDGET_WIDGETS_LINE_EDIT_HPP
#include "widget/widgets/textbox.hpp"
#include "painter/glyph_string.hpp"
#include "painter/color.hpp"
#include "system/mouse_button.hpp"
#include "system/key.hpp"

#include <signals/signals.hpp>

#include <cstddef>
#include <cstdint>
#include <string>

namespace cppurses {

class Line_edit : public Textbox {
   public:
    Line_edit(Glyph_string initial_text = "");

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
                           std::size_t global_x,
                           std::size_t global_y,
                           std::size_t local_x,
                           std::size_t local_y,
                           std::uint8_t device_id) override;
    bool focus_in_event() override;

   private:
    bool clear_on_enter_{false};
    bool on_initial_{true};
    std::function<bool(char)> validator_{[](char) { return true; }};
};

}  // namespace cppurses

#endif  // WIDGET_WIDGETS_LINE_EDIT_HPP

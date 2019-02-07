#ifndef CPPURSES_WIDGET_WIDGETS_TEXTBOX_HPP
#define CPPURSES_WIDGET_WIDGETS_TEXTBOX_HPP
#include <cstddef>
#include <cstdint>

#include <cppurses/painter/glyph_string.hpp>
#include <cppurses/system/keyboard_data.hpp>
#include <cppurses/system/mouse_data.hpp>
#include <cppurses/widget/widgets/textbox_base.hpp>

namespace cppurses {

/// Standard text input and display Widget.
/** Cont */
class Textbox : public Textbox_base {
   public:
    explicit Textbox(Glyph_string contents = "")
        : Textbox_base{std::move(contents)} {
        this->focus_policy = Focus_policy::Strong;
    }

    void enable_scrollwheel(bool enable = true) { scroll_wheel_ = enable; }
    void disable_scrollwheel(bool disable = true) { scroll_wheel_ = !disable; }
    void toggle_scrollwheel() { scroll_wheel_ = !scroll_wheel_; }
    bool does_scrollwheel() const { return scroll_wheel_; }
    void set_wheel_speed(std::size_t lines) {
        this->set_wheel_speed_up(lines);
        this->set_wheel_speed_down(lines);
    }
    void set_wheel_speed_up(std::size_t lines) { scroll_speed_up_ = lines; }
    void set_wheel_speed_down(std::size_t lines) { scroll_speed_down_ = lines; }
    void disable_input() { takes_input_ = false; }
    void enable_input() { takes_input_ = true; }

   protected:
    bool key_press_event(const Keyboard_data& keyboard) override;
    bool mouse_press_event(const Mouse_data& mouse) override;

   private:
    bool scroll_wheel_{true};
    bool takes_input_{true};
    std::size_t scroll_speed_up_{1};
    std::size_t scroll_speed_down_{1};
};

}  // namespace cppurses
#endif  // CPPURSES_WIDGET_WIDGETS_TEXTBOX_HPP

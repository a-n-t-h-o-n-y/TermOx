#ifndef WIDGET_WIDGETS_TEXTBOX_HPP
#define WIDGET_WIDGETS_TEXTBOX_HPP
#include <cstddef>
#include <cstdint>

#include <cppurses/painter/glyph_string.hpp>
#include <cppurses/system/keyboard_data.hpp>
#include <cppurses/system/mouse_data.hpp>
#include <cppurses/widget/widgets/textbox_base.hpp>

namespace cppurses {

class Textbox : public Textbox_base {
   public:
    explicit Textbox(Glyph_string contents = "");

    void enable_scrollwheel(bool enable = true);
    void disable_scrollwheel(bool disable = true);
    void toggle_scrollwheel();
    bool does_scrollwheel() const;
    void set_wheel_speed(std::size_t lines);
    void set_wheel_speed_up(std::size_t lines);
    void set_wheel_speed_down(std::size_t lines);
    void disable_input();
    void enable_input();

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
#endif  // WIDGET_WIDGETS_TEXTBOX_HPP

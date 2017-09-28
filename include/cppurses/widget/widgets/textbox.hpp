#ifndef WIDGET_WIDGETS_TEXTBOX_HPP
#define WIDGET_WIDGETS_TEXTBOX_HPP
#include <cstddef>
#include <signals/slot.hpp>
#include "painter/glyph_string.hpp"
#include "system/key.hpp"
#include "widget/widgets/textbox_base.hpp"

namespace cppurses {
class Key_event;
class Mouse_event;

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

   protected:
    bool key_press_event(Key key, char symbol) override;
    bool mouse_press_event(Mouse_button button,
                           std::size_t global_x,
                           std::size_t global_y,
                           std::size_t local_x,
                           std::size_t local_y,
                           std::uint8_t device_id) override;

   private:
    bool scroll_wheel_{true};
    std::size_t scroll_speed_up_{1};
    std::size_t scroll_speed_down_{1};
};

namespace slot {

sig::Slot<void()> enable_scrollwheel(Textbox& tb);
sig::Slot<void()> disable_scrollwheel(Textbox& tb);
sig::Slot<void()> toggle_scrollwheel(Textbox& tb);
sig::Slot<void()> set_scrollwheel(Textbox& tb, bool enable);
sig::Slot<void(bool)> set_scrollwheel(Textbox& tb);

sig::Slot<void()> set_wheel_speed(Textbox& tb, std::size_t lines);
sig::Slot<void(std::size_t)> set_wheel_speed(Textbox& tb);

sig::Slot<void()> set_wheel_speed_up(Textbox& tb, std::size_t lines);
sig::Slot<void(std::size_t)> set_wheel_speed_up(Textbox& tb);

sig::Slot<void()> set_wheel_speed_down(Textbox& tb, std::size_t lines);
sig::Slot<void(std::size_t)> set_wheel_speed_down(Textbox& tb);

}  // namespace slot

}  // namespace cppurses
#endif  // WIDGET_WIDGETS_TEXTBOX_HPP

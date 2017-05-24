#ifndef WIDGET_MODULE_WIDGETS_TEXTBOX_HPP
#define WIDGET_MODULE_WIDGETS_TEXTBOX_HPP

#include "painter_module/glyph_string.hpp"
#include "widget_module/widgets/textbox_core.hpp"

namespace twf {
class Key_event;
class Mouse_event;

class Textbox : public Textbox_core {
   public:
    explicit Textbox(Glyph_string contents = "");

    void enable_scroll_wheel(bool enable = true);
    void disable_scroll_wheel(bool disable = true);
    bool does_scroll_wheel() const;
    void set_wheel_speed(std::size_t lines);
    void set_wheel_speed_up(std::size_t lines);
    void set_wheel_speed_down(std::size_t lines);

    // are these connected to destroyed signal?
    sig::Slot<void()> scroll_up_slot = [this] { this->scroll_up(1); };
    sig::Slot<void()> scroll_down_slot = [this] { this->scroll_down(1); };

   protected:
    bool key_press_event(const Key_event& event) override;
    bool mouse_press_event(const Mouse_event& event) override;

   private:
    bool scroll_wheel_{true};
    std::size_t scroll_speed_up_{1};
    std::size_t scroll_speed_down_{1};
};

}  // namespace twf
#endif  // WIDGET_MODULE_WIDGETS_TEXTBOX_HPP

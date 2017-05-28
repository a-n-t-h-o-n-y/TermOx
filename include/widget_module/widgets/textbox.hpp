#ifndef WIDGET_MODULE_WIDGETS_TEXTBOX_HPP
#define WIDGET_MODULE_WIDGETS_TEXTBOX_HPP

#include "painter_module/glyph_string.hpp"
#include "widget_module/widgets/textbox_base.hpp"

namespace cppurses {
class Key_event;
class Mouse_event;

class Textbox : public Textbox_base {
   public:
    explicit Textbox(Glyph_string contents = "");

    void enable_scrollwheel(bool enable = true);
    void disable_scrollwheel(bool disable = true);
    bool does_scrollwheel() const;
    void set_wheel_speed(std::size_t lines);
    void set_wheel_speed_up(std::size_t lines);
    void set_wheel_speed_down(std::size_t lines);

   protected:
    bool key_press_event(const Key_event& event) override;
    bool mouse_press_event(const Mouse_event& event) override;

   private:
    using Textbox_base::scroll_up_;
    using Textbox_base::scroll_down_;

    bool scroll_wheel_{true};
    std::size_t scroll_speed_up_{1};
    std::size_t scroll_speed_down_{1};
};

}  // namespace cppurses
#endif  // WIDGET_MODULE_WIDGETS_TEXTBOX_HPP

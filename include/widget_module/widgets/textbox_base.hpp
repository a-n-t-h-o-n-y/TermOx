#ifndef WIDGET_MODULE_WIDGETS_TEXTBOX_BASE_HPP
#define WIDGET_MODULE_WIDGETS_TEXTBOX_BASE_HPP

#include "painter_module/glyph_string.hpp"
#include "widget_module/coordinate.hpp"
#include "widget_module/widget.hpp"
#include "widget_module/widgets/text_display.hpp"
#include <cstddef>

namespace twf {
class Resize_event;

class Textbox_base : public Text_display {
   public:
    explicit Textbox_base(Glyph_string contents = "");

    void cursor_up(std::size_t n = 1);
    void cursor_down(std::size_t n = 1);
    void cursor_left(std::size_t n = 1);
    void cursor_right(std::size_t n = 1);
    void set_cursor_at_coordinates(Coordinate pos);
    void set_cursor_at_coordinates(std::size_t x, std::size_t y);
    void set_cursor_at_index(std::size_t index);
    std::size_t cursor_index() const;

    void enable_scrolling(bool enable = true) { scroll_ = enable; }
    void disable_scrolling(bool disable = true) { scroll_ = !disable; }
    bool does_scroll() const { return scroll_; }

   private:
    void increment_cursor_right();
    void increment_cursor_left();
    bool scroll_{true};

   protected:
    bool resize_event(const Resize_event& event) override;

    void scroll_up(std::size_t n = 1);
    void scroll_down(std::size_t n = 1);
};

}  // namespace twf
#endif  // WIDGET_MODULE_WIDGETS_TEXTBOX_BASE_HPP

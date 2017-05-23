#ifndef TEXTBOX_CORE_HPP
#define TEXTBOX_CORE_HPP

#include "../widget.hpp"
#include "../coordinate.hpp"
#include "../../painter_module/glyph_string.hpp"
#include "widget_module/widgets/text_display.hpp"

#include <cstddef>

namespace twf {
class Resize_event;

class Textbox_core : public Text_display {
   public:
    Textbox_core(Glyph_string contents = "");

    void cursor_up(std::size_t n = 1);
    void cursor_down(std::size_t n = 1);
    void cursor_left(std::size_t n = 1);
    void cursor_right(std::size_t n = 1);
    void set_cursor_at_coordinates(Coordinate);
    void set_cursor_at_coordinates(std::size_t x, std::size_t y);
    void set_cursor_at_index(std::size_t index);
    std::size_t cursor_index() const;

    void enable_scrolling(bool enable = true) { scroll_ = enable; }
    void disable_scrolling(bool disable = true) { scroll_ = !disable; }
    bool does_scroll() const { return scroll_; }

    // are these connected to destroyed signal?
    sig::Slot<void()> scroll_up_slot = [this] { this->scroll_up(); };
    sig::Slot<void()> scroll_down_slot = [this] { this->scroll_down(); };

   private:
    void increment_cursor_right();
    void increment_cursor_left();
    bool scroll_{true};

   protected:
    bool resize_event(const Resize_event& event) override;

    void scroll_up(std::size_t n = 1) override;
    void scroll_down(std::size_t n = 1) override;
};

}  // namespace twf
#endif  // TEXTBOX_CORE_HPP

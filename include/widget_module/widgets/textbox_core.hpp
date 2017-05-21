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

    // Glyph_string text() const { return contents_; }

    void cursor_up(std::size_t n = 1);
    void cursor_down(std::size_t n = 1);
    void cursor_left(std::size_t n = 1);
    void cursor_right(std::size_t n = 1);
    void set_cursor_coordinates(Coordinate);
    void set_cursor_coordinates(std::size_t x, std::size_t y);
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
    // bool paint_event(const Paint_event& event) override;
    // bool resize_event(const Resize_event& event) override;

    // std::size_t index_from_position(Coordinate pos);
    // std::size_t index_from_position(std::size_t x, std::size_t y);
    // Coordinate position_from_index(std::size_t index);
    // void set_cursor_index(std::size_t index);
    // std::size_t previous_line_break(std::size_t current_upper_bound);
    // std::size_t find_lower_bound();

    // Glyph_string contents_;
    // std::size_t cursor_index_ = 0;
    // std::size_t upper_bound_ = 0;
    // std::size_t lower_bound_ = 0;
};

}  // namespace twf
#endif  // TEXTBOX_CORE_HPP

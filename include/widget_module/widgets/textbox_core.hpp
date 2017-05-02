#ifndef TEXTBOX_CORE_HPP
#define TEXTBOX_CORE_HPP

#include "../widget.hpp"
#include "../coordinate.hpp"
#include "../../system_module/events/paint_event.hpp"
#include "../../painter_module/glyph_string.hpp"

#include <cstddef>

namespace twf {

class Textbox_core : public Widget {
   public:
    Textbox_core(const Glyph_string& contents = Glyph_string{});

    void scroll_up(std::size_t n = 1);
    void scroll_down(std::size_t n = 1);
    void cursor_up(std::size_t n = 1);
    void cursor_down(std::size_t n = 1);
    void cursor_left(std::size_t n = 1);
    void cursor_right(std::size_t n = 1);

    sig::Slot<void()> scroll_up_slot =
        std::bind(&Textbox_core::scroll_up, this, 1);

    sig::Slot<void()> scroll_down_slot =
        std::bind(&Textbox_core::scroll_down, this, 1);

   protected:
    bool paint_event(const Paint_event& event) override;
    bool resize_event(const Resize_event& event) override;

    std::size_t index_from_position(Coordinate pos);
    std::size_t index_from_position(std::size_t x, std::size_t y);
    Coordinate position_from_index(std::size_t index);
    void set_cursor_index(std::size_t index);
    std::size_t previous_line_break(std::size_t current_upper_bound);
    std::size_t find_lower_bound();

    Glyph_string contents_;
    std::size_t cursor_index_ = 0;
    std::size_t upper_bound_ = 0;
    std::size_t lower_bound_ = 0;
};

}  // namespace twf
#endif  // TEXTBOX_CORE_HPP

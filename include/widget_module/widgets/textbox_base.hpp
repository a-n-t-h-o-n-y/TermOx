#ifndef WIDGET_MODULE_WIDGETS_TEXTBOX_BASE_HPP
#define WIDGET_MODULE_WIDGETS_TEXTBOX_BASE_HPP

#include "painter_module/glyph_string.hpp"
#include "widget_module/coordinates.hpp"
#include "widget_module/widgets/text_display.hpp"
#include <aml/signals/signals.hpp>
#include <cstddef>

namespace cppurses {
class Resize_event;

class Textbox_base : public Text_display {
   public:
    explicit Textbox_base(Glyph_string contents = "");

    // Cursor Movement
    sig::Slot<void()> cursor_up;
    sig::Slot<void()> cursor_down;
    sig::Slot<void()> cursor_left;
    sig::Slot<void()> cursor_right;
    sig::Slot<void(std::size_t)> cursor_up_n;
    sig::Slot<void(std::size_t)> cursor_down_n;
    sig::Slot<void(std::size_t)> cursor_left_n;
    sig::Slot<void(std::size_t)> cursor_right_n;
    sig::Slot<void(Coordinates)> set_cursor_at_coordinates;
    sig::Slot<void(std::size_t, std::size_t)> set_cursor_at_coordinates_x_y;
    sig::Slot<void(std::size_t)> set_cursor_at_index;

    // Scrolling
    sig::Slot<void()> scroll_up;
    sig::Slot<void()> scroll_down;
    sig::Slot<void(std::size_t)> scroll_up_n;
    sig::Slot<void(std::size_t)> scroll_down_n;
    sig::Slot<void()> enable_scrolling;
    sig::Slot<void()> disable_scrolling;
    sig::Slot<void()> toggle_scrolling;
    sig::Slot<void(bool)> set_scrolling;

    // Query Functions
    bool does_scroll() const { return scroll_; }
    std::size_t cursor_index() const;

    // Signals
    sig::Signal<void(std::size_t n)> cursor_moved_left;
    sig::Signal<void(std::size_t n)> cursor_moved_right;
    sig::Signal<void(std::size_t n)> cursor_moved_up;
    sig::Signal<void(std::size_t n)> cursor_moved_down;
    sig::Signal<void(Coordinates)> cursor_moved;

   private:
    using Text_display::scroll_up_;
    using Text_display::scroll_down_;
    void initialize();

    void increment_cursor_right();
    void increment_cursor_left();
    bool scroll_{true};

   protected:
    bool resize_event(const Resize_event& event) override;
    void scroll_up_(std::size_t n);
    void scroll_down_(std::size_t n);
    void enable_scrolling_(bool enable = true) { scroll_ = enable; }
    void disable_scrolling_(bool disable = true) { scroll_ = !disable; }
    void cursor_up_(std::size_t n);
    void cursor_down_(std::size_t n);
    void cursor_left_(std::size_t n);
    void cursor_right_(std::size_t n);
    void set_cursor_at_coordinates_(Coordinates pos);
    void set_cursor_at_coordinates_(std::size_t x, std::size_t y);
    void set_cursor_at_index_(std::size_t index);
};

}  // namespace cppurses
#endif  // WIDGET_MODULE_WIDGETS_TEXTBOX_BASE_HPP

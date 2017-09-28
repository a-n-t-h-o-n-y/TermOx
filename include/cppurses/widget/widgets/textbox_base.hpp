#ifndef WIDGET_WIDGETS_TEXTBOX_BASE_HPP
#define WIDGET_WIDGETS_TEXTBOX_BASE_HPP
#include <cstddef>
#include <signals/signals.hpp>
#include "painter/glyph_string.hpp"
#include "widget/coordinates.hpp"
#include "widget/widgets/text_display.hpp"

namespace cppurses {
class Resize_event;

class Textbox_base : public Text_display {
   public:
    explicit Textbox_base(Glyph_string contents = "");

    // Scrolling
    void scroll_up(std::size_t n = 1) override;
    void scroll_down(std::size_t n = 1) override;
    void enable_scrolling(bool enable = true);
    void disable_scrolling(bool disable = true);
    void toggle_scrolling();

    // Cursor Movement
    void cursor_up(std::size_t n = 1);
    void cursor_down(std::size_t n = 1);
    void cursor_left(std::size_t n = 1);
    void cursor_right(std::size_t n = 1);
    void set_cursor(Coordinates pos);
    void set_cursor(std::size_t x, std::size_t y);
    void set_cursor(std::size_t index);

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
    void increment_cursor_right();
    void increment_cursor_left();
    bool scroll_{true};

   protected:
    bool resize_event(std::size_t new_width,
                      std::size_t new_height,
                      std::size_t old_width,
                      std::size_t old_height) override;
};

namespace slot {

// Cursor Movement
sig::Slot<void()> cursor_up(Textbox_base& tb, std::size_t n);
sig::Slot<void(std::size_t)> cursor_up(Textbox_base& tb);
sig::Slot<void()> cursor_down(Textbox_base& tb, std::size_t n);
sig::Slot<void(std::size_t)> cursor_down(Textbox_base& tb);
sig::Slot<void()> cursor_left(Textbox_base& tb, std::size_t n);
sig::Slot<void(std::size_t)> cursor_left(Textbox_base& tb);
sig::Slot<void()> cursor_right(Textbox_base& tb, std::size_t n);
sig::Slot<void(std::size_t)> cursor_right(Textbox_base& tb);

sig::Slot<void()> set_cursor(Textbox_base& tb, const Coordinates& coords);
sig::Slot<void(Coordinates)> set_cursor(Textbox_base& tb);
sig::Slot<void()> set_cursor_xy(Textbox_base& tb, std::size_t x, std::size_t y);
sig::Slot<void(std::size_t, std::size_t)> set_cursor_xy(Textbox_base& tb);
sig::Slot<void()> set_cursor_to_index(Textbox_base& tb, std::size_t index);
sig::Slot<void(std::size_t)> set_cursor_to_index(Textbox_base& tb);

// Scrolling
sig::Slot<void()> scroll_up(Textbox_base& tb, std::size_t n);
sig::Slot<void(std::size_t)> scroll_up(Textbox_base& tb);
sig::Slot<void()> scroll_down(Textbox_base& tb, std::size_t n);
sig::Slot<void(std::size_t)> scroll_down(Textbox_base& tb);
sig::Slot<void()> enable_scrolling(Textbox_base& tb);
sig::Slot<void()> disable_scrolling(Textbox_base& tb);
sig::Slot<void()> toggle_scrolling(Textbox_base& tb);
sig::Slot<void(bool)> set_scrolling(Textbox_base& tb);

}  // namespace slot

}  // namespace cppurses
#endif  // WIDGET_WIDGETS_TEXTBOX_BASE_HPP

#ifndef CPPURSES_WIDGET_WIDGETS_TEXTBOX_BASE_HPP
#define CPPURSES_WIDGET_WIDGETS_TEXTBOX_BASE_HPP
#include <cstddef>

#include <signals/signal.hpp>

#include <cppurses/painter/glyph_string.hpp>
#include <cppurses/widget/point.hpp>
#include <cppurses/widget/widgets/text_display.hpp>

namespace cppurses {

class Textbox_base : public Text_display {
   public:
    explicit Textbox_base(Glyph_string contents = "")
        : Text_display{std::move(contents)} {
        this->cursor.enable();
    }

    // Scrolling
    void scroll_up(std::size_t n = 1) override;
    void scroll_down(std::size_t n = 1) override;
    void enable_scrolling(bool enable = true) { scroll_ = enable; }
    void disable_scrolling(bool disable = true) { scroll_ = !disable; }
    void toggle_scrolling() { scroll_ = !scroll_; }

    // Cursor Movement
    void cursor_up(std::size_t n = 1);
    void cursor_down(std::size_t n = 1);
    void cursor_left(std::size_t n = 1);
    void cursor_right(std::size_t n = 1);

    void set_cursor(Point pos) { this->set_cursor(pos.x, pos.y); }

    void set_cursor(std::size_t x, std::size_t y) {
        this->set_cursor(this->index_at({x, y}));
    }

    void set_cursor(std::size_t index);

    // Query Functions
    bool does_scroll() const { return scroll_; }

    std::size_t cursor_index() const {
        return this->index_at(this->cursor.position());
    }

    // Signals
    sig::Signal<void(std::size_t n)> cursor_moved_left;
    sig::Signal<void(std::size_t n)> cursor_moved_right;
    sig::Signal<void(std::size_t n)> cursor_moved_up;
    sig::Signal<void(std::size_t n)> cursor_moved_down;

   private:
    void increment_cursor_right();
    void increment_cursor_left();
    bool scroll_{true};

   protected:
    bool resize_event(Area new_size, Area old_size) override;
};

}  // namespace cppurses
#endif  // CPPURSES_WIDGET_WIDGETS_TEXTBOX_BASE_HPP

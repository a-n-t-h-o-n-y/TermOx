#ifndef CPPURSES_WIDGET_WIDGETS_TEXTBOX_HPP
#define CPPURSES_WIDGET_WIDGETS_TEXTBOX_HPP
#include <cstddef>

#include <cppurses/painter/glyph_string.hpp>
#include <cppurses/system/events/key.hpp>
#include <cppurses/system/events/mouse.hpp>
#include <cppurses/widget/widgets/detail/textbox_base.hpp>

namespace cppurses {

/// Interactive Glyph_string display.
/** Provides common text interaction and presentation options as well as proper
 *  cursor movement for these interactions. */
class Textbox : public detail::Textbox_base {
   public:
    /// Construct a Textbox with initial \p contents and strong focus.
    explicit Textbox(Glyph_string contents = "");

    /// Enable the mouse scroll wheel to scroll the display up and down.
    /** Enabled by default at construction. */
    void enable_scrollwheel(bool enable = true) { scroll_wheel_ = enable; }

    /// Disable the mouse scroll wheel from scrolling the display up and down.
    void disable_scrollwheel(bool disable = true) { scroll_wheel_ = !disable; }

    /// Return if the mouse scroll wheel is enabled.
    bool scrollwheel_enabled() const { return scroll_wheel_; }

    /// Set the number of lines a single scroll will move the display by.
    /** Default wheel speed is 1. */
    void set_wheel_speed(std::size_t lines);

    /// Set the number of lines a single scroll up will move the display by.
    /** Default wheel speed is 1. */
    void set_wheel_speed_up(std::size_t lines) { scroll_speed_up_ = lines; }

    /// Set the number of lines a single scroll down will move the display by.
    /** Default wheel speed is 1. */
    void set_wheel_speed_down(std::size_t lines) { scroll_speed_down_ = lines; }

    /// Enable the Textbox to take keyboard input.
    /** Enabled by default at construction. */
    void enable_input() { takes_input_ = true; }

    /// Disable the Textbox from taking keyboard input.
    void disable_input() { takes_input_ = false; }

   protected:
    /// Either input a Glyph from the Key, or move the cursor on arrow presses.
    bool key_press_event(const Key::State& keyboard) override;

    /// Move the cursor to the pressed, or nearest cell, that contains a Glyph.
    bool mouse_press_event(const Mouse::State& mouse) override;

   private:
    bool scroll_wheel_{true};
    bool takes_input_{true};
    std::size_t scroll_speed_up_{1};
    std::size_t scroll_speed_down_{1};
};

}  // namespace cppurses
#endif  // CPPURSES_WIDGET_WIDGETS_TEXTBOX_HPP

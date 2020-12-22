#ifndef TERMOX_WIDGET_WIDGETS_TEXTBOX_HPP
#define TERMOX_WIDGET_WIDGETS_TEXTBOX_HPP
#include <cstddef>
#include <utility>

#include <termox/painter/glyph_string.hpp>
#include <termox/system/key.hpp>
#include <termox/system/mouse.hpp>
#include <termox/widget/focus_policy.hpp>
#include <termox/widget/widgets/detail/textbox_base.hpp>

namespace ox {

/// Interactive Glyph_string display.
/** Provides common text interaction and presentation options as well as proper
 *  cursor movement for these interactions. */
class Textbox : public detail::Textbox_base {
   public:
    /// Construct a Textbox with initial \p contents and strong focus.
    explicit Textbox(Glyph_string contents = "")
        : Textbox_base{std::move(contents)}
    {
        this->focus_policy = Focus_policy::Strong;
    }

    /// Enable the mouse scroll wheel to scroll the display up and down.
    /** Enabled by default at construction. */
    void enable_scrollwheel(bool enable = true) { scroll_wheel_ = enable; }

    /// Disable the mouse scroll wheel from scrolling the display up and
    /// down.
    void disable_scrollwheel(bool disable = true) { scroll_wheel_ = !disable; }

    /// Return if the mouse scroll wheel is enabled.
    auto scrollwheel_enabled() const -> bool { return scroll_wheel_; }

    /// Set the number of lines a single scroll will move the display by.
    /** Default wheel speed is 1. */
    void set_wheel_speed(std::size_t lines)
    {
        this->set_wheel_speed_up(lines);
        this->set_wheel_speed_down(lines);
    }

    /// Set the number of lines a single scroll up will move the display by.
    /** Default wheel speed is 1. */
    void set_wheel_speed_up(std::size_t lines) { scroll_speed_up_ = lines; }

    /// Set the number of lines a single scroll down will move the display
    /// by.
    /** Default wheel speed is 1. */
    void set_wheel_speed_down(std::size_t lines) { scroll_speed_down_ = lines; }

    /// Enable the Textbox to take keyboard input.
    /** Enabled by default at construction. */
    void enable_input() { takes_input_ = true; }

    /// Disable the Textbox from taking keyboard input.
    void disable_input() { takes_input_ = false; }

   protected:
    /// Either input a Glyph from the Key, or move the cursor on arrow presses.
    auto key_press_event(Key k) -> bool override;

    /// Move the cursor to the pressed, or nearest cell, that contains a Glyph.
    auto mouse_press_event(Mouse const& m) -> bool override;

    /// Scroll
    auto mouse_wheel_event(Mouse const& m) -> bool override;

   private:
    bool scroll_wheel_             = true;
    bool takes_input_              = true;
    std::size_t scroll_speed_up_   = 1;
    std::size_t scroll_speed_down_ = 1;
};

/// Helper function to create an instance.
template <typename... Args>
auto textbox(Args&&... args) -> std::unique_ptr<Textbox>
{
    return std::make_unique<Textbox>(std::forward<Args>(args)...);
}

}  // namespace ox
#endif  // TERMOX_WIDGET_WIDGETS_TEXTBOX_HPP

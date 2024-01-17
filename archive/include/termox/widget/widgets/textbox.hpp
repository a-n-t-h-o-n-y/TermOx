#ifndef TERMOX_WIDGET_WIDGETS_TEXTBOX_HPP
#define TERMOX_WIDGET_WIDGETS_TEXTBOX_HPP
#include <memory>

#include <termox/painter/glyph_string.hpp>
#include <termox/system/key.hpp>
#include <termox/system/mouse.hpp>
#include <termox/widget/widgets/detail/textbox_base.hpp>

namespace ox {

/// Interactive Glyph_string display with text wrapping, alignment, etc...
/** Provides common text interaction and presentation options as well as proper
 *  cursor movement for these interactions. */
class Textbox : public detail::Textbox_base {
   public:
    struct Parameters {
        Glyph_string text  = U"";
        Align alignment    = Align::Left;
        Wrap wrap          = Wrap::Word;
        Brush insert_brush = Brush{};
        int scroll_speed   = 1;
        bool text_input    = true;
    };

   public:
    /// Construct a Textbox with initial \p contents and strong focus.
    explicit Textbox(Glyph_string text  = U"",
                     Align alignment    = Align::Left,
                     Wrap wrap          = Wrap::Word,
                     Brush insert_brush = Brush{},
                     int scroll_speed   = 1,
                     bool text_input    = true);

    explicit Textbox(Parameters p);

   public:
    /// Set the number of lines scrolled vertically on scroll wheel events.
    /** Default value is one. Negative will invert. */
    void set_scroll_speed(int x) noexcept;

    /// Return the current scroll wheel speed.
    [[nodiscard]] auto scroll_speed() const noexcept -> int;

    /// Enable the Textbox to take keyboard input.
    void enable_text_input() noexcept;

    /// Disable the Textbox from taking keyboard input.
    void disable_text_input() noexcept;

    /// Return true if currently takes text keyboard input.
    [[nodiscard]] auto has_text_input() const noexcept -> bool;

   protected:
    /// Either input a Glyph from the Key, or move the cursor on arrow presses.
    auto key_press_event(Key k) -> bool override;

    /// Move the cursor to the pressed, or nearest cell, that contains a Glyph.
    auto mouse_press_event(Mouse const& m) -> bool override;

    /// Scroll.
    auto mouse_wheel_event(Mouse const& m) -> bool override;

   private:
    int scroll_speed_;
    bool takes_input_;
};

/// Helper function to create a Textbox instance.
[[nodiscard]] auto textbox(Glyph_string text  = U"",
                           Align alignment    = Align::Left,
                           Wrap wrap          = Wrap::Word,
                           Brush insert_brush = Brush{},
                           int scroll_speed   = 1,
                           bool text_input = true) -> std::unique_ptr<Textbox>;

/// Helper function to create a Textbox instance.
[[nodiscard]] auto textbox(Textbox::Parameters p) -> std::unique_ptr<Textbox>;

}  // namespace ox
#endif  // TERMOX_WIDGET_WIDGETS_TEXTBOX_HPP

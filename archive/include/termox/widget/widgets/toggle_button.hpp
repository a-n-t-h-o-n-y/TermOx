#ifndef TERMOX_WIDGET_WIDGETS_TOGGLE_BUTTON_HPP
#define TERMOX_WIDGET_WIDGETS_TOGGLE_BUTTON_HPP
#include <memory>
#include <utility>

#include <signals_light/signal.hpp>

#include <termox/painter/glyph_string.hpp>
#include <termox/widget/pair.hpp>
#include <termox/widget/widgets/button.hpp>

namespace ox {

/// A Button with two alternating sides.
/** The top button is active first, switching between the two sides on clicks */
class Toggle_button : public SPair<Button, Button> {
   public:
    struct Parameters {
        Glyph_string top_text;
        Glyph_string bottom_text;
    };

   public:
    Button& top    = this->first;
    Button& bottom = this->second;

   public:
    sl::Signal<void()>& top_pressed    = top.pressed;
    sl::Signal<void()>& bottom_pressed = bottom.pressed;

   public:
    /// Construct with corresponding labels.
    Toggle_button(Glyph_string top_text, Glyph_string bottom_text);

    /// Construct with given \p p.
    explicit Toggle_button(Parameters p);

   public:
    /// Display the top button, without emitting any Signals.
    void show_top();

    /// Display the bottom button, without emitting any Signals.
    void show_bottom();

    /// Change the displayed button without emitting any signals.
    void toggle();
};

/// Helper function to create a Toggle_button instance.
[[nodiscard]] auto toggle_button(Glyph_string top_text,
                                 Glyph_string bottom_text)
    -> std::unique_ptr<Toggle_button>;

/// Helper function to create a Toggle_button instance.
[[nodiscard]] auto toggle_button(Toggle_button::Parameters p)
    -> std::unique_ptr<Toggle_button>;

}  // namespace ox
#endif  // TERMOX_WIDGET_WIDGETS_TOGGLE_BUTTON_HPP

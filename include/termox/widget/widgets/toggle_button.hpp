#ifndef TERMOX_WIDGET_WIDGETS_TOGGLE_BUTTON_HPP
#define TERMOX_WIDGET_WIDGETS_TOGGLE_BUTTON_HPP
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
    Toggle_button(Glyph_string top_text, Glyph_string bottom_text)
        : SPair<Button, Button>{{std::move(top_text)}, {std::move(bottom_text)}}
    {
        using namespace ox::pipe;
        *this | active_page(top_index_);
        this->give_focus_on_change(false);

        top | on_press([this]() { *this | active_page(bottom_index_); });
        bottom | on_press([this]() { *this | active_page(top_index_); });
    }

    /// Construct with given \p parameters.
    Toggle_button(Parameters parameters)
        : Toggle_button{std::move(parameters.top_text),
                        std::move(parameters.bottom_text)}
    {}

   public:
    /// Display the top button, without emitting any Signals.
    void show_top() { this->set_active_page(top_index_); }

    /// Display the bottom button, without emitting any Signals.
    void show_bottom() { this->set_active_page(bottom_index_); }

    /// Change the displayed button without emitting any signals.
    void toggle()
    {
        if (this->active_page_index() == top_index_)
            this->set_active_page(bottom_index_);
        else
            this->set_active_page(top_index_);
    }

   private:
    static auto constexpr top_index_    = 0uL;
    static auto constexpr bottom_index_ = 1uL;
};

/// Helper function to create a Toggle_button instance.
[[nodiscard]] inline auto toggle_button(Glyph_string top_text,
                                        Glyph_string bottom_text)
    -> std::unique_ptr<Toggle_button>
{
    return std::make_unique<Toggle_button>(std::move(top_text),
                                           std::move(bottom_text));
}

/// Helper function to create a Toggle_button instance.
[[nodiscard]] inline auto toggle_button(Toggle_button::Parameters parameters)
    -> std::unique_ptr<Toggle_button>
{
    return std::make_unique<Toggle_button>(std::move(parameters));
}

}  // namespace ox
#endif  // TERMOX_WIDGET_WIDGETS_TOGGLE_BUTTON_HPP

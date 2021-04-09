#ifndef TERMOX_WIDGET_WIDGETS_TITLEBAR_HPP
#define TERMOX_WIDGET_WIDGETS_TITLEBAR_HPP
#include <utility>

#include <termox/painter/color.hpp>
#include <termox/painter/glyph_string.hpp>
#include <termox/system/system.hpp>
#include <termox/widget/pair.hpp>
#include <termox/widget/pipe.hpp>
#include <termox/widget/size_policy.hpp>
#include <termox/widget/widgets/button.hpp>
#include <termox/widget/widgets/label.hpp>

namespace ox {

/// Provides a centered title and flush-right exit button.
/** The exit button will call System::quit() when pressed. Titlebar is fixed at
 *  a height of 1. */
// class Titlebar : public layout::Horizontal<> {
class Titlebar : public HPair<HLabel, Button> {
   public:
    struct Parameters {
        Glyph_string title_text = U"";
    };

   public:
    HLabel& title       = this->first;
    Button& exit_button = this->second;

   public:
    /// Construct a Titlebar with centered \p title.
    explicit Titlebar(Glyph_string title_text = U"")
        : HPair<HLabel, Button>{
              {std::move(title_text), Align::Center, extra_left, extra_right},
              {U"│✕ "}}

    {
        using namespace ox::pipe;
        *this | fixed_height(1);
        *this | children() | bg(Color::White) | fg(Color::Black);
        exit_button | fixed_width(exit_width) | on_press(System::quit);
    }

    /// Construct a Titlebar from \p parameters
    explicit Titlebar(Parameters parameters)
        : Titlebar{std::move(parameters.title_text)}
    {}

   private:
    inline static auto constexpr exit_width  = 3;
    inline static auto constexpr extra_left  = 0;
    inline static auto constexpr extra_right = exit_width;
};

/// Helper function to create a Titlebar instance.
[[nodiscard]] inline auto titlebar(Glyph_string title_text = U"")
    -> std::unique_ptr<Titlebar>
{
    return std::make_unique<Titlebar>(std::move(title_text));
}

/// Helper function to create a Titlebar instance.
[[nodiscard]] inline auto titlebar(Titlebar::Parameters parameters)
    -> std::unique_ptr<Titlebar>
{
    return std::make_unique<Titlebar>(std::move(parameters));
}

}  // namespace ox
#endif  // TERMOX_WIDGET_WIDGETS_TITLEBAR_HPP

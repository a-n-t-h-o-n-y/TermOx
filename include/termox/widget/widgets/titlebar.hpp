#ifndef TERMOX_WIDGET_WIDGETS_TITLEBAR_HPP
#define TERMOX_WIDGET_WIDGETS_TITLEBAR_HPP
#include <utility>

#include <termox/painter/color.hpp>
#include <termox/painter/glyph_string.hpp>
#include <termox/system/system.hpp>
#include <termox/widget/layouts/horizontal.hpp>
#include <termox/widget/pipe.hpp>
#include <termox/widget/size_policy.hpp>
#include <termox/widget/widgets/button.hpp>
#include <termox/widget/widgets/label.hpp>

namespace ox {

/// Provides a centered title and flush-right exit button.
/** The exit button will call System::quit() when pressed. Titlebar is fixed at
 *  a height of 1. */
class Titlebar : public layout::Horizontal<> {
   public:
    HLabel& title;
    Button& exit_button;

   public:
    /// Construct a Titlebar with centered \p title.
    explicit Titlebar(Glyph_string title_ = "")
        : title{this->make_child<HLabel>(
              {std::move(title_), Align::Center, extra_left, extra_right})},
          exit_button{this->make_child<Button>(L"│✕ ")}
    {
        using namespace ox::pipe;
        *this | fixed_height(1uL);
        *this | children() | bg(Color::White) | fg(Color::Black);
        exit_button | fixed_width(exit_width) | on_press(System::quit);
    }

   private:
    inline static auto constexpr exit_width  = 3uL;
    inline static auto constexpr extra_left  = 0uL;
    inline static auto constexpr extra_right = exit_width;
};

/// Helper function to create an instance.
template <typename... Args>
auto titlebar(Args&&... args) -> std::unique_ptr<Titlebar>
{
    return std::make_unique<Titlebar>(std::forward<Args>(args)...);
}

}  // namespace ox
#endif  // TERMOX_WIDGET_WIDGETS_TITLEBAR_HPP

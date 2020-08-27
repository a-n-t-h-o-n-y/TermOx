#ifndef CPPURSES_WIDGET_WIDGETS_TITLEBAR_HPP
#define CPPURSES_WIDGET_WIDGETS_TITLEBAR_HPP
#include <utility>

#include <cppurses/painter/color.hpp>
#include <cppurses/painter/glyph_string.hpp>
#include <cppurses/system/system.hpp>
#include <cppurses/widget/layouts/horizontal.hpp>
#include <cppurses/widget/pipe.hpp>
#include <cppurses/widget/size_policy.hpp>
#include <cppurses/widget/widgets/button.hpp>
#include <cppurses/widget/widgets/label.hpp>

namespace cppurses {

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
        using namespace cppurses::pipe;
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

}  // namespace cppurses
#endif  // CPPURSES_WIDGET_WIDGETS_TITLEBAR_HPP

#ifndef CPPURSES_WIDGET_WIDGETS_TITLEBAR_HPP
#define CPPURSES_WIDGET_WIDGETS_TITLEBAR_HPP
#include <utility>

#include <cppurses/painter/color.hpp>
#include <cppurses/painter/glyph_string.hpp>
#include <cppurses/system/system.hpp>
#include <cppurses/widget/layouts/horizontal.hpp>
#include <cppurses/widget/size_policy.hpp>
#include <cppurses/widget/widgets/label.hpp>
#include <cppurses/widget/widgets/push_button.hpp>

namespace cppurses {

/// Provides a centered title and flush-right exit button.
/** The exit button will call System::quit() when pressed. Titlebar is fixed at
 *  a height of 1. */
class Titlebar : public layout::Horizontal<> {
   public:
    /// The centered title Widget.
    Label& title;

    /// The exit button Widget.
    Push_button& exit_button;

   public:
    /// Construct a Titlebar with centered \p title.
    explicit Titlebar(Glyph_string title_ = "")
        : title{this->make_child<Label>(std::move(title_))},
          exit_button{this->make_child<Push_button>(L"│✕ ")}
    {
        this->height_policy.fixed(1);

        title.width_policy.type(Size_policy::Expanding);
        title.brush.set_background(Color::White);
        title.brush.set_foreground(Color::Black);
        title.set_alignment(Alignment::Center);

        exit_button.clicked.connect(System::quit);
        exit_button.brush.set_background(Color::White);
        exit_button.brush.set_foreground(Color::Black);
        exit_button.width_policy.fixed(3);
    }
};

}  // namespace cppurses
#endif  // CPPURSES_WIDGET_WIDGETS_TITLEBAR_HPP

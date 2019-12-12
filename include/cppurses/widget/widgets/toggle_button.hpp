#ifndef CPPURSES_WIDGET_WIDGETS_TOGGLE_BUTTON_HPP
#define CPPURSES_WIDGET_WIDGETS_TOGGLE_BUTTON_HPP
#include <utility>

#include <signals/signal.hpp>

#include <cppurses/painter/glyph_string.hpp>
#include <cppurses/widget/layouts/stack.hpp>
#include <cppurses/widget/widgets/push_button.hpp>

namespace cppurses {
class Glyph_string;

/// A Push_button with two alternating sides.
/** The top button is active first, switching between the two sides on clicks */
class Toggle_button : public layout::Stack<Push_button> {
    static auto const top_index    = 0;
    static auto const bottom_index = 1;

   public:
    /// Construct with corresponding labels.
    Toggle_button(Glyph_string top_label, Glyph_string bottom_label)
        : top{this->make_page(std::move(top_label))},
          bottom{this->make_page(std::move(bottom_label))}
    {
        this->set_name("Toggle_button");
        top.clicked.connect([this]() { this->set_active_page(bottom_index); });
        bottom.clicked.connect([this]() { this->set_active_page(top_index); });
        this->set_active_page(top_index);
    }

    Push_button& top;
    Push_button& bottom;

    sig::Signal<void()>& top_clicked{top.clicked};
    sig::Signal<void()>& bottom_clicked{bottom.clicked};
};

}  // namespace cppurses
#endif  // CPPURSES_WIDGET_WIDGETS_TOGGLE_BUTTON_HPP

#ifndef CPPURSES_WIDGET_WIDGETS_TOGGLE_BUTTON_HPP
#define CPPURSES_WIDGET_WIDGETS_TOGGLE_BUTTON_HPP
#include <utility>

#include <signals/signal.hpp>

#include <cppurses/painter/glyph_string.hpp>
#include <cppurses/widget/layouts/stack.hpp>
#include <cppurses/widget/widgets/button.hpp>

namespace cppurses {
class Glyph_string;

/// A Button with two alternating sides.
/** The top button is active first, switching between the two sides on clicks */
class Toggle_button : public layout::Stack<Button> {
   public:
    Button& top;
    Button& bottom;

   public:
    sig::Signal<void()>& top_pressed    = top.pressed;
    sig::Signal<void()>& bottom_pressed = bottom.pressed;

   public:
    /// Construct with corresponding labels.
    Toggle_button(Glyph_string top_label, Glyph_string bottom_label)
        : top{this->make_page(std::move(top_label))},
          bottom{this->make_page(std::move(bottom_label))}
    {
        top.pressed.connect([this]() { this->set_active_page(bottom_index_); });
        bottom.pressed.connect([this]() { this->set_active_page(top_index_); });
        this->set_active_page(top_index_);
    }

   private:
    static auto constexpr top_index_    = 0;
    static auto constexpr bottom_index_ = 1;
};

}  // namespace cppurses
#endif  // CPPURSES_WIDGET_WIDGETS_TOGGLE_BUTTON_HPP

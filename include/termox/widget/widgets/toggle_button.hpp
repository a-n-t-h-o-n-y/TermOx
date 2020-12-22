#ifndef TERMOX_WIDGET_WIDGETS_TOGGLE_BUTTON_HPP
#define TERMOX_WIDGET_WIDGETS_TOGGLE_BUTTON_HPP
#include <utility>

#include <signals_light/signal.hpp>

#include <termox/painter/glyph_string.hpp>
#include <termox/widget/layouts/stack.hpp>
#include <termox/widget/widgets/button.hpp>

namespace ox {
class Glyph_string;

/// A Button with two alternating sides.
/** The top button is active first, switching between the two sides on clicks */
class Toggle_button : public layout::Stack<Button> {
   public:
    Button& top;
    Button& bottom;

   public:
    sl::Signal<void()>& top_pressed    = top.pressed;
    sl::Signal<void()>& bottom_pressed = bottom.pressed;

   public:
    /// Construct with corresponding labels.
    Toggle_button(Glyph_string top_label, Glyph_string bottom_label)
        : top{this->make_page(std::move(top_label))},
          bottom{this->make_page(std::move(bottom_label))}
    {
        top.pressed.connect([this]() { this->set_active_page(bottom_index_); });
        bottom.pressed.connect([this]() { this->set_active_page(top_index_); });
        this->set_active_page(top_index_);
        this->give_focus_on_change(false);
    }

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
    static auto constexpr top_index_    = 0;
    static auto constexpr bottom_index_ = 1;
};

/// Helper function to create an instance.
template <typename... Args>
auto toggle_button(Args&&... args) -> std::unique_ptr<Toggle_button>
{
    return std::make_unique<Toggle_button>(std::forward<Args>(args)...);
}

}  // namespace ox
#endif  // TERMOX_WIDGET_WIDGETS_TOGGLE_BUTTON_HPP

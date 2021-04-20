#ifndef TERMOX_WIDGET_WIDGETS_CONFIRM_BUTTON_HPP
#define TERMOX_WIDGET_WIDGETS_CONFIRM_BUTTON_HPP
#include <utility>

#include <signals_light/signal.hpp>

#include <termox/painter/color.hpp>
#include <termox/painter/glyph_string.hpp>
#include <termox/widget/array.hpp>
#include <termox/widget/layouts/stack.hpp>
#include <termox/widget/pipe.hpp>
#include <termox/widget/tuple.hpp>
#include <termox/widget/widgets/button.hpp>

namespace ox::detail {

class Confirm_page : public HArray<Button, 2> {
   public:
    struct Parameters {
        Glyph_string confirm_text;
    };

   public:
    Button& confirm_btn = this->get<0>();
    Button& exit_btn    = this->get<1>();

   public:
    explicit Confirm_page(Glyph_string confirm_text)
        : HArray<Button, 2>{
              {Button::Parameters{std::move(confirm_text)}, {U"X"}}}
    {
        using namespace pipe;
        confirm_btn | bg(Color::Blue) | fg(Color::Black);
        exit_btn | bg(Color::Gray) | fg(Color::Black) | fixed_width(3);
    }

    explicit Confirm_page(Parameters parameters)
        : Confirm_page{std::move(parameters.confirm_text)}
    {}
};

}  // namespace ox::detail

namespace ox {

/// A Button with a confirm screen after the initial press.
/** Clicked signal is only emitted after confirmation. */
class Confirm_button : public STuple<Button, detail::Confirm_page> {
   public:
    struct Parameters {
        Glyph_string label;
        Glyph_string confirm_text = "Confirm";
    };

   public:
    /// Emitted once confirmation has been made.
    sl::Signal<void()> pressed;

   public:
    Button& main_btn                   = this->get<0>();
    detail::Confirm_page& confirm_page = this->get<1>();

   public:
    /// Construct a Button with \p label and corresponding \p confirm_text.
    explicit Confirm_button(Glyph_string label        = U"",
                            Glyph_string confirm_text = U"Confirm")
        : STuple<Button, detail::Confirm_page>{{std::move(label)},
                                               {std::move(confirm_text)}}
    {
        using namespace pipe;
        *this | fixed_height(1) | active_page(front_page_);
        this->give_focus_on_change(false);

        main_btn |
            on_left_click([this] { *this | active_page(confirm_page_); });

        confirm_page.confirm_btn | on_left_click([this] {
            pressed.emit();
            *this | active_page(front_page_);
        });

        confirm_page.exit_btn |
            on_left_click([this] { *this | active_page(front_page_); });
    }

    explicit Confirm_button(Parameters parameters)
        : Confirm_button{std::move(parameters.label),
                         std::move(parameters.confirm_text)}
    {}

   private:
    static auto constexpr front_page_   = 0uL;
    static auto constexpr confirm_page_ = 1uL;
};

/// Helper function to create a Confirm_button instance.
[[nodiscard]] inline auto confirm_button(Glyph_string label        = U"",
                                         Glyph_string confirm_text = U"Confirm")
    -> std::unique_ptr<Confirm_button>
{
    return std::make_unique<Confirm_button>(std::move(label),
                                            std::move(confirm_text));
}

/// Helper function to create a Confirm_button instance.
[[nodiscard]] inline auto confirm_button(Confirm_button::Parameters parameters)
    -> std::unique_ptr<Confirm_button>
{
    return std::make_unique<Confirm_button>(std::move(parameters));
}

}  // namespace ox
#endif  // TERMOX_WIDGET_WIDGETS_CONFIRM_BUTTON_HPP

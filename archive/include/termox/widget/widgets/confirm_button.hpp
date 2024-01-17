#ifndef TERMOX_WIDGET_WIDGETS_CONFIRM_BUTTON_HPP
#define TERMOX_WIDGET_WIDGETS_CONFIRM_BUTTON_HPP
#include <memory>

#include <signals_light/signal.hpp>

#include <termox/painter/glyph_string.hpp>
#include <termox/widget/array.hpp>
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
    explicit Confirm_page(Glyph_string confirm_text);

    explicit Confirm_page(Parameters p);
};

}  // namespace ox::detail

namespace ox {

/// A Button with a confirm screen after the initial press.
/** Clicked signal is only emitted after confirmation. */
class Confirm_button : public STuple<Button, detail::Confirm_page> {
   public:
    struct Parameters {
        Glyph_string label        = U"";
        Glyph_string confirm_text = U"Confirm";
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
                            Glyph_string confirm_text = U"Confirm");

    explicit Confirm_button(Parameters p);

   private:
    static auto constexpr front_page_   = 0uL;
    static auto constexpr confirm_page_ = 1uL;
};

/// Helper function to create a Confirm_button instance.
[[nodiscard]] auto confirm_button(Glyph_string label        = U"",
                                  Glyph_string confirm_text = U"Confirm")
    -> std::unique_ptr<Confirm_button>;

/// Helper function to create a Confirm_button instance.
[[nodiscard]] auto confirm_button(Confirm_button::Parameters p)
    -> std::unique_ptr<Confirm_button>;

}  // namespace ox
#endif  // TERMOX_WIDGET_WIDGETS_CONFIRM_BUTTON_HPP

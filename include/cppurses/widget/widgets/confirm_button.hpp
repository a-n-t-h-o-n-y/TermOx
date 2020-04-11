#ifndef CPPURSES_WIDGET_WIDGETS_CONFIRM_BUTTON_HPP
#define CPPURSES_WIDGET_WIDGETS_CONFIRM_BUTTON_HPP
#include <utility>

#include <signals/signal.hpp>

#include <cppurses/painter/color.hpp>
#include <cppurses/painter/glyph_string.hpp>
#include <cppurses/widget/layouts/horizontal.hpp>
#include <cppurses/widget/layouts/stack.hpp>
#include <cppurses/widget/pipe.hpp>
#include <cppurses/widget/widgets/button.hpp>

namespace cppurses {

/// A Button with a confirm screen after the initial click.
/** Clicked signal is only emitted after confirmation. */
class Confirm_button : public layout::Stack<> {
   private:
    class Confirm_page : public layout::Horizontal<Button> {
       public:
        Child& confirm_btn;
        Child& exit_btn = this->make_child(L"X");

       public:
        explicit Confirm_page(Glyph_string confirm_text)
            : confirm_btn{this->make_child(std::move(confirm_text))}
        {
            using namespace pipe;
            confirm_btn | bg(Color::Blue) | fg(Color::Black);
            exit_btn | bg(Color::Gray) | fg(Color::Black) | fixed_width(3);
        }
    };

   public:
    /// Emitted once confirmation has been made.
    sig::Signal<void()> clicked;

   public:
    Button& main_btn;
    Confirm_page& confirm_page;

   public:
    /// Construct a Button with \p label and corresponding \p confirm_text.
    explicit Confirm_button(Glyph_string label,
                            Glyph_string confirm_text = "Confirm")
        : main_btn{this->make_page<Button>(std::move(label))},
          confirm_page{this->make_page<Confirm_page>(std::move(confirm_text))}
    {
        using namespace pipe;
        *this | fixed_height(1) | active_page(front_page_);

        main_btn |
            on_left_click([this] { *this | active_page(confirm_page_); });

        confirm_page.confirm_btn | on_left_click([this] {
            clicked();
            *this | active_page(front_page_);
        });

        confirm_page.exit_btn |
            on_left_click([this] { *this | active_page(front_page_); });
    }

   private:
    static auto constexpr front_page_   = 0uL;
    static auto constexpr confirm_page_ = 1uL;
};

}  // namespace cppurses
#endif  // CPPURSES_WIDGET_WIDGETS_CONFIRM_BUTTON_HPP

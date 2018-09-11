#include <cppurses/widget/widgets/confirm_button.hpp>

#include <utility>

#include <cppurses/painter/color.hpp>
#include <cppurses/painter/glyph_string.hpp>
#include <cppurses/widget/layouts/horizontal_layout.hpp>
#include <cppurses/widget/widget_free_functions.hpp>
#include <cppurses/widget/widgets/push_button.hpp>
#include <cppurses/widget/widgets/widget_stack.hpp>

namespace cppurses {

struct Confirm_button::Confirm_screen : public Horizontal_layout {
    explicit Confirm_screen(Glyph_string confirm_text)
        : confirm_btn{this->make_child<Push_button>(confirm_text)} {
        x_btn.width_policy.type(Size_policy::Fixed);
        x_btn.width_policy.hint(3);
        set_background(x_btn, Color::Gray);
        set_foreground(x_btn, Color::Black);
        set_background(confirm_btn, Color::Blue);
        set_foreground(confirm_btn, Color::Black);
    }

    Push_button& confirm_btn;
    Push_button& x_btn{this->make_child<Push_button>("✕")};
};

Confirm_button::Confirm_button(Glyph_string title, Glyph_string confirm_text)
    : title_btn_{this->make_page<Push_button>(std::move(title))},
      confirm_screen_{
          this->make_page<Confirm_screen>(std::move(confirm_text))} {
    title_btn_.clicked.connect([this] { this->set_active_page(1); });

    confirm_screen_.confirm_btn.clicked.connect([this] {
        this->clicked();
        this->set_active_page(0);
    });

    confirm_screen_.x_btn.clicked.connect([this] { this->set_active_page(0); });

    this->set_active_page(0);
}

}  // namespace cppurses

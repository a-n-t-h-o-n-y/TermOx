#include <cppurses/widget/widgets/confirm_button.hpp>

#include <cstddef>
#include <utility>

#include <cppurses/painter/color.hpp>
#include <cppurses/painter/glyph_string.hpp>
#include <cppurses/widget/layouts/horizontal.hpp>
#include <cppurses/widget/layouts/stack.hpp>
#include <cppurses/widget/widgets/push_button.hpp>

namespace {

auto constexpr front_page   = 0uL;
auto constexpr confirm_page = 1uL;

}  // namespace

namespace cppurses {

struct Confirm_button::Confirm_screen : public layout::Horizontal<Push_button> {
    explicit Confirm_screen(Glyph_string confirm_text)
        : confirm_btn{this->make_child(std::move(confirm_text))}
    {
        exit_btn.width_policy.fixed(3);
        exit_btn.brush.set_background(Color::Gray);
        exit_btn.brush.set_foreground(Color::Black);
        confirm_btn.brush.set_background(Color::Blue);
        confirm_btn.brush.set_foreground(Color::Black);
    }

    Push_button& confirm_btn;
    Push_button& exit_btn = this->make_child("✕");
};

Confirm_button::Confirm_button(Glyph_string label, Glyph_string confirm_text)
    : main_btn{this->make_page<Push_button>(std::move(label))},
      confirm_screen{this->make_page<Confirm_screen>(std::move(confirm_text))}
{
    this->height_policy.fixed(1);
    this->set_active_page(front_page);
    main_btn.clicked.connect([this] { this->set_active_page(confirm_page); });
    confirm_screen.confirm_btn.clicked.connect([this] {
        this->clicked();
        this->set_active_page(front_page);
    });
    confirm_screen.exit_btn.clicked.connect(
        [this] { this->set_active_page(front_page); });
}

}  // namespace cppurses

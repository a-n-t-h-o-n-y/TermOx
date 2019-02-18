#include <cppurses/widget/widgets/confirm_button.hpp>

#include <cstddef>
#include <utility>

#include <cppurses/painter/color.hpp>
#include <cppurses/painter/glyph_string.hpp>
#include <cppurses/widget/layouts/horizontal.hpp>
#include <cppurses/widget/layouts/stack.hpp>
#include <cppurses/widget/widgets/push_button.hpp>

namespace {
const std::size_t front_page{0};
const std::size_t confirm_page{1};
}  // namespace

namespace cppurses {

struct Confirm_button::Confirm_screen : public layout::Horizontal {
    explicit Confirm_screen(Glyph_string confirm_text)
        : confirm_btn{this->make_child<Push_button>(std::move(confirm_text))} {
        this->set_name("Confirm_screen");
        exit_btn.width_policy.fixed(3);
        exit_btn.brush.set_background(Color::Gray);
        exit_btn.brush.set_foreground(Color::Black);
        confirm_btn.brush.set_background(Color::Blue);
        confirm_btn.brush.set_foreground(Color::Black);
    }

    Push_button& confirm_btn;
    Push_button& exit_btn{this->make_child<Push_button>("✕")};
};

Confirm_button::Confirm_button(Glyph_string label, Glyph_string confirm_text)
    : main_btn{this->make_page<Push_button>(std::move(label))},
      confirm_screen{this->make_page<Confirm_screen>(std::move(confirm_text))} {
    this->set_name("Confirm_button");
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

#include <termox/termox.hpp>

#include "demo_menu.hpp"

// int main() { return ox::System{ox::Mouse_mode::Drag}.run<demo::Demos>(); }

struct Foo : ox::VTuple<ox::Widget, ox::Password_edit, ox::Textbox> {
    Foo()
    {
        this->get<1>().submitted.connect(
            [this](auto const& g) { this->get<2>().set_text(g); });
        this->get<0>() | bg(ox::Color::Blue);
        this->get<2>() | bg(ox::Color::Light_blue) | fg(ox::Color::Black);
    }
};

int main() { return ox::System{}.run<Foo>(); }

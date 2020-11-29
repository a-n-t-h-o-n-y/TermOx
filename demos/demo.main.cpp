// #include <cppurses/system/system.hpp>

// #include "demo_menu.hpp"

// int main() { return cppurses::System{}.run<demos::Demos>(); }
#include <cppurses/cppurses.hpp>

int main()
{
    using namespace cppurses;
    auto app_ptr =
        layout::vertical(
            textbox("Hello, World!" | foreground(Color::Red) | Trait::Bold) | pipe::name("tb"),
            layout::horizontal<Button>(
                button("Button 1") | pipe::bg(Color::Blue) | pipe::name("b1"),
                button("Button 2") | pipe::bg(Color::Violet) | pipe::name("b2")
            ) | pipe::fixed_height(1)
        );
    auto& tb = app_ptr->find_child_by_name("tb");
    app_ptr->find_child_by_name("b1") | pipe::on_press([&tb]{ tb.append("Button 1 Pressed"); });
    app_ptr->find_child_by_name("b2") | pipe::on_press([&tb]{ tb.append("Button 2 Pressed"); });
    return System{}.run(*app_ptr);
}

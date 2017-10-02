#include "cppurses.hpp"

using namespace cppurses;
int main() {
    System sys;

    // Widget Creation
    Vertical_layout vert_layout;

    auto& stack = vert_layout.make_child<Widget_stack>();
    auto& textbox1 = stack.make_page<Textbox>("Textbox 1");
    auto& textbox2 = stack.make_page<Textbox>("Textbox 2");
    auto& textbox3 = stack.make_page<Textbox>("Textbox 3");
    auto& textbox4 = stack.make_page<Textbox>("Textbox 4");
    stack.set_active_page(0);

    auto& horiz_layout = vert_layout.make_child<Horizontal_layout>();
    auto& button1 = horiz_layout.make_child<Push_button>("Page 1");
    auto& button2 = horiz_layout.make_child<Push_button>("Page 2");
    auto& button3 = horiz_layout.make_child<Push_button>("Page 3");
    auto& button4 = horiz_layout.make_child<Push_button>("Page 4");

    // Signals/Slots
    button1.clicked.connect(slot::set_active_page(stack, 0));
    button2.clicked.connect(slot::set_active_page(stack, 1));
    button3.clicked.connect(slot::set_active_page(stack, 2));
    button4.clicked.connect(slot::set_active_page(stack, 3));

    // Aesthetics
    enable_border(stack);
    stack.height_policy.stretch(5);

    set_background(textbox1, Color::Gray);
    set_background(textbox2, Color::Yellow);
    set_background(textbox3, Color::White);
    set_foreground(textbox3, Color::Black);
    set_background(textbox4, Color::Violet);

    set_background(button1, Color::Light_blue);
    enable_border(button1);
    disable_walls(button1.border);
    disable_corners(button1.border);
    button1.border.east_enabled = true;

    set_background(button2, Color::Light_blue);
    enable_border(button2);
    disable_walls(button2.border);
    disable_corners(button2.border);
    button2.border.east_enabled = true;

    set_background(button3, Color::Light_blue);
    enable_border(button3);
    disable_walls(button3.border);
    disable_corners(button3.border);
    button3.border.east_enabled = true;

    set_background(button4, Color::Light_blue);

    sys.set_head(&vert_layout);
    return sys.run();
}

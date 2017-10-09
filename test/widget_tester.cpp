#include "cppurses.hpp"

using namespace cppurses;
int main() {
    System sys;

    Vertical_layout vl;
    Push_button& button = vl.make_child<Push_button>("Change Status");
    set_background(button, Color::White);
    set_foreground(button, Color::Black);
    button.set_alignment(Alignment::Center);

    Status_bar& bar = vl.make_child<Status_bar>("Initial thing");
    bar.set_alignment(Alignment::Center);

    button.clicked.connect(slot::update_status(bar, "A New S-t-a-t-u-s !"));

    sys.set_head(&vl);
    return sys.run();
}

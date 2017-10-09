#include "cppurses.hpp"

using namespace cppurses;
int main() {
    System sys;

    Vertical_layout vl;
    Push_button& button = vl.make_child<Push_button>("Change Status");
    set_background(button, Color::White);
    set_foreground(button, Color::Black);
    button.set_alignment(Alignment::Center);

    Log& log = vl.make_child<Log>();

    int i{0};
    sig::Slot<void()> increment{
        [&i, &log] { log.post_message(std::to_string(i++)); }};

    button.clicked.connect(increment);

    sys.set_head(&vl);
    return sys.run();
}

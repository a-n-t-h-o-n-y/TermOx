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

    Cycle_box& cb = vl.make_child<Cycle_box>("Switch thing");
    cb.add_option("option 1");
    cb.add_option("option 2");
    cb.add_option("o-p-t-i-o-n 3");
    // set_background(cb.label, Color::Light_blue);

    Text_display& cb_tb = vl.make_child<Text_display>();
    cb.option_changed.connect(slot::set_text(cb_tb));

    sys.set_head(&vl);
    return sys.run();
}

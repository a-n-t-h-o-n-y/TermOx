#include "cppurses.hpp"

using namespace cppurses;

class Textbox_holder : public Vertical_layout {
   public:
    Textbox_holder() { enable_border(*this); }
};

int main() {
    System sys;

    Vertical_layout vl;
    enable_border(vl);
    Push_button& button = vl.make_child<Push_button>("Change Status");
    set_background(button, Color::White);
    set_foreground(button, Color::Black);
    button.set_alignment(Alignment::Center);
    enable_border(button);

    Log& log = vl.make_child<Log>();

    int i{0};
    sig::Slot<void()> increment{
        [&i, &log] { log.post_message(std::to_string(i++)); }};

    button.clicked.connect(increment);

    Horizontal_layout& ai_layout{vl.make_child<Horizontal_layout>()};
    ai_layout.height_policy.type(Size_policy::Fixed);
    ai_layout.height_policy.hint(1);

    Cycle_box& bai = ai_layout.make_child<Cycle_box>("Black AI");
    bai.add_option("Human");
    bai.add_option("Random");
    bai.add_option("Easy");
    bai.add_option("Medium");
    bai.add_option("Hard");

    Cycle_box& wai = ai_layout.make_child<Cycle_box>("White AI");
    wai.add_option("Human");
    wai.add_option("Random");
    wai.add_option("Easy");
    wai.add_option("Medium");
    wai.add_option("Hard");
    disable_walls(wai.border);
    disable_corners(wai.border);
    wai.border.west_enabled = true;
    enable_border(wai);

    auto& tb = vl.make_child<Textbox>("Hello");
    tb.height_policy.type(Size_policy::Fixed);
    tb.height_policy.hint(3);
    enable_border(tb);

    sys.set_head(&vl);

    // Textbox_holder th;
    // th.make_child<Textbox>();
    // sys.set_head(&th);

    return sys.run();
}

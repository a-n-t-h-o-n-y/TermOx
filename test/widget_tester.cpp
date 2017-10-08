#include "cppurses.hpp"

using namespace cppurses;
int main() {
    System sys;

    Horizontal_layout hl;
    Label& label = hl.make_child<Label>("A Title that is very long");
    set_background(label, Color::White);
    set_foreground(label, Color::Black);

    label.set_alignment(Alignment::Right);

    sys.set_head(&hl);
    return sys.run();
}

#include <twidgets.hpp>
#include <functional>

class Bordered : public twf::Horizontal_layout {
   public:
    Bordered() {
        this->make_child<twf::Textbox>();
        this->enable_border();
    }
};

class active_tb : public twf::Text_display {
   public:
    active_tb(const twf::Glyph_string& gs = "") : twf::Text_display{gs} {
        this->enable_border();
    }

   protected:
    bool mouse_press_event(const twf::Mouse_event& event) override {
        if (event.button() == twf::Mouse_button::ScrollUp) {
            this->scroll_up(1);
        } else if (event.button() == twf::Mouse_button::ScrollDown) {
            this->scroll_down(1);
        }
        // this->erase(0, 1);
        // this->pop_back();
        this->update();
        return Widget::mouse_press_event(event);
    }
};

int main() {
    twf::System s;

    twf::Vertical_layout w;
    w.make_child<twf::Titlebar>("Text_display widget");
    auto& btb = w.make_child<twf::Textbox>();
    btb.enable_border();
    auto& pb = w.make_child<twf::Push_button>("Toggle Word Wrap below.");
    pb.geometry().size_policy().vertical_policy = twf::Size_policy::Fixed;
    pb.geometry().set_height_hint(3);
    pb.set_background(twf::Color::Red);
    auto& tb = w.make_child<twf::Textbox>(
        "The diddle-check seems like it's degenerated into the girls all "
        "getting very excited and exchanging data on what kinds of animal "
        "members of their own biologic families either imitate or physically "
        "resemble, and Avril's out of sight and silent and apparently letting "
        "them go with it for a while and vent stress. Hal keeps checking for "
        "jaw-drool with the back of his hand. Pemulis, in a Cyrillic-lettered "
        "T-shirt, takes off\n\n\n\n\n\nthe hat and looks around himself and "
        "makes "
        "reflexive tiestraightening movements, taking one last look at his\n"
        "lines on the printout while Axford stands there needing three tries "
        "to work the outside door's knob. Ann Kittenplan, on the other hand, "
        "wears an expression\nof almost regal calm, and precedes them through "
        "the inner door like someone stepping down off a dais.");
    tb.enable_border();
    // tb.disable_scrolling();
    // tb.disable_word_wrap();
    pb.clicked.connect([&tb] { tb.enable_word_wrap(!tb.does_word_wrap()); });

    s.set_head(&w);

    return s.run();
}

// int main() {
//     twf::System s;

//     twf::Horizontal_layout w;
//     auto& ckbx = w.make_child<twf::Checkbox>();
//     auto& vl = w.make_child<twf::Vertical_layout>();
//     vl.make_child<twf::Titlebar>("~ Checkbox and Textbox ~");
//     auto& tb = vl.make_child<twf::Textbox>();
//     tb.enable_border();
//     auto change_color = [&tb](twf::Color c) {
//         tb.brush().set_background(c);
//         tb.update();
//     };
//     ckbx.checked.connect(std::bind(change_color, twf::Color::Light_blue));
//     ckbx.unchecked.connect(std::bind(change_color, twf::Color::Light_green));
//     auto& sb = vl.make_child<twf::Horizontal_scrollbar>();
//     sb.left.connect(std::bind(change_color, twf::Color::Brown));
//     sb.right.connect(std::bind(change_color, twf::Color::Violet));

//     s.set_head(&w);
//     return s.run();
// }

// int main() {
//     twf::System s;

//     twf::Border b;
//     b.enable();
//     b.disable_corners();
//     b.enable_north_west();
//     b.enable_south_east();
//     b.enable_south_west();
//     b.disable_east();
//     b.disable_north();
//     b.set_north_west("│");
//     b.set_south_east("─");

//     Bordered widg;
//     widg.border() = b;
//     auto& c = widg.make_child<Bordered>();
//     c.border() = b;

//     s.set_head(&widg);
//     return s.run();
// }

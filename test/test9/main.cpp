#include <twidgets.hpp>
#include <functional>
#include <sstream>
#include <random>
#include <string>
#include <cstdint>
#include <codecvt>
#include <locale>

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
            this->scroll_up_(1);
        } else if (event.button() == twf::Mouse_button::ScrollDown) {
            this->scroll_down_(1);
        }
        // this->erase(0, 1);
        // this->pop_back();
        this->update();
        return Widget::mouse_press_event(event);
    }
};

twf::Color random_color() {
    std::random_device rd;
    std::mt19937 mt(rd());
    std::uniform_int_distribution<std::int16_t> dist(240, 256);
    auto value = dist(mt);
    return static_cast<twf::Color>(value);
}

std::string random_unicode() {
    std::random_device rd;
    std::mt19937 mt(rd());
    std::uniform_int_distribution<std::int32_t> dist(32, 65536);
    char32_t wc = static_cast<char32_t>(dist(mt));
    std::wstring_convert<std::codecvt_utf8<char32_t>, char32_t> converter;
    return converter.to_bytes(wc);
}

int main() {
    twf::System s;

    twf::Vertical_layout w;
    w.make_child<twf::Titlebar>("Text_display widget");
    auto& hl_2 = w.make_child<twf::Horizontal_layout>();
    auto& tb_1 = hl_2.make_child<twf::Textbox>();
    tb_1.enable_border();
    auto& tb_3 = hl_2.make_child<twf::Textbox>();
    tb_3.disable_border();
    tb_3.disable_word_wrap();
    tb_3.set_background(twf::Color::Dark_red);
    auto& hl = w.make_child<twf::Horizontal_layout>();
    hl.geometry().size_policy().vertical_policy = twf::Size_policy::Fixed;
    hl.geometry().set_height_hint(3);
    auto& pb_ww = hl.make_child<twf::Push_button>("Toggle Word Wrap Below");
    pb_ww.set_background(twf::Color::Red);
    auto& pb_del = hl.make_child<twf::Push_button>("Delete Textbox Above");
    pb_del.set_background(twf::Color::Blue);
    // pb_del.clicked.connect(tb_1.delete_later);
    pb_del.clicked.connect([&tb_1] {
        tb_1.size_policy().horizontal_policy = twf::Size_policy::Fixed;
        tb_1.geometry().set_width_hint(4);
    });
    auto& tb_2 = w.make_child<twf::Textbox>(
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
    tb_2.enable_border();
    tb_2.cursor_moved.connect([st = tb_1.set_text](twf::Coordinate c) {
        std::stringstream ss;
        ss << "x: " << c.x << '\n' << "y: " << c.y;
        st(ss.str());
    });
    tb_2.scrolled.connect([&tb_2] { tb_2.set_foreground(random_color()); });
    tb_2.cursor_moved.connect([append = tb_3.append](twf::Coordinate) {
        append(random_unicode());
    });
    // tb_2.disable_scrolling();
    // tb_2.disable_word_wrap();
    pb_ww.clicked.connect(tb_2.toggle_word_wrap);

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

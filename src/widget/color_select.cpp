#include <cppurses/widget/widgets/color_select.hpp>

#include <signals/signals.hpp>

#include <cppurses/painter/color.hpp>
#include <cppurses/widget/widget.hpp>
#include <cppurses/widget/widgets/push_button.hpp>

namespace cppurses {

Color_select::Color_select() {
    this->Color_select::initialize();
}

void Color_select::initialize() {
    this->width_policy.fixed(16);
    this->height_policy.fixed(2);

    black_.brush.set_background(Color::Black);
    dark_red_.brush.set_background(Color::Dark_red);
    dark_blue_.brush.set_background(Color::Dark_blue);
    dark_gray_.brush.set_background(Color::Dark_gray);
    brown_.brush.set_background(Color::Brown);
    green_.brush.set_background(Color::Green);
    red_.brush.set_background(Color::Red);
    gray_.brush.set_background(Color::Gray);
    blue_.brush.set_background(Color::Blue);
    orange_.brush.set_background(Color::Orange);
    light_gray_.brush.set_background(Color::Light_gray);
    light_green_.brush.set_background(Color::Light_green);
    violet_.brush.set_background(Color::Violet);
    light_blue_.brush.set_background(Color::Light_blue);
    yellow_.brush.set_background(Color::Yellow);
    white_.brush.set_background(Color::White);

    black_.clicked.connect([this] { color_changed(Color::Black); });
    dark_red_.clicked.connect([this] { color_changed(Color::Dark_red); });
    dark_blue_.clicked.connect([this] { color_changed(Color::Dark_blue); });
    dark_gray_.clicked.connect([this] { color_changed(Color::Dark_gray); });
    brown_.clicked.connect([this] { color_changed(Color::Brown); });
    green_.clicked.connect([this] { color_changed(Color::Green); });
    red_.clicked.connect([this] { color_changed(Color::Red); });
    gray_.clicked.connect([this] { color_changed(Color::Gray); });
    blue_.clicked.connect([this] { color_changed(Color::Blue); });
    orange_.clicked.connect([this] { color_changed(Color::Orange); });
    light_gray_.clicked.connect([this] { color_changed(Color::Light_gray); });
    light_green_.clicked.connect([this] { color_changed(Color::Light_green); });
    violet_.clicked.connect([this] { color_changed(Color::Violet); });
    light_blue_.clicked.connect([this] { color_changed(Color::Light_blue); });
    yellow_.clicked.connect([this] { color_changed(Color::Yellow); });
    white_.clicked.connect([this] { color_changed(Color::White); });
}

}  // namespace cppurses

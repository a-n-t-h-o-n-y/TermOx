#include "widget/widgets/color_chooser.hpp"

namespace cppurses {

Color_chooser::Color_chooser() {
    this->Color_chooser::initialize();
}

void Color_chooser::initialize() {
    black_.set_background(Color::Black);
    dark_red_.set_background(Color::Dark_red);
    dark_blue_.set_background(Color::Dark_blue);
    dark_gray_.set_background(Color::Dark_gray);
    brown_.set_background(Color::Brown);
    green_.set_background(Color::Green);
    red_.set_background(Color::Red);
    gray_.set_background(Color::Gray);
    blue_.set_background(Color::Blue);
    orange_.set_background(Color::Orange);
    light_gray_.set_background(Color::Light_gray);
    light_green_.set_background(Color::Light_green);
    violet_.set_background(Color::Violet);
    light_blue_.set_background(Color::Light_blue);
    yellow_.set_background(Color::Yellow);
    white_.set_background(Color::White);

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

    this->geometry().set_width_hint(16);
    this->size_policy().horizontal_policy = Size_policy::Fixed;
    this->geometry().set_height_hint(2);
    this->size_policy().vertical_policy = Size_policy::Fixed;
}
}  // namespace cppurses

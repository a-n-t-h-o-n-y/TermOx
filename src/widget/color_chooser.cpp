#include "widget/widgets/color_chooser.hpp"
#include "painter/color.hpp"
#include "widget/size_policy.hpp"
#include "widget/widget.hpp"
#include "widget/widgets/push_button.hpp"
#include <signals/signals.hpp>

namespace cppurses {

Color_chooser::Color_chooser() {
    this->Color_chooser::initialize();
}

void Color_chooser::initialize() {
    this->width_policy.hint(16);
    this->width_policy.type(Size_policy::Fixed);
    this->height_policy.hint(2);
    this->height_policy.type(Size_policy::Fixed);

    set_background(black_, Color::Black);
    set_background(dark_red_, Color::Dark_red);
    set_background(dark_blue_, Color::Dark_blue);
    set_background(dark_gray_, Color::Dark_gray);
    set_background(brown_, Color::Brown);
    set_background(green_, Color::Green);
    set_background(red_, Color::Red);
    set_background(gray_, Color::Gray);
    set_background(blue_, Color::Blue);
    set_background(orange_, Color::Orange);
    set_background(light_gray_, Color::Light_gray);
    set_background(light_green_, Color::Light_green);
    set_background(violet_, Color::Violet);
    set_background(light_blue_, Color::Light_blue);
    set_background(yellow_, Color::Yellow);
    set_background(white_, Color::White);

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

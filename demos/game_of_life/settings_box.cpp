#include "settings_box.hpp"

#include <cctype>
#include <chrono>
#include <string>

#include <cppurses/painter/color.hpp>
#include <cppurses/painter/glyph.hpp>
#include <cppurses/widget/size_policy.hpp>
#include <cppurses/widget/widgets/text_display.hpp>

using namespace cppurses;
namespace gol {

Clear_step_box::Clear_step_box() {
    this->height_policy.type(Size_policy::Fixed);
    this->height_policy.hint(2);
    clear_btn.title_btn_.brush.set_background(Color::Gray);
    step_btn.brush.set_background(Color::Light_gray);
    step_btn.brush.set_foreground(Color::Black);
}

Rule_edit::Rule_edit() {
    this->height_policy.type(Size_policy::Fixed);
    this->height_policy.hint(2);

    edit_box.brush.set_background(Color::White);
    edit_box.brush.set_foreground(Color::Black);
    edit_box.set_ghost_color(Color::Dark_gray);

    edit_box.set_validator([](char c) { return std::isdigit(c) || c == '/'; });
    edit_box.editing_finished.connect(
        [this](std::string rule_text) { rule_change(rule_text); });

    label.set_alignment(Alignment::Center);
}

Start_pause_btns::Start_pause_btns() : Toggle_button{"Start", "Pause"} {
    this->height_policy.type(Size_policy::Fixed);
    this->height_policy.hint(1);
    first_btn.brush.set_background(Color::Light_green);
    first_btn.brush.set_foreground(Color::Black);
    second_btn.brush.set_background(Color::Gray);
    second_btn.brush.set_foreground(Color::White);
}

Period_box::Period_box() {
    units.brush.set_background(Color::White);
    units.brush.set_foreground(Color::Gray);
    units.width_policy.type(Size_policy::Fixed);
    units.width_policy.hint(2);
    this->height_policy.type(Size_policy::Fixed);
    this->height_policy.hint(1);
}

Grid_fade::Grid_fade() {
    fade_box.toggle();
    this->height_policy.type(Size_policy::Fixed);
    this->height_policy.hint(1);
}

Settings_box::Settings_box() {
    this->height_policy.type(Size_policy::Fixed);
    this->height_policy.hint(8);

    this->border.enabled = true;
    disable_corners(this->border);
    disable_walls(this->border);
    this->border.north_enabled = true;
    this->border.north = Glyph{L'─', foreground(Color::Blue)};

    period_edit.value_set.connect(
        [this](int value) { period_set(std::chrono::milliseconds{value}); });
}
}  // namespace gol

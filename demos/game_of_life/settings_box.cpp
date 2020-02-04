#include "settings_box.hpp"

#include <cctype>
#include <chrono>
#include <string>

#include <cppurses/painter/color.hpp>
#include <cppurses/painter/glyph.hpp>
#include <cppurses/widget/widgets/text_display.hpp>

using namespace cppurses;
namespace gol {

Clear_step_box::Clear_step_box()
{
    this->height_policy.fixed(2);
    clear_btn.main_btn.brush.set_background(Color::Gray);
    step_btn.brush.set_background(Color::Light_gray);
    step_btn.brush.set_foreground(Color::Black);
}

Rule_edit::Rule_edit()
{
    this->height_policy.fixed(2);

    edit_box.brush.set_background(Color::White);
    edit_box.brush.set_foreground(Color::Black);
    edit_box.set_ghost_color(Color::Dark_gray);

    edit_box.set_validator([](char c) { return std::isdigit(c) or c == '/'; });
    edit_box.edit_finished.connect(
        [this](std::string rule_text) { rule_change(rule_text); });

    label.set_alignment(Alignment::Center);
}

Start_pause_btns::Start_pause_btns() : Toggle_button{"Start", "Pause"}
{
    this->height_policy.fixed(1);
    top.brush.set_background(Color::Light_green);
    top.brush.set_foreground(Color::Black);
    bottom.brush.set_background(Color::Gray);
    bottom.brush.set_foreground(Color::White);
}

Period_box::Period_box()
{
    this->height_policy.fixed(1);
    units.brush.set_background(Color::White);
    units.brush.set_foreground(Color::Gray);
    units.width_policy.fixed(2);
}

Grid_fade::Grid_fade()
{
    fade_box.toggle();
    this->height_policy.fixed(1);
}

Settings_box::Settings_box()
{
    this->height_policy.fixed(8);
    this->border.enable();
    this->border.segments.disable_all();
    this->border.segments.north.enable();
    this->border.segments.north = L'─'_g | foreground(Color::Blue);

    period_edit.value_set.connect(
        [this](int value) { period_set(std::chrono::milliseconds{value}); });
}
}  // namespace gol

#include "settings_box.hpp"

#include <cctype>
#include <chrono>
#include <string>

#include <cppurses/painter/color.hpp>
#include <cppurses/widget/size_policy.hpp>

namespace gol {

Clear_step_box::Clear_step_box() {
    this->height_policy.type(cppurses::Size_policy::Fixed);
    this->height_policy.hint(2);
}

template <typename Number_t>
Labeled_number_edit<Number_t>::Labeled_number_edit(
    const std::string& description,
    Number_t initial)
    : label{this->make_child<cppurses::Label>(description)},
      edit_box{this->make_child<cppurses::Number_edit<Number_t>>(initial)} {
    this->height_policy.type(cppurses::Size_policy::Fixed);
    this->height_policy.hint(1);
    label.width_policy.type(cppurses::Size_policy::Fixed);
    label.width_policy.hint(11);

    edit_box.brush.set_background(cppurses::Color::White);
    edit_box.brush.set_foreground(cppurses::Color::Black);
    edit_box.set_ghost_color(cppurses::Color::Dark_gray);
}

Rule_edit::Rule_edit() {
    this->height_policy.type(cppurses::Size_policy::Fixed);
    this->height_policy.hint(2);

    edit_box.brush.set_background(cppurses::Color::White);
    edit_box.brush.set_foreground(cppurses::Color::Black);
    edit_box.set_ghost_color(cppurses::Color::Dark_gray);

    edit_box.set_validator([](char c) { return std::isdigit(c) || c == '/'; });
    edit_box.editing_finished.connect([this](std::string rule_text) {
        if (!rule_text.empty()) {
            auto div = rule_text.find('/');
            if (div != std::string::npos) {
                const std::string birth{rule_text.substr(0, div)};
                const std::string survival{rule_text.substr(div + 1)};
                rule_change(birth, survival);
            }
        }
    });
}

Start_stop_btns::Start_stop_btns() {
    this->height_policy.type(cppurses::Size_policy::Fixed);
    this->height_policy.hint(1);
    start_btn.brush.set_background(cppurses::Color::Blue);
    start_btn.brush.set_foreground(cppurses::Color::White);
    stop_btn.brush.set_background(cppurses::Color::White);
    stop_btn.brush.set_foreground(cppurses::Color::Blue);
}

Settings_box::Settings_box() {
    this->height_policy.type(cppurses::Size_policy::Fixed);
    this->height_policy.hint(8);
    grid_box.brush.set_background(cppurses::Color::Light_gray);
    grid_box.brush.set_foreground(cppurses::Color::Black);
    fade_box.toggle();

    period_edit.value_set.connect(
        [this](int value) { period_set(std::chrono::milliseconds{value}); });
}

}  // namespace gol

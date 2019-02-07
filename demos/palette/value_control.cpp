#include "value_control.hpp"

#include <cctype>
#include <string>

#include <cppurses/painter/color.hpp>
#include <cppurses/widget/size_policy.hpp>

#include <cppurses/widget/widget_free_functions.hpp>

using namespace cppurses;

namespace palette {
namespace detail {
Buffered_edit_box::Buffered_edit_box() {
    this->height_policy.type(Size_policy::Fixed);
    this->height_policy.hint(1);
    this->box.set_validator([](char c) { return std::isdigit(c); });
    this->box.set_ghost_color(cppurses::Color::White);
    set_background_recursive(*this, cppurses::Color::Blue);
}
}  // namespace detail

// Sets up value_edit_ and slider_ to be consistent with each other.
Value_control::Value_control(const Glyph_string& label)
    : label_{this->make_child<cppurses::Label>(label)} {
    this->width_policy.type(Size_policy::Maximum);
    this->width_policy.hint(5);
    auto& slider = this->slider_.slider;
    auto& edit_box = this->value_edit_.box;

    value_edit_.box.editing_finished.connect(
        [&slider, &edit_box](std::string value_str) {
            if (value_str.empty()) {
                slider.set_value(0);
            } else {
                slider.set_value(std::stoi(value_str));
            }
            edit_box.set_contents(std::to_string(slider.value()));
            edit_box.set_cursor(edit_box.contents().size());
        });

    slider.value_changed.connect([&edit_box](int value) {
        edit_box.set_contents(std::to_string(value));
        edit_box.set_cursor(edit_box.contents().size());
    });

    label_.set_alignment(cppurses::Alignment::Center);
}
}  // namespace palette

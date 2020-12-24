#include "value_control.hpp"

#include <cctype>
#include <string>

#include <termox/painter/color.hpp>
#include <termox/widget/layouts/horizontal.hpp>

using namespace ox;

namespace palette::detail {

Buffered_edit_box::Buffered_edit_box()
{
    this->height_policy.fixed(1);
    this->box.set_validator([](char c) { return std::isdigit(c); });
    this->box.set_ghost_color(Color::White);
    for (auto& child : this->get_children())
        child.brush.background = Color::Blue;
}
}  // namespace palette::detail

namespace palette {

// Set up value_edit_ and slider_ to be consistent with each other.
Value_control::Value_control(const Glyph_string& label)
    : label_{this->make_child<HLabel>(label)}
{
    this->width_policy.maximum(5);
    auto& slider   = this->slider_.slider;
    auto& edit_box = this->value_edit_.box;

    value_edit_.box.edit_finished.connect(
        [&slider, &edit_box](std::string value_str) {
            if (value_str.empty())
                slider.set_value(0);
            else
                slider.set_value(std::stoi(value_str));
            edit_box.set_contents(std::to_string(slider.value()));
            edit_box.set_cursor(edit_box.contents().size());
        });

    slider.value_changed.connect([&edit_box](int value) {
        edit_box.set_contents(std::to_string(value));
        edit_box.set_cursor(edit_box.contents().size());
    });

    label_.set_alignment(Align::Center);
}
}  // namespace palette

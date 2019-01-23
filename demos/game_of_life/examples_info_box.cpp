#include "examples_info_box.hpp"

#include <cppurses/painter/color.hpp>
#include <cppurses/widget/size_policy.hpp>

namespace gol {

Examples_box::Examples_box() {
    to_info_btn.height_policy.type(cppurses::Size_policy::Fixed);
    to_info_btn.height_policy.hint(1);
    to_info_btn.brush.set_background(cppurses::Color::Light_gray);
    to_info_btn.brush.set_foreground(cppurses::Color::Black);
}

Info_box::Info_box() {
    to_examples_btn.height_policy.type(cppurses::Size_policy::Fixed);
    to_examples_btn.height_policy.hint(1);
    to_examples_btn.brush.set_background(cppurses::Color::Light_gray);
    to_examples_btn.brush.set_foreground(cppurses::Color::Black);
}

Examples_info_box::Examples_info_box() {
    this->set_active_page(0);
    examples.to_info_btn.clicked.connect(
        [this]() { this->set_active_page(1); });
    info.to_examples_btn.clicked.connect(
        [this]() { this->set_active_page(0); });
}
}  // namespace gol

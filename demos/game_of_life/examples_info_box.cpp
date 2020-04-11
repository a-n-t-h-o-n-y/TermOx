#include "examples_info_box.hpp"

#include <cppurses/painter/color.hpp>

using namespace cppurses;
namespace gol {

Examples_box::Examples_box()
{
    to_info_btn.height_policy.fixed(1);
    to_info_btn.brush.set_background(Color::Light_gray);
    to_info_btn.brush.set_foreground(Color::Black);
}

Info_box::Info_box()
{
    to_examples_btn.height_policy.fixed(1);
    to_examples_btn.brush.set_background(Color::Light_gray);
    to_examples_btn.brush.set_foreground(Color::Black);
}

Examples_info_box::Examples_info_box()
{
    this->set_active_page(0);
    examples.to_info_btn.pressed.connect(
        [this]() { this->set_active_page(1); });
    info.to_examples_btn.pressed.connect(
        [this]() { this->set_active_page(0); });
}
}  // namespace gol

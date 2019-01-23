#include "files_box.hpp"

#include <string>

#include <cppurses/painter/color.hpp>
#include <cppurses/widget/size_policy.hpp>
#include <cppurses/widget/widgets/confirm_button.hpp>

namespace gol {

File_widget::File_widget(const std::string& btn_text)
    : confirm_btn_{this->make_child<cppurses::Confirm_button>(btn_text)} {
    confirm_btn_.clicked.connect(
        [this]() { filename_given(filename_box_.contents().str()); });
    confirm_btn_.title_btn_.brush.set_background(cppurses::Color::White);
    confirm_btn_.title_btn_.brush.set_foreground(cppurses::Color::Dark_gray);
}

Files_box::Files_box() {
    this->height_policy.type(cppurses::Size_policy::Fixed);
    this->height_policy.hint(4);
}
}  // namespace gol

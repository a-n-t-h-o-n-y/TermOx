#include "files_box.hpp"

#include <string>

#include <cppurses/painter/color.hpp>
#include <cppurses/widget/size_policy.hpp>
#include <cppurses/widget/widgets/confirm_button.hpp>

using namespace cppurses;
namespace gol {

File_widget::File_widget(const std::string& btn_text)
    : confirm_btn_{this->make_child<Confirm_button>(btn_text)} {
    confirm_btn_.clicked.connect(
        [this]() { filename_given(filename_box_.contents().str()); });
    confirm_btn_.title_btn_.brush.set_background(Color::White);
    confirm_btn_.title_btn_.brush.set_foreground(Color::Dark_gray);
}

Files_box::Files_box() {
    this->height_policy.type(Size_policy::Fixed);
    this->height_policy.hint(5);
    this->border.enabled = true;
    disable_corners(this->border);
    disable_walls(this->border);
    this->border.north_enabled = true;
    this->border.north = Glyph{L'─', foreground(Color::Blue)};
}
}  // namespace gol

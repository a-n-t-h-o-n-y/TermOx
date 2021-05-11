#include <termox/widget/widgets/read_file.hpp>

#include <termox/painter/color.hpp>
#include <termox/widget/pipe.hpp>

namespace ox::detail {

Read_file_widgets::Read_file_widgets()
    : HTuple<Button, Tile, Line_edit>{
          {U"Open"},
          {U'>'},
          {U"Filename" | fg(Color::Dark_gray), ox::Align::Left,
           ox::Line_edit::Action::Clear}}
{
    using namespace ox::pipe;
    *this | fixed_height(1);
    open_btn | fixed_width(4) | bg(Color::Blue);
    filename_edit | bg(Color::White) | fg(Color::Black);
}

}  // namespace ox::detail

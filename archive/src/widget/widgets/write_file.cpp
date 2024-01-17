#include <termox/widget/widgets/write_file.hpp>

#include <termox/painter/color.hpp>
#include <termox/widget/pipe.hpp>
#include <termox/widget/widgets/button.hpp>
#include <termox/widget/widgets/line_edit.hpp>
#include <termox/widget/widgets/tile.hpp>

namespace ox::detail {

Write_file_widgets::Write_file_widgets()
    : HTuple<Button, Tile, Line_edit>{
          {U"Save"},
          {U'>'},
          {U"Filename" | fg(Color::Dark_gray), ox::Align::Left,
           ox::Line_edit::Action::Clear}}
{
    using namespace ox::pipe;
    *this | fixed_height(1);
    save_btn | fixed_width(4) | bg(Color::Blue);
    filename_edit | bg(Color::White) | fg(Color::Black);
}

}  // namespace ox::detail

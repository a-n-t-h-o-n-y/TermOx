#include <termox/widget/widgets/color_select.hpp>

#include <string>
#include <utility>

#include <termox/painter/color.hpp>
#include <termox/terminal/terminal.hpp>
#include <termox/widget/pipe.hpp>

namespace ox {

Color_tile::Color_tile(Color c, Display display)
    : Button{display == Display::None ? "" : std::to_string(c.value)}
{
    *this | bg(c);
}

Color_tile::Color_tile(Parameters p) : Color_tile{p.color, p.display} {}

Color_select::Color_select(Color_tile::Display display) : display_{display}
{
    this->set_palette(Terminal::current_palette());
    Terminal::palette_changed.connect(
        [this](auto const& pal) { this->set_palette(pal); });
}

Color_select::Color_select(Parameters p) : Color_select{p.display} {}

void Color_select::set_palette(Palette const& pal)
{
    this->delete_all_children();
    auto const size           = pal.size();
    auto constexpr row_length = 8uL;
    auto count                = 0uL;

    while (count != size) {
        auto& color_line = this->make_child();
        for (auto i = 0uL; i < row_length && count != size; ++i, ++count) {
            auto const color = pal[count].color;
            color_line.make_child(color, display_) |
                pipe::on_press([this, color] { color_selected.emit(color); });
        }
    }
}

auto color_select(Color_tile::Display display) -> std::unique_ptr<Color_select>
{
    return std::make_unique<Color_select>(display);
}

auto color_select(Color_select::Parameters p) -> std::unique_ptr<Color_select>
{
    return std::make_unique<Color_select>(std::move(p));
}

}  // namespace ox

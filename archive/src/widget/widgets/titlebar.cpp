#include <termox/widget/widgets/titlebar.hpp>

#include <memory>
#include <utility>

#include <termox/painter/color.hpp>
#include <termox/painter/glyph_string.hpp>
#include <termox/system/system.hpp>
#include <termox/widget/align.hpp>
#include <termox/widget/pair.hpp>
#include <termox/widget/pipe.hpp>
#include <termox/widget/widgets/button.hpp>
#include <termox/widget/widgets/label.hpp>

namespace ox {

Titlebar::Titlebar(Glyph_string title_text)
    : HPair<HLabel, Button>{
          {std::move(title_text), Align::Center, extra_left, extra_right},
          {U"│✕ "}}

{
    using namespace ox::pipe;
    *this | fixed_height(1);
    *this | children() | bg(Color::White) | fg(Color::Black);
    exit_button | fixed_width(exit_width) | on_press(System::quit);
}

Titlebar::Titlebar(Parameters p) : Titlebar{std::move(p.title_text)} {}

auto titlebar(Glyph_string title_text) -> std::unique_ptr<Titlebar>
{
    return std::make_unique<Titlebar>(std::move(title_text));
}

auto titlebar(Titlebar::Parameters p) -> std::unique_ptr<Titlebar>
{
    return std::make_unique<Titlebar>(std::move(p));
}

}  // namespace ox

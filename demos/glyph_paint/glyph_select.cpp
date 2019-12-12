#include "glyph_select.hpp"

#include <cmath>
#include <utility>

#include <signals/signal.hpp>

#include <cppurses/painter/glyph_string.hpp>
#include <cppurses/widget/layouts/horizontal.hpp>
#include <cppurses/widget/widget.hpp>
#include <cppurses/widget/widgets/horizontal_slider.hpp>
#include <cppurses/widget/widgets/push_button.hpp>

#include "glyph_select_stack.hpp"

using namespace cppurses;
namespace demos {
namespace glyph_paint {

Glyph_select::Bottom_row::Bottom_row()
{
    this->height_policy.fixed(1);
    left_btn.brush.set_background(Color::Light_gray);
    left_btn.brush.set_foreground(Color::Black);
    left_btn.width_policy.fixed(1);
    right_btn.brush.set_background(Color::Light_gray);
    right_btn.brush.set_foreground(Color::Black);
    right_btn.width_policy.fixed(1);
}

Glyph_select::Glyph_select(Glyph_string symbols)
{
    glyph_stack.set_symbols(std::move(symbols));

    glyph_stack.glyph_selected.connect(
        [this](Glyph glyph) { this->glyph_selected(std::move(glyph)); });

    bottom_row.left_btn.clicked.connect([this] { this->flip_page_backward(); });
    bottom_row.right_btn.clicked.connect([this] { this->flip_page_forward(); });
    bottom_row.slider.scrolled_up.connect(
        [this] { this->flip_page_backward(); });
    bottom_row.slider.scrolled_down.connect(
        [this] { this->flip_page_forward(); });
    bottom_row.slider.percent_changed.connect(
        slot::set_page_percent(glyph_stack));
}

void Glyph_select::set_symbols(Glyph_string symbols)
{
    glyph_stack.set_symbols(std::move(symbols));
}

std::size_t Glyph_select::current_index()
{
    auto const index = bottom_row.slider.percent() * (glyph_stack.size() - 1);
    return std::round(index);
}

void Glyph_select::flip_page_forward()
{
    if (glyph_stack.size() < 2)
        return;
    auto const index = this->current_index();
    if (index == glyph_stack.size())
        return;
    auto const new_percent =
        static_cast<float>(index + 1) / (glyph_stack.size() - 1);
    bottom_row.slider.set_percent(new_percent);
}

void Glyph_select::flip_page_backward()
{
    if (glyph_stack.size() < 2)
        return;
    auto const index = this->current_index();
    if (index == 0)
        return;
    auto const new_percent =
        static_cast<float>(index - 1) / (glyph_stack.size() - 1);
    bottom_row.slider.set_percent(new_percent);
}

}  // namespace glyph_paint
}  // namespace demos

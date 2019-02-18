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

Glyph_select::Glyph_select(Glyph_string symbols) {
    glyph_stack.set_symbols(std::move(symbols));
    bottom_row.height_policy.fixed(1);
    left_btn.brush.set_background(Color::Light_gray);
    left_btn.brush.set_foreground(Color::Black);
    left_btn.width_policy.fixed(1);
    right_btn.brush.set_background(Color::Light_gray);
    right_btn.brush.set_foreground(Color::Black);
    right_btn.width_policy.fixed(1);

    glyph_stack.glyph_selected.connect(
        [this](Glyph glyph) { this->glyph_selected(std::move(glyph)); });

    slider.percent_changed.connect(slot::set_page_percent(glyph_stack));

    left_btn.clicked.connect([this] { this->flip_page_backward(); });
    right_btn.clicked.connect([this] { this->flip_page_forward(); });
    slider.scrolled_up.connect([this] { this->flip_page_backward(); });
    slider.scrolled_down.connect([this] { this->flip_page_forward(); });
}

void Glyph_select::set_symbols(Glyph_string symbols) {
    glyph_stack.set_symbols(std::move(symbols));
}

std::size_t Glyph_select::current_index() {
    float index = slider.percent() * (glyph_stack.size() - 1);
    return std::round(index);
}

void Glyph_select::flip_page_forward() {
    if (glyph_stack.size() < 2) {
        return;
    }
    std::size_t index{this->current_index()};
    if (index == glyph_stack.size()) {
        return;
    }
    float new_percent{static_cast<float>(index + 1) / (glyph_stack.size() - 1)};
    slider.set_percent(new_percent);
}

void Glyph_select::flip_page_backward() {
    if (glyph_stack.size() < 2) {
        return;
    }
    std::size_t index{this->current_index()};
    if (index == 0) {
        return;
    }
    float new_percent{static_cast<float>(index - 1) / (glyph_stack.size() - 1)};
    slider.set_percent(new_percent);
}

}  // namespace glyph_paint
}  // namespace demos

#include <cppurses/widget/size_policy.hpp>
#include <cppurses/widget/widgets/glyph_select.hpp>

#include <cmath>
#include <utility>

namespace cppurses {

Glyph_select::Glyph_select(Glyph_string symbols) {
    glyph_stack.set_symbols(std::move(symbols));
    bottom_row.height_policy.type(Size_policy::Fixed);
    bottom_row.height_policy.hint(1);
    left_btn.width_policy.type(Size_policy::Fixed);
    left_btn.width_policy.hint(2);
    right_btn.width_policy.type(Size_policy::Fixed);
    right_btn.width_policy.hint(2);

    glyph_stack.glyph_selected.connect(
        [this](Glyph glyph) { this->glyph_selected(std::move(glyph)); });

    slider.percent_changed.connect(
        [this](float percent) { this->set_stack_index_by_percent(percent); });

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

void Glyph_select::set_stack_index_by_percent(float percent) {
    auto index{static_cast<std::size_t>(
        std::round(percent * (glyph_stack.size() - 1)))};
    glyph_stack.set_active_page(index);
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

}  // namespace cppurses

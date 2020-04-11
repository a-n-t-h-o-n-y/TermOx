#include "glyph_select_stack.hpp"

#include <cmath>
#include <cstddef>
#include <utility>

#include <signals/signal.hpp>

#include <cppurses/painter/glyph.hpp>
#include <cppurses/painter/glyph_matrix.hpp>
#include <cppurses/painter/glyph_string.hpp>
#include <cppurses/widget/area.hpp>
#include <cppurses/widget/layouts/stack.hpp>
#include <cppurses/widget/point.hpp>
#include <cppurses/widget/widgets/matrix_display.hpp>

using namespace cppurses;
namespace demos {
namespace glyph_paint {

void Glyph_select_stack::set_symbols(Glyph_string symbols)
{
    symbols_ = std::move(symbols);
    this->update_stack();
}

void Glyph_select_stack::append_symbols(Glyph_string symbols)
{
    symbols_.append(symbols);
    this->update_stack();
}

void Glyph_select_stack::set_page_percent(float percent)
{
    percent_ = percent;
    std::size_t size{this->size()};
    if (size == 0) {
        return;
    }
    --size;
    std::size_t page_index = std::round(percent * size);
    this->set_active_page(page_index);
}

bool Glyph_select_stack::resize_event(Area new_size, Area old_size)
{
    this->update_stack();
    return layout::Stack<Matrix_display>::resize_event(new_size, old_size);
}

void Glyph_select_stack::update_stack()
{
    if (this->width() == 0 or this->height() == 0) {
        return;
    }
    this->clear();

    std::size_t symbols_index{0};
    while (symbols_index < symbols_.size()) {
        Matrix_display& symbol_page{
            this->make_page<Matrix_display>(this->width(), this->height())};
        // Attach to glyph pressed signal in Matrix_display
        symbol_page.mouse_pressed.connect([this, &symbol_page](auto const& m) {
            const Glyph& g{symbol_page.matrix.at(m.local.x, m.local.y)};
            if (g != L' '_g) {
                this->glyph_selected(
                    symbol_page.matrix.at(m.local.x, m.local.y));
            }
        });
        for (std::size_t y{0}; y < this->height(); ++y) {
            for (std::size_t x{0}; x < this->width(); ++x) {
                if (symbols_index < symbols_.size()) {
                    symbol_page.matrix.at(x, y) = symbols_.at(symbols_index);
                    ++symbols_index;
                }
                else
                    goto endloop;
            }
        }
    }
endloop:
    this->set_page_percent(percent_);
    this->update();
}

namespace slot {

sig::Slot<void(float)> set_page_percent(Glyph_select_stack& gss)
{
    sig::Slot<void(float)> slot{
        [&gss](float percent) { gss.set_page_percent(percent); }};
    slot.track(gss.destroyed);
    return slot;
}

sig::Slot<void()> set_page_percent(Glyph_select_stack& gss, float percent)
{
    sig::Slot<void()> slot{[&gss, percent] { gss.set_page_percent(percent); }};
    slot.track(gss.destroyed);
    return slot;
}

}  // namespace slot
}  // namespace glyph_paint
}  // namespace demos

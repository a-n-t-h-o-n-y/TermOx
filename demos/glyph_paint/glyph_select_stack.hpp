#ifndef CPPURSES_DEMOS_GLYPH_PAINT_GLYPH_SELECT_STACK_HPP
#define CPPURSES_DEMOS_GLYPH_PAINT_GLYPH_SELECT_STACK_HPP
#include <signals/signal.hpp>
#include <signals/slot.hpp>

#include <cppurses/painter/glyph_string.hpp>
#include <cppurses/widget/layouts/stack.hpp>
#include <cppurses/widget/widgets/matrix_display.hpp>

namespace cppurses {
struct Area;
struct Glyph;
}  // namespace cppurses

namespace demos {
namespace glyph_paint {

class Glyph_select_stack
    : public cppurses::layout::Stack<cppurses::Matrix_display> {
   public:
    Glyph_select_stack() { this->set_name("Glyph_select_stack"); }
    void set_symbols(cppurses::Glyph_string symbols);
    void append_symbols(cppurses::Glyph_string symbols);

    void set_page_percent(float percent);

    // Signals
    sig::Signal<void(cppurses::Glyph)> glyph_selected;

   protected:
    bool resize_event(cppurses::Area new_size,
                      cppurses::Area old_size) override;

   private:
    cppurses::Glyph_string symbols_;
    float percent_{0.0};

    void update_stack();
};

namespace slot {

sig::Slot<void(float)> set_page_percent(Glyph_select_stack& gss);
sig::Slot<void()> set_page_percent(Glyph_select_stack& gss, float percent);

}  // namespace slot
}  // namespace glyph_paint
}  // namespace demos
#endif  // CPPURSES_DEMOS_GLYPH_PAINT_GLYPH_SELECT_STACK_HPP

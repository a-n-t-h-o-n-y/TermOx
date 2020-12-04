#ifndef CPPURSES_DEMOS_GLYPH_PAINT_GLYPH_SELECT_STACK_HPP
#define CPPURSES_DEMOS_GLYPH_PAINT_GLYPH_SELECT_STACK_HPP
#include <signals_light/signal.hpp>

#include <cppurses/painter/glyph_string.hpp>
#include <cppurses/widget/layouts/stack.hpp>
#include <cppurses/widget/widgets/matrix_display.hpp>

namespace cppurses {
struct Area;
struct Glyph;
}  // namespace cppurses

namespace demos::glyph_paint {

class Glyph_select_stack
    : public cppurses::layout::Stack<cppurses::Matrix_display> {
   public:
    void set_symbols(cppurses::Glyph_string symbols);
    void append_symbols(cppurses::Glyph_string symbols);

    void set_page_percent(float percent);

    // Signals
    sl::Signal<void(cppurses::Glyph)> glyph_selected;

   protected:
    bool resize_event(cppurses::Area new_size,
                      cppurses::Area old_size) override;

   private:
    cppurses::Glyph_string symbols_;
    float percent_{0.0};

    void update_stack();
};

}  // namespace demos::glyph_paint

namespace demos::glyph_paint::slot {

auto set_page_percent(Glyph_select_stack& gss) -> sl::Slot<void(float)>;

auto set_page_percent(Glyph_select_stack& gss, float percent)
    -> sl::Slot<void()>;

}  // namespace demos::glyph_paint::slot
#endif  // CPPURSES_DEMOS_GLYPH_PAINT_GLYPH_SELECT_STACK_HPP

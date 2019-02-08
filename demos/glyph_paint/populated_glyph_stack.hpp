#ifndef DEMOS_GLYPH_PAINT_POPULATED_GLYPH_STACK_HPP
#define DEMOS_GLYPH_PAINT_POPULATED_GLYPH_STACK_HPP
#include <functional>
#include <vector>

#include <cppurses/painter/glyph.hpp>
#include <cppurses/widget/widgets/cycle_stack.hpp>

#include <signals/slot.hpp>

namespace demos {
namespace glyph_paint {

class Populated_glyph_stack : public cppurses::Cycle_stack {
   public:
    Populated_glyph_stack();
    void make_connections(sig::Slot<void(cppurses::Glyph)> slot);

   private:
    template <typename Function_t>
    using Signal_ref_t = std::reference_wrapper<sig::Signal<Function_t>>;

    std::vector<Signal_ref_t<void(cppurses::Glyph)>> signal_refs_;
};

}  // namespace glyph_paint
}  // namespace demos
#endif  // DEMOS_GLYPH_PAINT_POPULATED_GLYPH_STACK_HPP

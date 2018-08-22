#ifndef DEMOS_GLYPH_PAINT_POPULATED_GLYPH_STACK_HPP
#define DEMOS_GLYPH_PAINT_POPULATED_GLYPH_STACK_HPP

#include <cppurses/cppurses.hpp>
#include <signals/slot.hpp>

#include <functional>
#include <vector>

using namespace cppurses;

namespace demos {
namespace glyph_paint {

class Populated_glyph_stack : public Cycle_stack {
   public:
    Populated_glyph_stack();
    void make_connections(sig::Slot<void(Glyph)> slot);

   private:
    std::vector<std::reference_wrapper<sig::Signal<void(Glyph)>>> signal_refs_;
};

}  // namespace glyph_paint
}  // namespace demos
#endif  // DEMOS_GLYPH_PAINT_POPULATED_GLYPH_STACK_HPP

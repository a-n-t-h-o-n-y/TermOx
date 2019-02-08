#include "attribute_box.hpp"

#include <cppurses/widget/size_policy.hpp>

namespace demos {
namespace glyph_paint {

Attribute_box::Attribute_box() {
    this->height_policy.type(cppurses::Size_policy::Fixed);
    this->height_policy.hint(4);
}
}  // namespace glyph_paint
}  // namespace demos

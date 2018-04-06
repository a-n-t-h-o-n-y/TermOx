#include <cppurses/widget/widgets/label.hpp>

#include <cstddef>
#include <utility>

#include <cppurses/painter/painter.hpp>
#include <cppurses/widget/focus_policy.hpp>
#include <cppurses/widget/size_policy.hpp>

namespace cppurses {

Label::Label(Glyph_string text) : Text_display{std::move(text)} {
    this->focus_policy = Focus_policy::None;
    this->height_policy.type(Size_policy::Fixed);
    this->height_policy.hint(1);
}

}  // namespace cppurses

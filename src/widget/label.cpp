#include <cppurses/widget/widgets/label.hpp>

#include <utility>

#include <cppurses/painter/glyph_string.hpp>
#include <cppurses/widget/widgets/text_display.hpp>

namespace cppurses {

Label::Label(Glyph_string text) : Text_display{std::move(text)} {
    this->set_name("Label");
    this->height_policy.fixed(1);
    this->disable_word_wrap();
}
}  // namespace cppurses

#include <cppurses/widget/widgets/toggle_button.hpp>

#include <utility>

#include <cppurses/painter/glyph_string.hpp>
#include <cppurses/widget/widgets/push_button.hpp>

namespace {
const auto top_index = 0;
const auto bottom_index = 1;
}  // namespace

namespace cppurses {

Toggle_button::Toggle_button(Glyph_string top_label, Glyph_string bottom_label)
    : top{this->make_page<Push_button>(std::move(top_label))},
      bottom{this->make_page<Push_button>(std::move(bottom_label))} {
    this->set_name("Toggle_button");
    top.clicked.connect([this]() { this->set_active_page(bottom_index); });
    bottom.clicked.connect([this]() { this->set_active_page(top_index); });
    this->set_active_page(top_index);
}
}  // namespace cppurses

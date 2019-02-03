#include <cppurses/widget/widgets/toggle_button.hpp>

#include <utility>

#include <cppurses/painter/glyph_string.hpp>
#include <cppurses/widget/widgets/push_button.hpp>

namespace cppurses {

Toggle_button::Toggle_button(Glyph_string first_label,
                             Glyph_string second_label)
    : first_btn{this->make_page<Push_button>(std::move(first_label))},
      second_btn{this->make_page<Push_button>(std::move(second_label))} {
    first_btn.clicked.connect([this]() { this->set_active_page(1); });
    second_btn.clicked.connect([this]() { this->set_active_page(0); });
    this->set_active_page(0);
}

}  // namespace cppurses

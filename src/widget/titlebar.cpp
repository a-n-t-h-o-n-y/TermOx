#include <cppurses/widget/widgets/titlebar.hpp>

#include <utility>

#include <signals/signal.hpp>

#include <cppurses/painter/color.hpp>
#include <cppurses/painter/glyph.hpp>
#include <cppurses/painter/glyph_string.hpp>
#include <cppurses/system/system.hpp>
#include <cppurses/widget/size_policy.hpp>
#include <cppurses/widget/widgets/label.hpp>
#include <cppurses/widget/widgets/push_button.hpp>

namespace cppurses {

Titlebar::Titlebar(Glyph_string title_)
    : title{this->make_child<Label>(std::move(title_))},
      exit_button{this->make_child<Push_button>(L"│✕ ")} {
    this->set_name("Titlebar");
    this->height_policy.fixed(1);

    title.width_policy.type(Size_policy::Expanding);
    title.brush.set_background(Color::White);
    title.brush.set_foreground(Color::Black);
    title.set_alignment(Alignment::Center);

    exit_button.clicked.connect(System::quit);
    exit_button.brush.set_background(Color::White);
    exit_button.brush.set_foreground(Color::Black);
    exit_button.width_policy.fixed(3);
}
}  // namespace cppurses

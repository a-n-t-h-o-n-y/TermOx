#ifndef CPPURSES_DEMOS_GAME_OF_LIFE_OFFSET_CONTROL_WIDGET_HPP
#define CPPURSES_DEMOS_GAME_OF_LIFE_OFFSET_CONTROL_WIDGET_HPP
#include <signals/signals.hpp>

#include <cppurses/painter/glyph_string.hpp>
#include <cppurses/system/mouse_data.hpp>
#include <cppurses/widget/layouts/horizontal.hpp>
#include <cppurses/widget/layouts/vertical.hpp>
#include <cppurses/widget/widget.hpp>
#include <cppurses/widget/widgets/fixed_height.hpp>
#include <cppurses/widget/widgets/push_button.hpp>

namespace gol {

struct Vertical_arrows : cppurses::layout::Vertical {
    Vertical_arrows();

    cppurses::Push_button& up_btn{
        this->make_child<cppurses::Push_button>(L'▲')};
    cppurses::Push_button& down_btn{
        this->make_child<cppurses::Push_button>(L'▼')};

   protected:
    bool mouse_press_event_filter(cppurses::Widget& receiver,
                                  const cppurses::Mouse_data& mouse) override;
};

struct Scroll_btn : cppurses::Push_button {
    Scroll_btn(cppurses::Glyph_string title);

    sig::Signal<void()> scrolled_up;
    sig::Signal<void()> scrolled_down;

   protected:
    bool mouse_press_event(const cppurses::Mouse_data& mouse) override;
};

struct Horizontal_arrow : cppurses::layout::Vertical {
    Horizontal_arrow(cppurses::Glyph_string title);

    cppurses::Fixed_height& space{this->make_child<cppurses::Fixed_height>(1)};
    Scroll_btn& arrow;
};

struct Offset_control_widget : cppurses::layout::Horizontal {
    Offset_control_widget();

    Horizontal_arrow& left_arrow{this->make_child<Horizontal_arrow>(L'⯇')};
    Vertical_arrows& vertical_arrows{this->make_child<Vertical_arrows>()};
    Horizontal_arrow& right_arrow{this->make_child<Horizontal_arrow>(L'⯈')};

    sig::Signal<void()>& up_request{vertical_arrows.up_btn.clicked};
    sig::Signal<void()>& down_request{vertical_arrows.down_btn.clicked};
    sig::Signal<void()>& left_request{left_arrow.arrow.clicked};
    sig::Signal<void()>& right_request{right_arrow.arrow.clicked};
};

}  // namespace gol
#endif  // CPPURSES_DEMOS_GAME_OF_LIFE_OFFSET_CONTROL_WIDGET_HPP

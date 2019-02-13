#ifndef CPPURSES_WIDGET_WIDGETS_COLOR_SELECT_HPP
#define CPPURSES_WIDGET_WIDGETS_COLOR_SELECT_HPP
#include <signals/signals.hpp>

#include <cppurses/painter/color.hpp>
#include <cppurses/widget/layouts/horizontal.hpp>
#include <cppurses/widget/layouts/vertical.hpp>
#include <cppurses/widget/widgets/push_button.hpp>

namespace cppurses {

/// Provides a visual display of the 16 colors and sends Signals on clicks.
class Color_select : public layout::Vertical {
   public:
    Color_select();

    /// Emitted on left mouse button press, sends along the Color clicked on.
    sig::Signal<void(Color)> color_changed;

   private:
    layout::Horizontal& row1_{this->make_child<layout::Horizontal>()};
    layout::Horizontal& row2_{this->make_child<layout::Horizontal>()};

    Push_button& black_{row1_.make_child<Push_button>()};
    Push_button& dark_red_{row1_.make_child<Push_button>()};
    Push_button& dark_blue_{row1_.make_child<Push_button>()};
    Push_button& dark_gray_{row1_.make_child<Push_button>()};
    Push_button& brown_{row1_.make_child<Push_button>()};
    Push_button& green_{row1_.make_child<Push_button>()};
    Push_button& red_{row1_.make_child<Push_button>()};
    Push_button& gray_{row1_.make_child<Push_button>()};
    Push_button& blue_{row2_.make_child<Push_button>()};
    Push_button& orange_{row2_.make_child<Push_button>()};
    Push_button& light_gray_{row2_.make_child<Push_button>()};
    Push_button& light_green_{row2_.make_child<Push_button>()};
    Push_button& violet_{row2_.make_child<Push_button>()};
    Push_button& light_blue_{row2_.make_child<Push_button>()};
    Push_button& yellow_{row2_.make_child<Push_button>()};
    Push_button& white_{row2_.make_child<Push_button>()};

    void initialize();
};
}  // namespace cppurses
#endif  // CPPURSES_WIDGET_WIDGETS_COLOR_SELECT_HPP

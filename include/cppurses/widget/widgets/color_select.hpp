#ifndef WIDGET_WIDGETS_COLOR_SELECT_HPP
#define WIDGET_WIDGETS_COLOR_SELECT_HPP
#include "painter/color.hpp"
#include "widget/widgets/push_button.hpp"
#include "widget/layouts/vertical_layout.hpp"
#include "widget/layouts/horizontal_layout.hpp"
#include <signals/signals.hpp>

namespace cppurses {

class Color_select : public Vertical_layout {
   public:
    Color_select();

    // Signals
    sig::Signal<void(Color)> color_changed;

   protected:
    Horizontal_layout& row1_{this->make_child<Horizontal_layout>()};
    Horizontal_layout& row2_{this->make_child<Horizontal_layout>()};

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

   private:
    void initialize();
};

}  // namespace cppurses
#endif  // WIDGET_WIDGETS_COLOR_SELECT_HPP

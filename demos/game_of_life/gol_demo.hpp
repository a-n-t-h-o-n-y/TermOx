#ifndef CPPURSES_DEMOS_GAME_OF_LIFE_GOL_DEMO_HPP
#define CPPURSES_DEMOS_GAME_OF_LIFE_GOL_DEMO_HPP
#include <cppurses/widget/layouts/horizontal.hpp>
#include <cppurses/widget/widgets/accordion.hpp>

#include "cppurses/widget/widgets/accordion.hpp"
#include "gol_widget.hpp"
#include "side_panel.hpp"

namespace gol {

struct GoL_demo : cppurses::layout::Horizontal<> {
    GoL_demo();

    using Side_panel_accordion =
        cppurses::Accordion<cppurses::layout::Horizontal<>, Side_panel>;

    Side_panel_accordion& side_panel =
        this->make_child<Side_panel_accordion>("Settings");
    GoL_widget& gol_display = this->make_child<GoL_widget>();
};

}  // namespace gol
#endif  // CPPURSES_DEMOS_GAME_OF_LIFE_GOL_DEMO_HPP

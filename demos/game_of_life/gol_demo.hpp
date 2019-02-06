#ifndef CPPURSES_DEMOS_GAME_OF_LIFE_GOL_DEMO_HPP
#define CPPURSES_DEMOS_GAME_OF_LIFE_GOL_DEMO_HPP
#include <cppurses/widget/layouts/horizontal_layout.hpp>

#include "gol_widget.hpp"
#include "side_panel.hpp"

namespace gol {
struct GoL_demo : cppurses::Horizontal_layout {
    GoL_demo();

    Side_panel& side_panel{this->make_child<Side_panel>()};
    GoL_widget& gol_display{this->make_child<GoL_widget>()};
};
}  // namespace gol
#endif  // CPPURSES_DEMOS_GAME_OF_LIFE_GOL_DEMO_HPP

#ifndef CPPURSES_DEMOS_GAME_OF_LIFE_SIDE_PANEL_HPP
#define CPPURSES_DEMOS_GAME_OF_LIFE_SIDE_PANEL_HPP
#include <cppurses/widget/layouts/vertical_layout.hpp>

#include "examples_info_box.hpp"
#include "files_box.hpp"
#include "offset_control_widget.hpp"
#include "settings_box.hpp"

namespace gol {

struct Side_panel : cppurses::Vertical_layout {
    Side_panel();

    Examples_info_box& examples_info{this->make_child<Examples_info_box>()};
    Files_box& files{this->make_child<Files_box>()};
    Settings_box& settings{this->make_child<Settings_box>()};
    Offset_control_widget& offset_control{
        this->make_child<Offset_control_widget>()};
};
}  // namespace gol
#endif  // CPPURSES_DEMOS_GAME_OF_LIFE_SIDE_PANEL_HPP

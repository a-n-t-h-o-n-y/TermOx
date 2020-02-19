#ifndef CPPURSES_DEMOS_GAME_OF_LIFE_SIDE_PANEL_HPP
#define CPPURSES_DEMOS_GAME_OF_LIFE_SIDE_PANEL_HPP
#include <cppurses/painter/color.hpp>
#include <cppurses/widget/layouts/vertical.hpp>
#include <cppurses/widget/widget.hpp>

// #include "examples_info_box.hpp"
#include "files_box.hpp"
#include "settings_box.hpp"
#include "status_box.hpp"

namespace gol {

struct Side_panel : cppurses::layout::Vertical<> {
   public:
    // Examples_info_box& examples_info{this->make_child<Examples_info_box>()};
    cppurses::Widget& empty_space = this->make_child<cppurses::Widget>();
    Files_box& files              = this->make_child<Files_box>();
    Settings_box& settings        = this->make_child<Settings_box>();
    Status_box& status            = this->make_child<Status_box>();

   public:
    Side_panel()
    {
        this->width_policy.fixed(16);
        this->border.enable();
        this->border.segments.disable_all();
        this->border.segments.east.enable();
        this->border.segments.east.brush.set_foreground(cppurses::Color::Blue);

        empty_space.height_policy.expanding(0);
    }
};
}  // namespace gol
#endif  // CPPURSES_DEMOS_GAME_OF_LIFE_SIDE_PANEL_HPP

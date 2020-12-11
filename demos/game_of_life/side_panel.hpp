#ifndef CPPURSES_DEMOS_GAME_OF_LIFE_SIDE_PANEL_HPP
#define CPPURSES_DEMOS_GAME_OF_LIFE_SIDE_PANEL_HPP
#include <cppurses/painter/color.hpp>
#include <cppurses/widget/layouts/vertical.hpp>
#include <cppurses/widget/pipe.hpp>
#include <cppurses/widget/widget.hpp>
#include <cppurses/widget/widgets/accordion.hpp>

#include "colors.hpp"
#include "controls_box.hpp"
#include "files_box.hpp"
#include "patterns_rulesets_box.hpp"
#include "status_box.hpp"

namespace gol {

struct Side_panel : cppurses::layout::Vertical<> {
   private:
    using Pattern_rulesets = cppurses::VAccordion<Patterns_rulesets_box>;
    using Files            = cppurses::VAccordion<Files_box>;
    using Settings         = cppurses::VAccordion<Controls_box>;
    using Status           = cppurses::VAccordion<Status_box>;

   public:
    Patterns_rulesets_box& patterns_rulesets =
        this->make_child<Pattern_rulesets>(
                {L"Examples", cppurses::Align::Left, line})
            .wrapped();

    Files_box& files =
        this->make_child<Files>({L"Files", cppurses::Align::Left, line})
            .wrapped();

    Settings& settings_accordion =
        this->make_child<Settings>({L"Controls", cppurses::Align::Left, line});

    Controls_box& settings = settings_accordion.wrapped();

    Status_box& status =
        this->make_child<Status>({L"Status", cppurses::Align::Left, line})
            .wrapped();

    cppurses::Widget& empty_space = this->make_child();

   public:
    Side_panel()
    {
        using namespace cppurses;
        using namespace cppurses::pipe;

        *this | fixed_width(16uL);

        empty_space | expanding_height(0uL);
        settings_accordion.expand();
    }

   private:
    inline static auto const line = L'─' | fg(color::Light_green);
};
}  // namespace gol
#endif  // CPPURSES_DEMOS_GAME_OF_LIFE_SIDE_PANEL_HPP

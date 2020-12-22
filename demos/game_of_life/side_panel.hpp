#ifndef TERMOX_DEMOS_GAME_OF_LIFE_SIDE_PANEL_HPP
#define TERMOX_DEMOS_GAME_OF_LIFE_SIDE_PANEL_HPP
#include <termox/painter/color.hpp>
#include <termox/widget/layouts/vertical.hpp>
#include <termox/widget/pipe.hpp>
#include <termox/widget/widget.hpp>
#include <termox/widget/widgets/accordion.hpp>

#include "colors.hpp"
#include "controls_box.hpp"
#include "files_box.hpp"
#include "patterns_rulesets_box.hpp"
#include "status_box.hpp"

namespace gol {

struct Side_panel : ox::layout::Vertical<> {
   private:
    using Pattern_rulesets = ox::VAccordion<Patterns_rulesets_box>;
    using Files            = ox::VAccordion<Files_box>;
    using Settings         = ox::VAccordion<Controls_box>;
    using Status           = ox::VAccordion<Status_box>;

   public:
    Patterns_rulesets_box& patterns_rulesets =
        this->make_child<Pattern_rulesets>({L"Examples", ox::Align::Left, line})
            .wrapped();

    Files_box& files =
        this->make_child<Files>({L"Files", ox::Align::Left, line}).wrapped();

    Settings& settings_accordion =
        this->make_child<Settings>({L"Controls", ox::Align::Left, line});

    Controls_box& settings = settings_accordion.wrapped();

    Status_box& status =
        this->make_child<Status>({L"Status", ox::Align::Left, line}).wrapped();

    ox::Widget& empty_space = this->make_child();

   public:
    Side_panel()
    {
        using namespace ox;
        using namespace ox::pipe;

        *this | fixed_width(16uL);

        empty_space | expanding_height(0uL);
        settings_accordion.expand();
    }

   private:
    inline static auto const line = L'─' | fg(color::Light_green);
};
}  // namespace gol
#endif  // TERMOX_DEMOS_GAME_OF_LIFE_SIDE_PANEL_HPP

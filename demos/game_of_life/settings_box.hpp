#ifndef CPPURSES_DEMOS_GAME_OF_LIFE_SETTINGS_BOX_HPP
#define CPPURSES_DEMOS_GAME_OF_LIFE_SETTINGS_BOX_HPP
#include <cctype>
#include <chrono>
#include <string>

#include <signals/signal.hpp>

#include <cppurses/painter/color.hpp>
#include <cppurses/painter/glyph.hpp>
#include <cppurses/painter/glyph_string.hpp>
#include <cppurses/painter/trait.hpp>
#include <cppurses/widget/align.hpp>
#include <cppurses/widget/layouts/horizontal.hpp>
#include <cppurses/widget/layouts/vertical.hpp>
#include <cppurses/widget/pipe.hpp>
#include <cppurses/widget/widget.hpp>
#include <cppurses/widget/widgets/button.hpp>
#include <cppurses/widget/widgets/checkbox.hpp>
#include <cppurses/widget/widgets/confirm_button.hpp>
#include <cppurses/widget/widgets/label.hpp>
#include <cppurses/widget/widgets/line_edit.hpp>
#include <cppurses/widget/widgets/number_edit.hpp>
#include <cppurses/widget/widgets/text_display.hpp>
#include <cppurses/widget/widgets/toggle_button.hpp>

#include "colors.hpp"
#include "make_break.hpp"

namespace gol {

struct Clear_step_box : cppurses::layout::Vertical<> {
   public:
    cppurses::Button& step_btn = this->make_child<cppurses::Button>("Step>");

    cppurses::Confirm_button& clear_btn =
        this->make_child<cppurses::Confirm_button>("Clear");

   public:
    Clear_step_box()
    {
        using namespace cppurses;
        using namespace cppurses::pipe;

        *this | fixed_height(2uL);
        clear_btn.main_btn | bg(color::Light_green) | fg(color::Teal);
        clear_btn.confirm_page.confirm_btn | bg(color::Light_green) |
            fg(color::Teal);
        clear_btn.confirm_page.exit_btn | bg(color::Teal) | fg(color::White);
        step_btn | bg(color::Teal) | fg(color::Light_green);
    }
};

struct Rule_edit : cppurses::layout::Vertical<> {
   public:
    sig::Signal<void(std::string const&)> rule_change;

   public:
    cppurses::HLabel& label = this->make_child<cppurses::HLabel>(
        {L"Rules[B/S]" | cppurses::Trait::Underline | cppurses::Trait::Bold,
         cppurses::Align::Center});

    cppurses::Line_edit& edit_box =
        this->make_child<cppurses::Line_edit>(L"3/23");

   public:
    Rule_edit()
    {
        using namespace cppurses;
        using namespace cppurses::pipe;

        *this | fixed_height(2uL);
        edit_box | bg(color::White) | fg(color::Black) | ghost(color::Teal);

        edit_box.set_validator(
            [](char c) { return std::isdigit(c) || c == '/'; });

        edit_box.edit_finished.connect(
            [this](std::string rule_text) { rule_change(rule_text); });
    }
};

struct Start_pause_btns : cppurses::Toggle_button {
   public:
    sig::Signal<void()>& start_requested = top.pressed;
    sig::Signal<void()>& pause_requested = bottom.pressed;

   public:
    Start_pause_btns() : Toggle_button{L"Start", L"Pause"}
    {
        using namespace cppurses;
        using namespace cppurses::pipe;

        *this | fixed_height(1uL);
        top | bg(color::Light_green) | fg(color::Teal);
        bottom | bg(color::Green) | fg(color::White);
    }
};

struct Interval_box : cppurses::layout::Horizontal<> {
   public:
    cppurses::Labeled_number_edit<unsigned>& value_edit =
        this->make_child<cppurses::Labeled_number_edit<unsigned>>("Interval ",
                                                                  120);

    cppurses::HLabel& units = this->make_child<cppurses::HLabel>({L"ms"});

    sig::Signal<void(unsigned)>& value_set = value_edit.value_set;

   public:
    Interval_box()
    {
        using namespace cppurses;
        using namespace cppurses::pipe;

        *this | fixed_height(1uL);
        units | bg(color::White) | fg(color::Teal) | fixed_width(2uL);
    }
};

struct Grid_fade : cppurses::layout::Horizontal<cppurses::Labeled_checkbox> {
   public:
    Child_t& grid_box = this->make_child("Grid");
    Child_t& fade_box = this->make_child("Fade");

   public:
    Grid_fade()
    {
        using namespace cppurses::pipe;
        *this | fixed_height(1uL) | children() |
            for_each([](auto& c) { c.padding | fixed_width(1uL); });
        fade_box.checkbox.toggle();
    }
};

// TODO rename to Controls_box, also file rename
struct Settings_box : cppurses::layout::Vertical<> {
   public:
    // Widget& break_                    = this->append(make_break());
    Interval_box& interval_edit        = this->make_child<Interval_box>();
    Widget& break_0                    = this->append(make_break());
    Start_pause_btns& start_pause_btns = this->make_child<Start_pause_btns>();
    Clear_step_box& clear_step_btns    = this->make_child<Clear_step_box>();
    Widget& break_1                    = this->append(make_break());
    Grid_fade& grid_fade               = this->make_child<Grid_fade>();
    Widget& break_2                    = this->append(make_break());
    Rule_edit& rule_edit               = this->make_child<Rule_edit>();
    Widget& break_3                    = this->append(make_break());

   public:
    sig::Signal<void(std::string const&)>& rule_change = rule_edit.rule_change;
    sig::Signal<void(std::chrono::milliseconds)> interval_set;
    sig::Signal<void()>& grid_toggled  = grid_fade.grid_box.toggled;
    sig::Signal<void()>& fade_toggled  = grid_fade.fade_box.toggled;
    sig::Signal<void()>& clear_request = clear_step_btns.clear_btn.pressed;
    sig::Signal<void()>& step_request  = clear_step_btns.step_btn.pressed;

   public:
    Settings_box()
    {
        using namespace cppurses::pipe;
        *this | fixed_height(10uL);

        interval_edit.value_set.connect([this](int value) {
            interval_set(std::chrono::milliseconds{value});
        });
        interval_edit.value_edit.number_edit | ghost(color::Teal);
    }
};

}  // namespace gol
#endif  // CPPURSES_DEMOS_GAME_OF_LIFE_SETTINGS_BOX_HPP

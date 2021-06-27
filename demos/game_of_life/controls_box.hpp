#ifndef TERMOX_DEMOS_GAME_OF_LIFE_SETTINGS_BOX_HPP
#define TERMOX_DEMOS_GAME_OF_LIFE_SETTINGS_BOX_HPP
#include <cctype>
#include <chrono>
#include <string>

#include <signals_light/signal.hpp>

#include <termox/painter/color.hpp>
#include <termox/painter/glyph.hpp>
#include <termox/painter/glyph_string.hpp>
#include <termox/painter/trait.hpp>
#include <termox/widget/align.hpp>
#include <termox/widget/layouts/horizontal.hpp>
#include <termox/widget/layouts/vertical.hpp>
#include <termox/widget/pipe.hpp>
#include <termox/widget/widget.hpp>
#include <termox/widget/widgets/button.hpp>
#include <termox/widget/widgets/checkbox.hpp>
#include <termox/widget/widgets/confirm_button.hpp>
#include <termox/widget/widgets/label.hpp>
#include <termox/widget/widgets/line.hpp>
#include <termox/widget/widgets/line_edit.hpp>
#include <termox/widget/widgets/number_edit.hpp>
#include <termox/widget/widgets/text_view.hpp>
#include <termox/widget/widgets/toggle_button.hpp>

#include "colors.hpp"
#include "rule.hpp"

namespace gol {

struct Clear_step_box : ox::layout::Vertical<> {
   public:
    ox::Button& step_btn = this->make_child<ox::Button>("Step>");

    ox::Confirm_button& clear_btn =
        this->make_child<ox::Confirm_button>("Clear");

   public:
    Clear_step_box()
    {
        using namespace ox;
        using namespace ox::pipe;

        *this | fixed_height(2);
        clear_btn.main_btn | bg(color::Light_green) | fg(color::Teal);
        clear_btn.confirm_page.confirm_btn | bg(color::Light_green) |
            fg(color::Teal);
        clear_btn.confirm_page.exit_btn | bg(color::Teal) | fg(color::White);
        step_btn | bg(color::Teal) | fg(color::Light_green);
    }
};

class Rule_edit : public ox::layout::Vertical<> {
   public:
    sl::Signal<void(std::string const&)> rule_change;

   public:
    Rule_edit()
    {
        using namespace ox::pipe;

        *this | fixed_height(2);
        edit_box_ | bg(color::White) | fg(color::Black);

        edit_box_.set_validator([](char c) {
            return std::isdigit(c) || c == '/' || c == 'B' || c == 'b' ||
                   c == 'S' || c == 's';
        });

        edit_box_.submitted.connect(
            [this](std::string const& rule_text) { rule_change(rule_text); });

        this->set_rule(parse_rule_string("B3/S23"));
    }

   public:
    void set_rule(Rule r)
    {
        auto const rs = to_rule_string(r);
        edit_box_.set_text(rs);
        edit_box_.set_cursor_to_index(rs.size());
    }

   private:
    ox::HLabel& label = this->make_child<ox::HLabel>(
        {U"RuleString[B/S]" | ox::Trait::Underline | ox::Trait::Bold,
         ox::Align::Center});

    ox::Line_edit& edit_box_ = this->make_child<ox::Line_edit>();
};

struct Start_pause_btns : ox::Toggle_button {
   public:
    sl::Signal<void()>& start_requested = top.pressed;
    sl::Signal<void()>& pause_requested = bottom.pressed;

   public:
    Start_pause_btns() : Toggle_button{U"Start", U"Pause"}
    {
        using namespace ox;
        using namespace ox::pipe;

        *this | fixed_height(1);
        top | bg(color::Light_green) | fg(color::Teal);
        bottom | bg(color::Green) | fg(color::White);
    }
};

struct Value_edit : ox::HPair<ox::HLabel, ox::Unsigned_edit> {
    ox::HLabel& label       = this->first;
    ox::Unsigned_edit& edit = this->second;

    Value_edit()
        : ox::HPair<ox::HLabel, ox::Unsigned_edit>{{U"Interval"},
                                                   {40, {1, 5'000}}}
    {
        label | ox::pipe::fixed_width(9);
    }
};

struct Interval_box : ox::layout::Horizontal<> {
   public:
    Value_edit& value = this->make_child<Value_edit>();
    ox::HLabel& units = this->make_child<ox::HLabel>({U"ms"});

    sl::Signal<void(unsigned)>& submitted = value.edit.submitted;

   public:
    Interval_box()
    {
        using namespace ox;
        using namespace ox::pipe;

        *this | fixed_height(1);
        units | bg(color::White) | fg(color::Teal) | fixed_width(2);
        value.edit | bg(color::White) | fg(color::Teal);
    }
};

struct Grid_hi_res : ox::layout::Horizontal<ox::HCheckbox_label> {
   public:
    Child_t& grid_box   = this->make_child({U"Grid"});
    Child_t& hi_res_box = this->make_child({U"Hi-Res"});

   public:
    Grid_hi_res()
    {
        using namespace ox::pipe;
        *this | fixed_height(1) | children() |
            for_each([](auto& c) { c.padding | fixed_width(1); });
        hi_res_box.checkbox.toggle();
    }
};

struct Rainbow_btn : ox::HCheckbox_label {
    Rainbow_btn() : ox::HCheckbox_label{{U"Rainbow Mode"}} {}
};

struct Controls_box : ox::VTuple<Interval_box,
                                 ox::HLine,
                                 Start_pause_btns,
                                 Clear_step_box,
                                 ox::HLine,
                                 Grid_hi_res,
                                 Rainbow_btn,
                                 ox::HLine,
                                 Rule_edit,
                                 ox::HLine> {
   public:
    Interval_box& interval_edit        = this->get<0>();
    ox::HLine& break_0                 = this->get<1>() | fg(color::Teal);
    Start_pause_btns& start_pause_btns = this->get<2>();
    Clear_step_box& clear_step_btns    = this->get<3>();
    ox::HLine& break_1                 = this->get<4>() | fg(color::Teal);
    Grid_hi_res& grid_hi_res           = this->get<5>();
    Rainbow_btn& rainbow_btn           = this->get<6>();
    ox::HLine& break_2                 = this->get<7>() | fg(color::Teal);
    Rule_edit& rule_edit               = this->get<8>();
    ox::HLine& break_3                 = this->get<9>() | fg(color::Teal);

   public:
    sl::Signal<void(std::string const&)>& rule_change = rule_edit.rule_change;
    sl::Signal<void(std::chrono::milliseconds)> interval_set;
    sl::Signal<void()>& grid_toggled = grid_hi_res.grid_box.checkbox.toggled;
    sl::Signal<void()>& hi_res_toggled =
        grid_hi_res.hi_res_box.checkbox.toggled;
    sl::Signal<void()>& clear_request  = clear_step_btns.clear_btn.pressed;
    sl::Signal<void()>& step_request   = clear_step_btns.step_btn.pressed;
    sl::Signal<void()>& rainbow_toggle = rainbow_btn.checkbox.toggled;

   public:
    Controls_box()
    {
        *this | ox::pipe::fixed_height(12);

        interval_edit.submitted.connect([this](int value) {
            interval_set(std::chrono::milliseconds{value});
        });
    }
};

}  // namespace gol
#endif  // TERMOX_DEMOS_GAME_OF_LIFE_SETTINGS_BOX_HPP

#ifndef CPPURSES_DEMOS_GAME_OF_LIFE_SETTINGS_BOX_HPP
#define CPPURSES_DEMOS_GAME_OF_LIFE_SETTINGS_BOX_HPP
#include <cctype>
#include <chrono>
#include <string>

#include <signals/signal.hpp>

#include <cppurses/painter/color.hpp>
#include <cppurses/widget/layouts/horizontal_layout.hpp>
#include <cppurses/widget/layouts/vertical_layout.hpp>
#include <cppurses/widget/widgets/checkbox.hpp>
#include <cppurses/widget/widgets/confirm_button.hpp>
#include <cppurses/widget/widgets/label.hpp>
#include <cppurses/widget/widgets/line_edit.hpp>
#include <cppurses/widget/widgets/number_edit.hpp>
#include <cppurses/widget/widgets/push_button.hpp>

namespace gol {

struct Clear_step_box : cppurses::Vertical_layout {
    Clear_step_box();

    cppurses::Confirm_button& clear_btn{
        this->make_child<cppurses::Confirm_button>("Clear")};

    cppurses::Push_button& step_btn{
        this->make_child<cppurses::Push_button>("Step")};
};

template <typename Number_t = int>
struct Labeled_number_edit : cppurses::Horizontal_layout {
    Labeled_number_edit(const std::string& description, Number_t initial);

    cppurses::Label& label;
    cppurses::Number_edit<Number_t>& edit_box;

    sig::Signal<void(Number_t)>& value_set{edit_box.value_set};
};

struct Rule_edit : cppurses::Vertical_layout {
    Rule_edit();

    cppurses::Label& label{this->make_child<cppurses::Label>("Rules[B/S]")};
    cppurses::Line_edit& edit_box{
        this->make_child<cppurses::Line_edit>("3/23")};

    sig::Signal<void(const std::string&, const std::string&)> rule_change;
};

struct Start_stop_btns : cppurses::Horizontal_layout {
    Start_stop_btns();

    cppurses::Push_button& start_btn{
        this->make_child<cppurses::Push_button>("Start")};
    cppurses::Push_button& stop_btn{
        this->make_child<cppurses::Push_button>("Stop")};
};

struct Settings_box : cppurses::Vertical_layout {
    Settings_box();

    cppurses::Checkbox& grid_box{
        this->make_child<cppurses::Checkbox>("Grid", 5)};
    cppurses::Checkbox& fade_box{
        this->make_child<cppurses::Checkbox>("Fade", 5)};
    Rule_edit& rule_edit{this->make_child<Rule_edit>()};
    Labeled_number_edit<>& period_edit{
        this->make_child<Labeled_number_edit<>>("Period[ms]", 200)};
    Clear_step_box& clear_step_btns{this->make_child<Clear_step_box>()};
    Start_stop_btns& start_stop{this->make_child<Start_stop_btns>()};

    sig::Signal<void(const std::string&, const std::string&)>& rule_change{
        rule_edit.rule_change};
    sig::Signal<void(std::chrono::milliseconds)> period_set;
    sig::Signal<void()>& grid_toggled{grid_box.toggled};
    sig::Signal<void()>& fade_toggled{fade_box.toggled};
    sig::Signal<void()>& clear_request{clear_step_btns.clear_btn.clicked};
    sig::Signal<void()>& step_request{clear_step_btns.step_btn.clicked};
    sig::Signal<void()>& start_request{start_stop.start_btn.clicked};
    sig::Signal<void()>& stop_request{start_stop.stop_btn.clicked};
};
}  // namespace gol
#endif  // CPPURSES_DEMOS_GAME_OF_LIFE_SETTINGS_BOX_HPP

#include "gol_demo.hpp"

#include <chrono>
#include <cstdint>
#include <string>

#include <cppurses/widget/focus_policy.hpp>
#include <cppurses/widget/layouts/horizontal.hpp>
#include <cppurses/widget/widgets/accordion.hpp>

namespace gol {

GoL_demo::GoL_demo()
{
    this->focus_policy = cppurses::Focus_policy::Direct;
    side_panel_accordion.expand();
    side_panel.settings.rule_change.connect(
        [this](std::string const& rule_str) {
            gol_display.set_rules(rule_str);
        });
    gol_display.rule_changed.connect([this](const std::string& rule_str) {
        side_panel.settings.rule_edit.edit_box.set_contents(rule_str);
        side_panel.settings.rule_edit.edit_box.set_cursor(rule_str.size());
    });
    side_panel.settings.interval_set.connect(
        [this](std::chrono::milliseconds period) {
            gol_display.set_period(period);
        });
    side_panel.settings.grid_toggled.connect(
        [this]() { gol_display.toggle_grid(); });
    side_panel.settings.fade_toggled.connect(
        [this]() { gol_display.toggle_fade(); });
    side_panel.settings.clear_request.connect(
        [this]() { gol_display.clear(); });
    side_panel.settings.step_request.connect([this]() { gol_display.step(); });
    side_panel.settings.start_pause_btns.start_requested.connect(
        [this]() { gol_display.start(); });
    side_panel.settings.start_pause_btns.pause_requested.connect(
        [this]() { gol_display.pause(); });
    side_panel.files.import_request.connect(
        [this](const std::string& filename) { gol_display.import(filename); });
    side_panel.files.export_request.connect(
        [this](const std::string& filename) {
            gol_display.export_as(filename);
        });

    gol_display.offset_changed.connect([this](Coordinate c) {
        side_panel.status.center_offset.x_coords.set_value(c.x);
        side_panel.status.center_offset.y_coords.set_value(c.y);
    });
    side_panel.status.center_offset.x_coords.value_set.connect([this](int x) {
        gol_display.set_offset({x, gol_display.offset().y});
    });

    side_panel.status.center_offset.y_coords.value_set.connect([this](int y) {
        gol_display.set_offset({gol_display.offset().x, y});
    });

    gol_display.generation_count_changed.connect([this](std::uint32_t count) {
        side_panel.status.gen_count.update_count(count);
    });

    // TODO make this a free function that takes the patterns object and the
    // gol_display object and adds and hooks up everything
    auto& patterns = side_panel.patterns_rulesets.patterns;
    patterns.add_option(L"Pattern 1");
    patterns.add_option(L"Pattern 2");
    patterns.add_option(L"Pattern 3");
    patterns.add_option(L"Pattern 4");
    patterns.add_option(L"Pattern 5");
    patterns.add_option(L"Pattern 6");
    patterns.add_option(L"Pattern 7");

    auto& rules = side_panel.patterns_rulesets.rulesets;
    rules.add_option(L"Rule 1");
    rules.add_option(L"Rule 2");
    rules.add_option(L"Rule 3");
    rules.add_option(L"Rule 4");
    rules.add_option(L"Rule 5");
    rules.add_option(L"Rule 6");
    rules.add_option(L"Rule 7");
    rules.add_option(L"Rule 8");
    rules.add_option(L"Rule 9");
    rules.add_option(L"Rule 10");

    patterns.selection_made.connect([](auto const& /* name */) {
        // if (name == L"Pattern 1")
        //     ;
        // gol_display.import(
        //     "Pattern 1");  // TODO make import take some structure in
        // memory, then have a store for these structure,
        // and instead of the if/else if here, put that
        // in the store lookup and just pass the result
        // to gol_display.import();
        // else if (name == L"Pattern 2")
        //     ;
        // else if (name == L"Pattern 3")
        //     ;
        // else if (name == L"Pattern 4")
        //     ;
    });
}
}  // namespace gol

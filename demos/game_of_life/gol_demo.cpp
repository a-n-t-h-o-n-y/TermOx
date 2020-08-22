#include "gol_demo.hpp"

#include <chrono>
#include <cstdint>
#include <string>

namespace gol {

GoL_demo::GoL_demo()
{
    side_panel.wrapped().settings.rule_change.connect(
        [this](const std::string& rule_str) {
            gol_display.set_rules(rule_str);
        });
    gol_display.rule_changed.connect([this](const std::string& rule_str) {
        side_panel.wrapped().settings.rule_edit.edit_box.set_contents(rule_str);
        side_panel.wrapped().settings.rule_edit.edit_box.set_cursor(rule_str.size());
    });
    side_panel.wrapped().settings.period_set.connect(
        [this](std::chrono::milliseconds period) {
            gol_display.set_period(period);
        });
    side_panel.wrapped().settings.grid_toggled.connect(
        [this]() { gol_display.toggle_grid(); });
    side_panel.wrapped().settings.fade_toggled.connect(
        [this]() { gol_display.toggle_fade(); });
    side_panel.wrapped().settings.clear_request.connect(
        [this]() { gol_display.clear(); });
    side_panel.wrapped().settings.step_request.connect([this]() { gol_display.step(); });
    side_panel.wrapped().settings.start_pause_btns.start_requested.connect(
        [this]() { gol_display.start(); });
    side_panel.wrapped().settings.start_pause_btns.pause_requested.connect(
        [this]() { gol_display.pause(); });
    side_panel.wrapped().files.import_request.connect(
        [this](const std::string& filename) { gol_display.import(filename); });
    side_panel.wrapped().files.export_request.connect(
        [this](const std::string& filename) {
            gol_display.export_as(filename);
        });

    gol_display.offset_changed.connect([this](Coordinate c) {
        side_panel.wrapped().status.center_offset.x_coords.set_value(c.x);
        side_panel.wrapped().status.center_offset.y_coords.set_value(c.y);
    });
    side_panel.wrapped().status.center_offset.x_coords.value_set.connect([this](int x) {
        gol_display.set_offset({x, gol_display.offset().y});
    });

    side_panel.wrapped().status.center_offset.y_coords.value_set.connect([this](int y) {
        gol_display.set_offset({gol_display.offset().x, y});
    });

    gol_display.generation_count_changed.connect([this](std::uint32_t count) {
        side_panel.wrapped().status.gen_count.update_count(count);
    });
}
}  // namespace gol

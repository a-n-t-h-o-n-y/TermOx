#include "gol_demo.hpp"

#include <chrono>
#include <cstdint>
#include <string>

namespace gol {

GoL_demo::GoL_demo()
{
    side_panel.settings.rule_change.connect(
        [this](const std::string& rule_str) {
            gol_display.set_rules(rule_str);
        });
    gol_display.rule_changed.connect([this](const std::string& rule_str) {
        side_panel.settings.rule_edit.edit_box.set_contents(rule_str);
        side_panel.settings.rule_edit.edit_box.set_cursor(rule_str.size());
    });
    side_panel.settings.period_set.connect(
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
}
}  // namespace gol

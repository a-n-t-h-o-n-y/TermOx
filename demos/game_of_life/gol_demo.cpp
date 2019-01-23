#include "gol_demo.hpp"

#include <chrono>

namespace gol {

GoL_demo::GoL_demo() {
    side_panel.settings.rule_change.connect(
        [this](const std::string& birth, const std::string& survival) {
            gol_display.set_rules(birth, survival);
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
    side_panel.settings.start_request.connect(
        [this]() { gol_display.start(); });
    side_panel.settings.stop_request.connect([this]() { gol_display.stop(); });
    side_panel.files.import_request.connect(
        [this](const std::string& filename) { gol_display.import(filename); });
    side_panel.files.export_request.connect(
        [this](const std::string& filename) {
            gol_display.export_as(filename);
        });
    side_panel.offset_control.up_request.connect([this]() {
        Coordinate new_offset = gol_display.offset();
        --new_offset.y;
        gol_display.set_offset(new_offset);
    });
    side_panel.offset_control.down_request.connect([this]() {
        Coordinate new_offset = gol_display.offset();
        ++new_offset.y;
        gol_display.set_offset(new_offset);
    });
    side_panel.offset_control.left_request.connect([this]() {
        Coordinate new_offset = gol_display.offset();
        --new_offset.x;
        gol_display.set_offset(new_offset);
    });
    side_panel.offset_control.right_request.connect([this]() {
        Coordinate new_offset = gol_display.offset();
        ++new_offset.x;
        gol_display.set_offset(new_offset);
    });
}
}  // namespace gol

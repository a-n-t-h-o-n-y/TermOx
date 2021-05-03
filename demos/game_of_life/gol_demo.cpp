#include "gol_demo.hpp"

#include <chrono>
#include <cstdint>
#include <string>

#include <termox/widget/focus_policy.hpp>
#include <termox/widget/layouts/horizontal.hpp>
#include <termox/widget/widgets/accordion.hpp>

#include "patterns.hpp"

namespace gol {

GoL_demo::GoL_demo()
{
    this->focus_policy = ox::Focus_policy::Direct;
    side_panel_accordion.expand();
    side_panel.settings.rule_change.connect(
        [this](std::string const& rule_str) {
            gol_display.set_rules(rule_str);
        });
    gol_display.rule_changed.connect(
        [this](Rule r) { side_panel.settings.rule_edit.set_rule(r); });
    side_panel.settings.interval_set.connect(
        [this](std::chrono::milliseconds period) {
            gol_display.set_period(period);
        });
    side_panel.settings.grid_toggled.connect(
        [this]() { gol_display.toggle_grid(); });
    side_panel.settings.hi_res_toggled.connect(
        [this]() { gol_display.toggle_hi_res(); });
    side_panel.settings.rainbow_toggle.connect(
        [this]() { gol_display.toggle_rainbow(); });
    side_panel.settings.clear_request.connect(
        [this]() { gol_display.clear(); });
    side_panel.settings.step_request.connect([this]() { gol_display.step(); });
    side_panel.settings.start_pause_btns.start_requested.connect(
        [this]() { gol_display.start(); });
    side_panel.settings.start_pause_btns.pause_requested.connect(
        [this]() { gol_display.pause(); });
    side_panel.files.import_request.connect(
        [this](const std::string& filename) {
            gol_display.import_file(filename);
        });
    side_panel.files.export_request.connect(
        [this](const std::string& filename) {
            gol_display.export_to_file(filename);
        });

    gol_display.offset_changed.connect([this](Coordinate c) {
        side_panel.status.center_offset.x_coords.edit.set_value(c.x);
        side_panel.status.center_offset.y_coords.edit.set_value(c.y);
    });
    side_panel.status.center_offset.x_coords.edit.submitted.connect(
        [this](int x) {
            gol_display.set_offset({x, gol_display.offset().y});
        });

    side_panel.status.center_offset.y_coords.edit.submitted.connect(
        [this](int y) {
            gol_display.set_offset({gol_display.offset().x, y});
        });

    gol_display.generation_count_changed.connect([this](std::uint32_t count) {
        side_panel.status.gen_count.update_count(count);
    });

    auto& patterns = side_panel.patterns_rulesets.patterns;
    patterns.selection_made.connect([this](std::u32string const& name) {
        gol_display.import_pattern(pattern_store_.get(name));
    });

    this->add_pattern_example(U"Tetrominos", pattern::tetrominos);
    this->add_pattern_example(U"R-pentomino", pattern::r_pentomino);
    this->add_pattern_example(U"B-heptomino", pattern::b_heptomino);
    this->add_pattern_example(U"C-heptomino", pattern::c_heptomino);
    this->add_pattern_example(U"E-heptomino", pattern::e_heptomino);
    this->add_pattern_example(U"F-heptomino", pattern::f_heptomino);
    this->add_pattern_example(U"Pi-heptomino", pattern::pi_heptomino);
    this->add_pattern_example(U"Century", pattern::century);
    this->add_pattern_example(U"StairStep Hexomino",
                              pattern::stairstep_hexomino);
    this->add_pattern_example(U"Z-hexomino", pattern::z_hexomino);
    this->add_pattern_example(U"Kok's Galaxy", pattern::koks_galaxy);
    this->add_pattern_example(U"Achim's p144", pattern::achims_p144);
    this->add_pattern_example(U"Achim's p11", pattern::achims_p11);
    this->add_pattern_example(U"Achim's Other p16", pattern::achims_other_p16);
    this->add_pattern_example(U"Gabriels p138", pattern::gabriels_p138);
    this->add_pattern_example(U"Smiley", pattern::smiley);
    this->add_pattern_example(U"Twin Bees Shuttle", pattern::twin_bees_shuttle);
    this->add_pattern_example(U"Loafer", pattern::loafer);
    this->add_pattern_example(U"Queen Bee Shuttle", pattern::queen_bee_shuttle);
    this->add_pattern_example(U"Breeder1", pattern::breeder1);
    this->add_pattern_example(U"Halfmax", pattern::halfmax);
    this->add_pattern_example(U"Sir Robin", pattern::sir_robin);
    this->add_pattern_example(U"David Hilbert", pattern::david_hilbert);
    this->add_pattern_example(U"Spider", pattern::spider);
    this->add_pattern_example(U"Scholar", pattern::scholar);
    this->add_pattern_example(U"Snark Variant", pattern::snark_variant);
    this->add_pattern_example(U"Two Engine Cordership Seed",
                              pattern::two_engine_cordership_seed);
    this->add_pattern_example(U"Gosper Glyder Gun", pattern::gosper_glyder_gun);
    this->add_pattern_example(U"Bi-Gun", pattern::bi_gun);
    this->add_pattern_example(U"CopperHead", pattern::copperhead);
    this->add_pattern_example(U"Three Engine Cordership Rake",
                              pattern::three_engine_cordership_rake);
    this->add_pattern_example(U"Merzenich's p31", pattern::merzenichs_p31);
    this->add_pattern_example(U"Lobster", pattern::lobster);
    this->add_pattern_example(U"37P4H1V0", pattern::three7P4H1V0);
    this->add_pattern_example(U"Swan Tagalong", pattern::swan_tagalong);
    this->add_pattern_example(U"P50 Traffic Jam", pattern::p50_traffic_jam);
    this->add_pattern_example(U"Quad Fuse", pattern::quad_fuse);
    this->add_pattern_example(
        U"xp4_y19hggggh9z3w1u042w240u1w3z6w43g9a88a9g34w6zy144y044",
        pattern::xp4_y19hggggh9z3w1u042w240u1w3z6w43g9a88a9g34w6zy144y044);
    this->add_pattern_example(U"xq8_y1842ve4z8sqp821zw1",
                              pattern::xq8_y1842ve4z8sqp821zw1);
    this->add_pattern_example(
        U"xp4_63p4ky2k4p36zy1ci11iczog68aw11wa86goz011y611",
        pattern::xp4_63p4ky2k4p36zy1ci11iczog68aw11wa86goz011y611);

    auto& rules = side_panel.patterns_rulesets.rulesets;
    rules.selection_made.connect([this](std::u32string const& name) {
        gol_display.set_rules(rule_store_.get(name));
    });

    this->add_rule_example(U"Replicator", "B1357/S1357");
    this->add_rule_example(U"Fredkin", "B1357/S02468");
    this->add_rule_example(U"Seeds", "B2/S");
    this->add_rule_example(U"Fractal Rug", "B1/S12");
    this->add_rule_example(U"Slow Explosion", "B12/S123");
    this->add_rule_example(U"Live Free or Die", "B2/S0");
    this->add_rule_example(U"Life Without Death", "B3/S012345678");
    this->add_rule_example(U"Flock", "B3/S12");
    this->add_rule_example(U"Mazectric", "B3/S1234");
    this->add_rule_example(U"Maze", "B3/S12345");
    this->add_rule_example(U"Conway's Game of Life", "B3/S23");
    this->add_rule_example(U"EightLife", "B3/S238");
    this->add_rule_example(U"Coral", "B3/S45678");
    this->add_rule_example(U"2x2", "B36/S125");
    this->add_rule_example(U"HighLife", "B36/S23");
    this->add_rule_example(U"Move", "B368/S245");
    this->add_rule_example(U"Day & Night", "B3678/S34678");
    this->add_rule_example(U"DryLife", "B37/S23");
    this->add_rule_example(U"Pedestrian Life", "B38/S23");
    this->add_rule_example(U"AntiLife", "B0123478/S01234678");
    this->add_rule_example(U"InverseLife", "B012345678/S34678");
    this->add_rule_example(U"H-Trees", "B1/S012345678");
    this->add_rule_example(U"Gnarl", "B1/S1");
    this->add_rule_example(U"Serviettes", "B234/S");
    this->add_rule_example(U"Iceballs", "B25678/S5678");
    this->add_rule_example(U"DotLife", "B3/S023");
    this->add_rule_example(U"SnowLife", "B3/S1237");
    this->add_rule_example(U"Corrosion of Conformity", "B3/S124");
    this->add_rule_example(U"LowLife", "B3/S13");
    this->add_rule_example(U"B3/S2", "B3/S2");
    this->add_rule_example(U"3-4 Life", "B34/S34");
    this->add_rule_example(U"Bacteria", "B34/S456");
    this->add_rule_example(U"Blinkers", "B345/S2");
    this->add_rule_example(U"Assimilation", "B345/S4567");
    this->add_rule_example(U"Long Life", "B345/S5");
    this->add_rule_example(U"Gems", "B3457/S4568");
    this->add_rule_example(U"Gems Minor", "B34578/S456");
    this->add_rule_example(U"Land Rush", "B35/S234578");
    this->add_rule_example(U"Bugs", "B3567/S15678");
    this->add_rule_example(U"Holstein", "B35678/S4678");
    this->add_rule_example(U"Diamoeba", "B35678/S5678");
    this->add_rule_example(U"Amoeba", "B357/S1358");
    this->add_rule_example(U"Pseudo Life", "B357/S238");
    this->add_rule_example(U"Blinker Life", "B36/S235");
    this->add_rule_example(U"Slow Blob", "B367/S125678");
    this->add_rule_example(U"Stains", "B3678/S235678");
    this->add_rule_example(U"LowDeath", "B368/S238");
    this->add_rule_example(U"Mazectric w/Mice", "B37/S1234");
    this->add_rule_example(U"Maze W/Mice", "B37/S12345");
    this->add_rule_example(U"Plow World", "B378/S012345678");
    this->add_rule_example(U"Coagulations", "B378/S235678");
    this->add_rule_example(U"HoneyLife", "B38/S238");
    this->add_rule_example(U"Electrified Maze", "B45/S12345");
    this->add_rule_example(U"Walled Cities", "B45678/S2345");
    this->add_rule_example(U"Vote 4/5", "B4678/S35678");
    this->add_rule_example(U"Vote", "B5678/S45678");
    this->add_rule_example(U"Star Trek", "B3/S0248");
}

}  // namespace gol

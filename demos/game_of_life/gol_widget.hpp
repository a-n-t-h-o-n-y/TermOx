#ifndef TERMOX_DEMOS_GAME_OF_LIFE_GOL_WIDGET_HPP
#define TERMOX_DEMOS_GAME_OF_LIFE_GOL_WIDGET_HPP
#include <chrono>
#include <cstdint>
#include <string>
#include <string_view>

#include <signals_light/signal.hpp>

#include <termox/painter/color.hpp>
#include <termox/painter/glyph.hpp>
#include <termox/painter/painter.hpp>
#include <termox/system/key.hpp>
#include <termox/system/mouse.hpp>
#include <termox/widget/area.hpp>
#include <termox/widget/point.hpp>
#include <termox/widget/widget.hpp>

#include "colors.hpp"
#include "game_of_life_engine.hpp"
#include "pattern.hpp"
#include "rule.hpp"

namespace gol {

class GoL_widget : public ox::Widget {
   public:
    using Period_t = std::chrono::milliseconds;

   public:
    GoL_widget();

   public:
    /// Change the animation rate, in milliseconds.
    void set_period(Period_t period);

    /// Start animation, no-op if already running.
    void start();

    /// Pause auto generation increment.
    void pause();

    /// Progress to the next iteration of the game.
    void step();

    /// Set Hi-Resolution Mode.
    void enable_hi_res(bool enabled = true);

    /// Alternate between Hi-Resolution mode and Low-Resolution Mode
    void toggle_hi_res();

    /// Set the birth/survivor neighbor counts.
    /** conwaylife.com/wiki/Rulestring
     *  B/S Format: B\d*\/S\d*
     *  S/B Format: \d*\/\d*
     *  Throws std::invalid_argument if string is not in proper format.
     */
    void set_rules(std::string_view rule_string);

    /// Directly set the Game of Life rules.
    void set_rules(Rule r);

    // TODO this and import pattern should be the same.
    /// Directly set the Pattern, rule; doesn't clear the screen before setting.
    void set_pattern(Pattern const& pattern);

    /// Clear the screen and reset the pattern to be all dead cells.
    void clear();

    /// Enable/disable a background grid for easier pattern creation.
    void toggle_grid();

    /// Import and reset the pattern to the file contents.
    /** File extension and headers are used to determine the filetype. Can
     * parse Life 1.05, Life 1.06, Plaintext, and RLE files. */
    void import_file(std::string const& filename);

    /// Import a pattern, set the rules, and reset the generation count.
    /** This does not clear the current alive cells. */
    void import_pattern(Pattern pattern);

    /// Export the current state as file.
    /** File extension is used to determine the filetype to export as. Can
     *  export as Life 1.05(.lif), Life 1.06(.life), Plaintext(.cell), and
     *  RLE(.rle) */
    void export_to_file(std::string const& filename);

    /// Set the engine coordinates that are at the center of the screen.
    void set_offset(Coordinate offset);

    /// Return the engine coordinates that are at the center of the screen.
    auto offset() const -> Coordinate;

    void enable_rainbow();

    void disable_rainbow();

    void toggle_rainbow();

   protected:
    auto paint_event(ox::Painter& p) -> bool override;

    auto mouse_press_event(ox::Mouse const& m) -> bool override;

    auto mouse_move_event(ox::Mouse const& m) -> bool override;

    auto mouse_wheel_event(ox::Mouse const& m) -> bool override;

    auto timer_event() -> bool override;

    auto key_press_event(ox::Key k) -> bool override;

    auto resize_event(ox::Area new_size, ox::Area old_size) -> bool override;

    auto enable_event() -> bool override;

    auto disable_event() -> bool override;

   private:
    Game_of_life_engine engine_;
    bool hi_res_ = true;
    bool grid_   = false;
    Period_t period_{40};
    Coordinate offset_{0, 0};
    ox::Color cell_color_ = color::White;
    bool running_         = false;

    int half_width_  = 0;
    int half_height_ = 0;

   private:
    /// Update the period if currently running.
    void update_period();

    /// Convert unsigned screen Point to engine Coordinate.
    /** Engine coordinate (0, 0) is at the center of the display. */
    auto transform_to_engine(ox::Point p) const -> Coordinate;

    void paint_low_res(ox::Painter& p);

    void paint_hi_res(ox::Painter& p);

    auto braille_at(ox::Point p) const -> char32_t;

    /// Find the braille representation of the Coordinates at \p p.
    auto braille_at(Coordinate c) const -> char32_t;

    void increment_at(Coordinate c);

    void decrement_at(Coordinate c);

    auto has_bit_set(char32_t subject, char32_t bit) -> bool;

    void change_state(Coordinate c, char32_t bits);

    void handle_mouse_event(ox::Mouse const& m);

   public:
    sl::Signal<void(Coordinate)> offset_changed;
    sl::Signal<void(Rule)> rule_changed;
    sl::Signal<void(std::uint32_t)>& generation_count_changed{
        engine_.generation_count_changed};
};

}  // namespace gol
#endif  // TERMOX_DEMOS_GAME_OF_LIFE_GOL_WIDGET_HPP

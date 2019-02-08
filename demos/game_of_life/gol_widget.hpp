#ifndef CPPURSES_DEMOS_GAME_OF_LIFE_GOL_WIDGET_HPP
#define CPPURSES_DEMOS_GAME_OF_LIFE_GOL_WIDGET_HPP
#include <chrono>
#include <string>

#include <signals/signal.hpp>

#include <cppurses/painter/glyph.hpp>
#include <cppurses/system/keyboard_data.hpp>
#include <cppurses/system/mouse_data.hpp>
#include <cppurses/widget/point.hpp>
#include <cppurses/widget/widget.hpp>

#include "game_of_life_engine.hpp"

namespace gol {

class GoL_widget : public cppurses::Widget {
   public:
    using Period_t = std::chrono::milliseconds;

    GoL_widget();

    /// Change the animation rate, in milliseconds.
    void set_period(Period_t period);

    /// Start animation, no-op if already running.
    void start();

    /// Pause auto generation increment.
    void pause();

    /// Progress to the next iteration of the game.
    void step();

    /// Change the appearance of the alive cells.
    void set_dead(const cppurses::Glyph& dead_look);

    /// Set whether the Cells' appearance fades with age.
    void enable_fade(bool enabled = true);

    /// Alternate between fade set and not set.
    void toggle_fade() { this->enable_fade(!fade_); }

    /// Set the birth/survivor neighbor counts.
    /** Provide two sets of numbers separated by a '/' or any non-digit
     *  separator. Each number is a neighbor count. */
    void set_rules(const std::string& rule_string);

    /// Clear the screen and reset the pattern to be all dead cells.
    void clear();

    /// Enable/disable a background grid for easier pattern creation.
    void toggle_grid();

    /// Import and reset the pattern to the file contents.
    /** File extension and headers are used to determine the filetype. Can parse
     *  Life 1.05, Life 1.06, Plaintext, and RLE files. */
    void import(const std::string& filename);

    /// Export the current state as file.
    /** File extension is used to determine the filetype to export as. Can
     *  export as Life 1.05(.lif), Life 1.06(.life), Plaintext(.cell), and
     *  RLE(.rle) */
    void export_as(const std::string& filename);

    /// Set the engine coordinates that are at the center of the screen.
    void set_offset(Coordinate offset);

    /// Return the engine coordinates that are at the center of the screen.
    Coordinate offset() const { return offset_; }

    sig::Signal<void(Coordinate)> offset_changed;
    sig::Signal<void(const std::string&)> rule_changed;

   protected:
    bool paint_event() override;
    bool mouse_press_event(const cppurses::Mouse_data& mouse) override;
    bool timer_event() override;
    bool key_press_event(const cppurses::Keyboard_data& keyboard) override;

   private:
    Game_of_life_engine engine_;
    cppurses::Glyph dead_look_{L' '};
    bool running_{false};
    bool fade_{true};
    bool grid_{false};
    Period_t period_{120};
    Coordinate offset_{0, 0};

    /// Update the period if currently running.
    void update_period();

    /// Convert signed Coordinates from engine to display positions.
    /** Engine coordinate (0,0) is at the center of the display. Return max
     *  values for x and y if transformation cannot fit on the display. */
    cppurses::Point transform_from_engine(Coordinate position) const;

    /// Convert unsigned display position to engine Coordinates.
    /** Engine coordinate (0,0) is at the center of the display. */
    Coordinate transform_from_display(cppurses::Point p) const;

    /// Return a Glyph to represent a Cell of a given \p age.
    cppurses::Glyph get_look(typename Cell::Age_t age) const;

   public:
    sig::Signal<void(std::uint32_t)>& generation_count_changed{
        engine_.generation_count_changed};
};
}  // namespace gol
#endif  // CPPURSES_DEMOS_GAME_OF_LIFE_GOL_WIDGET_HPP

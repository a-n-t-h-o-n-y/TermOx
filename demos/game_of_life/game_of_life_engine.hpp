#ifndef TERMOX_DEMOS_GAME_OF_LIFE_GAME_OF_LIFE_ENGINE_HPP
#define TERMOX_DEMOS_GAME_OF_LIFE_GAME_OF_LIFE_ENGINE_HPP
#include <cstdint>
#include <vector>

#include <signals_light/signal.hpp>

#include "bitset.hpp"
#include "coordinate.hpp"
#include "pattern.hpp"
#include "rule.hpp"

namespace gol {

/// Holds game state and provides an interface to update to the next pattern.
class Game_of_life_engine {
   public:
    sl::Signal<void(std::uint32_t)> generation_count_changed;

   public:
    /// Create a living cell at \p c and reset the generation count.
    /** No-op if already alive at \p c. */
    void create_life(Coordinate c);

    /// Kill cell at \p c and reset the generation count.
    /** No-op if no cell alive at \p c. */
    void kill(Coordinate c);

    /// Remove all living cells from the pattern and reset the generation count.
    void kill_all();

    /// Updates the engine state to the next generation of cells.
    void step_to_next_generation();

    /// Import a pattern, set the rules, and reset the generation count.
    /** This does not clear the current alive cells. */
    void import(Pattern const& pattern);

    /// Set the Game of Life rules for the engine to follow.
    void set_rules(Rule r);

   public:
    /// Begin iterator to boolean is_alive and Coordinate.
    [[nodiscard]] auto begin() const -> Bitset::const_iterator;

    /// End iterator to boolean is_alive and Coordinate.
    [[nodiscard]] auto end() const -> Bitset::const_iterator;

    /// Check if a cell is alive at the given Coordinate.
    [[nodiscard]] auto is_alive_at(Coordinate c) const -> bool;

    /// Return a copy of the currently set Rule.
    [[nodiscard]] auto rules() const -> Rule;

   private:
    Bitset cells_;
    std::vector<Coordinate> volatiles_;
    Rule rules_;
    std::uint32_t generation_count_ = 0;

   private:
    /// Add live cells and volatiles to the engine from a Pattern.
    /** Does not clear the state. */
    void add_cells(Pattern::Cells cells);

    /// Increment the generation count by 1 and emit generation_count_changed.
    void increment_generation_count();

    /// Set the generation count to 0 and emit generation_count_changed.
    void reset_generation_count();

    /// Add a live cell at the given position and adds to volatile cells.
    void add_cell_at(Coordinate c);

    /// Remove a live cell at the given position and adds to volatile cells.
    void remove_cell_at(Coordinate c);

    /// Make every live cell a volatile cell.
    /** Used when changing the Rule. */
    void make_all_volatile();
};

}  // namespace gol
#endif  // TERMOX_DEMOS_GAME_OF_LIFE_GAME_OF_LIFE_ENGINE_HPP

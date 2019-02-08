#ifndef CPPURSES_DEMOS_GAME_OF_LIFE_GAME_OF_LIFE_ENGINE_HPP
#define CPPURSES_DEMOS_GAME_OF_LIFE_GAME_OF_LIFE_ENGINE_HPP
#include <cstdint>
#include <iterator>
#include <map>
#include <set>

#include <fstream>

#include <signals/signal.hpp>

#include "cell.hpp"
#include "coordinate.hpp"

namespace gol {

/// Holds game state and provides an interface to update to the next pattern.
class Game_of_life_engine {
   public:
    /// Updates the engine state to the next generation of cells.
    void get_next_generation();

    /// Create a living cell at \p position and reset the generation count.
    /** No-op if already alive at \p position. */
    void give_life(Coordinate position);

    /// Kill cell at \p position and reset the generation count.
    /** No-op if no cell alive at \p position. */
    void kill(Coordinate position);

    /// Remove all living cells from the pattern and reset the generation count.
    void kill_all();

    /// Check if a cell is alive at the given Coordinate.
    bool alive_at(Coordinate position) const;

    /// Return the number of alive neighbors the given \p position has.
    int alive_neighbor_count(Coordinate position) const;

    /// Return const bidirectional iterator to beginning of [Coordinate, Cell].
    auto begin() const { return std::begin(alive_cells_); }

    /// Return const bidirectional iterator to ending of [Coordinate, Cell].
    auto end() const { return std::end(alive_cells_); }

    /// Import a container of alive cell positions, reset the generation count.
    template <typename Container_t>
    void import(const Container_t& cells) {
        for (Coordinate cell : cells) {
            this->add_cell_at(cell);
        }
        this->reset_generation_count();
    }

    /// Set the neighbor counts that allow survival for a living cell.
    template <typename Container_t>
    void set_survival_rule(const Container_t& neighbor_counts) {
        survival_rule_ = {std::begin(neighbor_counts),
                          std::end(neighbor_counts)};
        if (survival_rule_.empty()) {
            survival_rule_.insert(9); 
        }
    }

    /// Set the neighbor counts that allow new cells to form from dead cells.
    template <typename Container_t>
    void set_birth_rule(const Container_t& neighbor_counts) {
        birth_rule_ = {std::begin(neighbor_counts), std::end(neighbor_counts)};
    }

    sig::Signal<void(std::uint32_t)> generation_count_changed;

   private:
    std::map<Coordinate, Cell> alive_cells_;
    std::set<Coordinate> volatiles_;
    std::set<int> birth_rule_{3};
    std::set<int> survival_rule_{2, 3};
    std::uint32_t generation_count_{0};

    /// Add \p cell and its neighbors to the volatiles_ container.
    /** Volatiles are Coordinates that can potentially change state in the next
     *  iteration. */
    void add_volatiles(Coordinate cell);

    /// Set the generation count to 0 and emit generation_count_changed.
    void reset_generation_count();

    /// Increment the generation count by 1 and emit generation_count_changed.
    void increment_generation_count();

    /// Add an alive cell at the given position.
    void add_cell_at(Coordinate position);

    /// Remove an alive cell at the given position.
    void remove_cell_at(Coordinate position);
};
}  // namespace gol
#endif  // CPPURSES_DEMOS_GAME_OF_LIFE_GAME_OF_LIFE_ENGINE_HPP

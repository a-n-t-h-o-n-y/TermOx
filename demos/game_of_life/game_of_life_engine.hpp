#ifndef CPPURSES_DEMOS_GAME_OF_LIFE_GAME_OF_LIFE_ENGINE_HPP
#define CPPURSES_DEMOS_GAME_OF_LIFE_GAME_OF_LIFE_ENGINE_HPP
#include <iterator>
#include <map>
#include <set>

#include "cell.hpp"
#include "coordinate.hpp"

namespace gol {

/// Holds game state and provides an interface to update to the next pattern.
class Game_of_life_engine {
   public:
    /// Updates the engine to the next iteration of the state.
    void next_iteration();

    /// Create a living cell at the given Coordinate.
    void give_life(Coordinate position);

    /// Kill a cell at the given Coordinate.
    void kill(Coordinate cell);

    /// Removes all living cells from the pattern.
    void kill_all();

    /// Checks if a cell is alive at the given Coordinate.
    bool alive_at(Coordinate position) const;

    /// Returns the number of neighbors the given cell Coordinate has.
    int alive_neighbor_count(Coordinate position) const;

    /// Return const bidirectional iterator to beginning of [Coordinate, Cell].
    auto begin() const { return std::begin(alive_cells_); }

    /// Return const bidirectional iterator to ending of [Coordinate, Cell].
    auto end() const { return std::end(alive_cells_); }

    /// Set the neighbor counts that allow survival for a living cell.
    template <typename Container_t>
    void set_survival_rule(const Container_t& neighbor_counts) {
        survival_rule_ = {std::begin(neighbor_counts),
                          std::end(neighbor_counts)};
    }

    /// Set the neighbor counts that allow new cells to form from dead cells.
    template <typename Container_t>
    void set_birth_rule(const Container_t& neighbor_counts) {
        birth_rule_ = {std::begin(neighbor_counts), std::end(neighbor_counts)};
    }

    /// Reset the game state to the Coordinates from [first, last).
    template <typename Forward_iter_t>
    void import(Forward_iter_t first, Forward_iter_t last) {
        this->kill_all();
        while (first != last) {
            this->give_life(*first);
        }
    }

   private:
    std::map<Coordinate, Cell> alive_cells_;
    std::set<Coordinate> volatiles_;
    std::set<int> birth_rule_{3};
    std::set<int> survival_rule_{2, 3};

    /// Add \p cell and its neighbors to the volatiles_ container.
    /** Volatiles are Coordinates that can potentially change state in the next
     *  iteration. */
    void add_volatiles(Coordinate cell);
};

}  // namespace gol
#endif  // CPPURSES_DEMOS_GAME_OF_LIFE_GAME_OF_LIFE_ENGINE_HPP

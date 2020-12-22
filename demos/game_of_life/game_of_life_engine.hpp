#ifndef TERMOX_DEMOS_GAME_OF_LIFE_GAME_OF_LIFE_ENGINE_HPP
#define TERMOX_DEMOS_GAME_OF_LIFE_GAME_OF_LIFE_ENGINE_HPP
#include <algorithm>
#include <cstdint>
#include <iterator>
#include <unordered_map>
#include <unordered_set>

#include <signals_light/signal.hpp>

#include "bitset.hpp"
#include "cell.hpp"
#include "coordinate.hpp"
#include "coordinates.hpp"
#include "pattern.hpp"
#include "rule.hpp"

namespace gol {

/// Holds game state and provides an interface to update to the next pattern.
class Game_of_life_engine {
   public:
    sl::Signal<void(std::uint32_t)> generation_count_changed;

   public:
    /// Updates the engine state to the next generation of cells.
    void get_next_generation()
    {
        auto const volatiles = std::move(volatiles_);
        auto temp_cells      = cells_;
        for (auto c : volatiles) {
            auto const neighbor_count = this->alive_neighbor_count(c);
            auto const is_alive       = alive_at(c);
            if (is_alive && !contains(neighbor_count, rules_.survival)) {
                temp_cells.remove(c);
                // coordinates_.stage_removal(c);
                this->add_volatiles(c);
            }
            else if (!is_alive && contains(neighbor_count, rules_.birth)) {
                temp_cells.insert(c);
                // coordinates_.push_back(c);
                this->add_volatiles(c);
            }
        }
        // coordinates_.commit_removals();
        this->increment_generation_count();
        std::swap(cells_, temp_cells);
    }

    /// Create a living cell at \p c and reset the generation count.
    /** No-op if already alive at \p c. */
    void create_life(Coordinate c)
    {
        if (!alive_at(c)) {
            this->add_cell_at(c);
            this->reset_generation_count();
        }
    }

    /// Kill cell at \p c and reset the generation count.
    /** No-op if no cell alive at \p c. */
    void kill(Coordinate c)
    {
        if (this->alive_at(c)) {
            this->remove_cell_at(c);
            this->reset_generation_count();
        }
    }

    /// Remove all living cells from the pattern and reset the generation count.
    void kill_all()
    {
        cells_.clear();
        // coordinates_.clear();
        volatiles_.clear();
        this->reset_generation_count();
    }

    /// Check if a cell is alive at the given Coordinate.
    auto alive_at(Coordinate c) const -> bool { return cells_.contains(c); }

    /// Return the number of alive neighbors the given \p c has.
    auto alive_neighbor_count(Coordinate c) const -> int
    {
        auto count = 0;
        for (Coordinate neighbor : neighbors(c))
            count += alive_at(neighbor) ? 1 : 0;
        return count;
    }

    /// Number of alive cells.
    // TODO remove this and call site
    // auto alive_cell_count() const -> std::size_t { return
    // coordinates_.size(); }

    /// Begin iterator to boolean is_alive and Coordinate.
    auto begin() const { return std::cbegin(cells_); }

    /// End iterator to boolean is_alive and Coordinate.
    auto end() const { return std::cend(cells_); }

    /// Add live cells to the engine, does not clear the current state.
    void add_cells(Pattern::Cells cells)
    {
        for (Coordinate c : cells) {
            if (!alive_at(c))
                this->add_cell_at(c);
        }
    }

    /// Import a pattern, set the rules, and reset the generation count.
    /** This does not clear the current alive cells. */
    void import(Pattern const& pattern)
    {
        this->add_cells(pattern.cells);
        this->set_rules(pattern.rule);
        this->reset_generation_count();
    }

    /// Set the Game of Life rules for the engine to follow.
    void set_rules(Rule r)
    {
        rules_ = r;
        this->make_all_volatile();
    }

   private:
    // Fast Lookup
    Bitset cells_;

    // Duplicate for Fast Iteration
    // Coordinates coordinates_;

    std::unordered_set<Coordinate> volatiles_;
    Rule rules_;
    std::uint32_t generation_count_ = 0;

   private:
    /// Add \p cell and its neighbors to the volatiles_ container.
    /** Volatiles are Coordinates that can potentially change state in the next
     *  iteration. */
    void add_volatiles(Coordinate c)
    {
        volatiles_.insert(c);
        for (Coordinate neighbor : neighbors(c))
            volatiles_.insert(neighbor);
    }

    /// Set the generation count to 0 and emit generation_count_changed.
    void reset_generation_count()
    {
        generation_count_ = 0;
        generation_count_changed(generation_count_);
    }

    /// Increment the generation count by 1 and emit generation_count_changed.
    void increment_generation_count()
    {
        ++generation_count_;
        generation_count_changed(generation_count_);
    }

    /// Add an alive cell at the given position.
    void add_cell_at(Coordinate c)
    {
        this->add_volatiles(c);
        cells_.insert(c);
        // coordinates_.push_back(c);
    }

    /// Remove an alive cell at the given position.
    void remove_cell_at(Coordinate c)
    {
        this->add_volatiles(c);
        cells_.remove(c);
        // coordinates_.immediate_remove(c);
    }

    /// Make every alive cell a volatile cell, needed for changing the rules.
    void make_all_volatile()
    {
        for (auto const cell : cells_) {
            if (cell.is_alive)
                volatiles_.insert(cell.coordinate);
        }
    }

    static auto contains(int count, Rule::Neighbors const& neighbor_counts)
        -> bool
    {
        return neighbor_counts[count];
    }

    static auto neighbors(Coordinate c) -> std::array<Coordinate, 8>
    {
        auto const north      = Coordinate{c.x, c.y - 1};
        auto const south      = Coordinate{c.x, c.y + 1};
        auto const east       = Coordinate{c.x + 1, c.y};
        auto const west       = Coordinate{c.x - 1, c.y};
        auto const north_west = Coordinate{c.x - 1, c.y - 1};
        auto const north_east = Coordinate{c.x + 1, c.y - 1};
        auto const south_east = Coordinate{c.x + 1, c.y + 1};
        auto const south_west = Coordinate{c.x - 1, c.y + 1};
        return {north,      south,      east,       west,
                north_west, north_east, south_east, south_west};
    }
};

}  // namespace gol
#endif  // TERMOX_DEMOS_GAME_OF_LIFE_GAME_OF_LIFE_ENGINE_HPP

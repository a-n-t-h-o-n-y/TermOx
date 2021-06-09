#include "game_of_life_engine.hpp"

#include <algorithm>
#include <array>
#include <future>
#include <iterator>
#include <thread>
#include <utility>
#include <vector>

namespace {
using namespace gol;

struct Cell_state {
    Coordinate at;
    bool is_alive;
};

using Diff = std::vector<Cell_state>;

auto constexpr outer_bound = 5'000;

/// Remove/Insert Life to \p cells according to \p diff.
void merge(Bitset& cells, Diff const& diff)
{
    for (auto x : diff) {
        if (x.is_alive)
            cells.insert(x.at);
        else
            cells.remove(x.at);
    }
}

/// Removed duplicate elements.
void compress(std::vector<Coordinate>& x)
{
    std::sort(std::begin(x), std::end(x));
    auto const end = std::unique(std::begin(x), std::end(x));
    x.erase(end, std::end(x));
}

[[nodiscard]] auto neighbors(Coordinate c) -> std::array<Coordinate, 8>

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

/// Add \p c and its neighbors to the \p v volatiles container.
/** Volatiles are Coords that can potentially change state in the next step. */
void add_volatiles(std::vector<Coordinate>& v, Coordinate c)
{
    if (c.x > outer_bound || c.x < -outer_bound || c.y > outer_bound ||
        c.y < -outer_bound) {
        return;
    }
    v.push_back(c);
    for (Coordinate neighbor : neighbors(c))
        v.push_back(neighbor);
}

/// Return the number of alive neighbors the given \p c has.
[[nodiscard]] auto alive_neighbor_count(Bitset const& cells, Coordinate c)
    -> int
{
    auto count = 0;
    for (Coordinate neighbor : neighbors(c))
        count += cells.contains(neighbor) ? 1 : 0;
    return count;
}

/// Calculates a diff of the current to next + volatiles from this change.
/** Returns the Diff and volatiles as a pair. */
[[nodiscard]] auto next_generation_diff(
    Bitset const& cells,
    std::vector<Coordinate>::const_iterator first_volatile,
    std::vector<Coordinate>::const_iterator last_volatile,
    Rule const& rules) -> std::pair<Diff, std::vector<Coordinate>>
{
    auto const v_count = std::distance(first_volatile, last_volatile);

    auto diff = Diff{};
    diff.reserve(v_count);
    auto next_volatiles = std::vector<Coordinate>{};
    next_volatiles.reserve(v_count);

    for (auto iter = first_volatile; iter != last_volatile; ++iter) {
        auto const neighbor_count = alive_neighbor_count(cells, *iter);
        auto const is_alive       = cells.contains(*iter);
        if (is_alive && !rules.survival[neighbor_count]) {
            diff.push_back({*iter, false});
            add_volatiles(next_volatiles, *iter);
        }
        else if (!is_alive && rules.birth[neighbor_count]) {
            diff.push_back({*iter, true});
            add_volatiles(next_volatiles, *iter);
        }
    }
    return {diff, next_volatiles};
}

void series_step_to_next_generation(Bitset& cells,
                                    std::vector<Coordinate>& volatiles,
                                    Rule const& rules)
{
    auto [diff, v] = next_generation_diff(cells, std::cbegin(volatiles),
                                          std::cend(volatiles), rules);
    merge(cells, diff);
    volatiles = std::move(v);
}

void par_step_to_next_generation(Bitset& cells,
                                 std::vector<Coordinate>& volatiles,
                                 Rule const& rules)
{
    // Break volatiles Into Chunks
    auto const thread_count   = std::thread::hardware_concurrency();
    auto const chunk_distance = volatiles.size() / thread_count;

    // Launch Threads
    auto futures =
        std::vector<std::future<std::pair<Diff, std::vector<Coordinate>>>>{};
    for (auto i = 0uL; i < thread_count; ++i) {
        futures.push_back(std::async(
            std::launch::async,
            [i, chunk_distance, &volatiles, &cells, &rules] {
                auto const first_iter =
                    std::cbegin(volatiles) + (i * chunk_distance);
                return next_generation_diff(cells, first_iter,
                                            first_iter + chunk_distance, rules);
            }));
    }
    // Leftovers, if any.
    futures.push_back(std::async(std::launch::async, [&] {
        auto const first_iter =
            std::cbegin(volatiles) + (thread_count * chunk_distance);
        return next_generation_diff(cells, first_iter, std::cend(volatiles),
                                    rules);
    }));

    // Wait on all, then combine results.
    // volatiles needs to be cleared, but it is shared by all threads, wait.
    for (auto& f : futures)
        f.wait();

    volatiles.clear();
    for (auto& f : futures) {
        auto [diff, vs] = f.get();
        merge(cells, diff);
        volatiles.insert(std::end(volatiles), std::begin(vs), std::end(vs));
    }
}

}  // namespace

namespace gol {

void Game_of_life_engine::step_to_next_generation()
{
    compress(volatiles_);
    if (volatiles_.size() < 100'000)
        series_step_to_next_generation(cells_, volatiles_, rules_);
    else
        par_step_to_next_generation(cells_, volatiles_, rules_);
    this->increment_generation_count();
}

void Game_of_life_engine::create_life(Coordinate c)
{
    if (!cells_.contains(c)) {
        this->add_cell_at(c);
        this->reset_generation_count();
    }
}

void Game_of_life_engine::kill(Coordinate c)
{
    if (cells_.contains(c)) {
        this->remove_cell_at(c);
        this->reset_generation_count();
    }
}

void Game_of_life_engine::kill_all()
{
    cells_.clear();
    volatiles_.clear();
    this->reset_generation_count();
}

auto Game_of_life_engine::begin() const -> Bitset::const_iterator
{
    return std::cbegin(cells_);
}

auto Game_of_life_engine::end() const -> Bitset::const_iterator
{
    return std::cend(cells_);
}

auto Game_of_life_engine::is_alive_at(Coordinate c) const -> bool
{
    return cells_.contains(c);
}

void Game_of_life_engine::add_cells(Pattern::Cells cells)
{
    for (Coordinate c : cells) {
        if (!cells_.contains(c))
            this->add_cell_at(c);
    }
}

void Game_of_life_engine::import(Pattern const& pattern)
{
    this->add_cells(pattern.cells);
    this->set_rules(pattern.rule);
    this->reset_generation_count();
}

void Game_of_life_engine::set_rules(Rule r)
{
    rules_ = r;
    this->make_all_volatile();
}

auto Game_of_life_engine::rules() const -> Rule { return rules_; }

void Game_of_life_engine::reset_generation_count()
{
    generation_count_ = 0;
    generation_count_changed(generation_count_);
}

void Game_of_life_engine::increment_generation_count()
{
    ++generation_count_;
    generation_count_changed(generation_count_);
}

void Game_of_life_engine::add_cell_at(Coordinate c)
{
    add_volatiles(volatiles_, c);
    cells_.insert(c);
}

void Game_of_life_engine::remove_cell_at(Coordinate c)
{
    add_volatiles(volatiles_, c);
    cells_.remove(c);
}

void Game_of_life_engine::make_all_volatile()
{
    for (auto const cell : cells_) {
        if (cell.is_alive)
            volatiles_.push_back(cell.coordinate);
    }
}

}  // namespace gol

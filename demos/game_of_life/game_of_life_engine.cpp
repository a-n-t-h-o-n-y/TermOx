#include "game_of_life_engine.hpp"

#include <array>
#include <set>

#include "coordinate.hpp"

namespace {
using namespace gol;

template <typename Map_t>
bool contains(const typename Map_t::key_type& key, const Map_t& values) {
    return values.count(key) > 0;
}

std::array<Coordinate, 8> neighbors(Coordinate position) {
    const Coordinate north{position.x, position.y - 1};
    const Coordinate sound{position.x, position.y + 1};
    const Coordinate east{position.x + 1, position.y};
    const Coordinate west{position.x - 1, position.y};
    const Coordinate north_west{position.x - 1, position.y - 1};
    const Coordinate north_east{position.x + 1, position.y - 1};
    const Coordinate south_east{position.x + 1, position.y + 1};
    const Coordinate south_west{position.x - 1, position.y + 1};
    return {north,      sound,      east,       west,
            north_west, north_east, south_east, south_west};
}
}  // namespace

namespace gol {

void Game_of_life_engine::next_iteration() {
    const auto volatiles_copy = volatiles_;
    volatiles_.clear();
    std::set<Coordinate> to_kill;
    std::set<Coordinate> to_create;
    for (Coordinate position : volatiles_copy) {
        const auto neighbor_count = this->alive_neighbor_count(position);
        if (alive_at(position)) {
            if (!contains(neighbor_count, survival_rule_)) {
                to_kill.insert(position);
            }
        } else {
            if (contains(neighbor_count, birth_rule_)) {
                to_create.insert(position);
            }
        }
    }
    for (Coordinate position : to_kill) {
        this->kill(position);
    }
    for (auto& coord_cell : alive_cells_) {
        ++coord_cell.second.age;
    }
    for (Coordinate position : to_create) {
        this->give_life(position);
    }
}

void Game_of_life_engine::give_life(Coordinate position) {
    if (!alive_at(position)) {
        this->add_volatiles(position);
        alive_cells_[position] = Cell{};
    }
}

void Game_of_life_engine::kill(Coordinate position) {
    if (this->alive_at(position)) {
        this->add_volatiles(position);
        alive_cells_.erase(position);
    }
}

void Game_of_life_engine::kill_all() {
    alive_cells_.clear();
    volatiles_.clear();
}

bool Game_of_life_engine::alive_at(Coordinate position) const {
    return contains(position, alive_cells_);
}

int Game_of_life_engine::alive_neighbor_count(Coordinate position) const {
    int count{0};
    for (Coordinate neighbor : neighbors(position)) {
        count += alive_at(neighbor) ? 1 : 0;
    }
    return count;
}

void Game_of_life_engine::add_volatiles(Coordinate position) {
    volatiles_.insert(position);
    for (Coordinate neighbor : neighbors(position)) {
        volatiles_.insert(neighbor);
    }
}
}  // namespace gol

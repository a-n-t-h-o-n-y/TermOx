#ifndef TERMOX_DEMOS_GAME_OF_LIFE_COORDINATES_HPP
#define TERMOX_DEMOS_GAME_OF_LIFE_COORDINATES_HPP
#include <algorithm>
#include <vector>

#include "coordinate.hpp"

namespace gol {

class Coordinates {
   private:
    using Container_t = std::vector<Coordinate>;

   public:
    using iterator       = Container_t::iterator;
    using const_iterator = Container_t::const_iterator;

   public:
    void push_back(Coordinate c) { coordinates_.push_back(c); }

    /// Marks the \p c for removal, modifies the value but does not remove.
    /** Assumes the container has unique values and only marks the first. */
    void stage_removal(Coordinate c)
    {
        auto const iter =
            std::find(std::begin(coordinates_), std::end(coordinates_), c);
        if (iter != std::end(coordinates_))
            iter->x = Coordinate::Invalid;
    }

    /// Erase all Coordinates that have been marked by stage_removal().
    void commit_removals()
    {
        auto const iter = std::remove_if(
            std::begin(coordinates_), std::end(coordinates_),
            [](auto coord) { return coord.x == Coordinate::Invalid; });
        coordinates_.erase(iter, std::end(coordinates_));
    }

    /// Immediately erase the element \p c from the container.
    void immediate_remove(Coordinate c)
    {
        auto const iter =
            std::find(std::begin(coordinates_), std::end(coordinates_), c);
        if (iter != std::end(coordinates_))
            coordinates_.erase(iter);
    }

    /// Remove all elements, size == 0 after call.
    void clear() { coordinates_.clear(); }

    /// Number of elements stored.
    auto size() const -> std::size_t { return coordinates_.size(); }

    /// Const Begin Iterator
    auto begin() const -> const_iterator { return std::cbegin(coordinates_); }

    /// Const End Iterator
    auto end() const -> const_iterator { return std::cend(coordinates_); }

    auto begin() -> iterator { return std::begin(coordinates_); }
    auto end() -> iterator { return std::end(coordinates_); }

   private:
    Container_t coordinates_;
};

}  // namespace gol
#endif  // TERMOX_DEMOS_GAME_OF_LIFE_COORDINATES_HPP

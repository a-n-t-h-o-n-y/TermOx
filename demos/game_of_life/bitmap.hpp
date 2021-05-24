#ifndef TERMOX_DEMOS_GAME_OF_LIFE_BITMAP_HPP
#define TERMOX_DEMOS_GAME_OF_LIFE_BITMAP_HPP
#include <cstdint>
#include <cstddef>
#include <vector>

#include "cell.hpp"
#include "coordinate.hpp"

namespace gol {

/// Dynamically Resizing 2D Bitmap with std::int16_t Dimensions, holding Cells.
/** Can replace a std::(unordered_)map<Coordinate, Cell>. */
class Bitmap {
   public:
    using Dimension_t = std::int16_t;
    using Container_t = std::vector<Cell>;

   public:
    void insert(Coordinate c, Cell value);

    void erase(Coordinate c);

    auto contains(Coordinate c) const -> bool;

    auto find(Coordinate c) const -> typename Container_t::const_iterator;

    auto find(Coordinate c) -> typename Container_t::iterator;

    /// Sets all bits to false.
    void clear();

   private:
    Dimension_t side_length_ = 100;
    Dimension_t half_length_ = side_length_ / 2;
    Container_t values_ =
        Container_t(side_length_ * side_length_, Cell{Cell::Age_t(-1)});

    /// Lowest possible value for a Coordinate dimension.
    Dimension_t lower_bound_ = -half_length_;

    /// Largest possible value for a Coordinate dimension.
    Dimension_t upper_bound_ = half_length_ - 1;

   private:
    /// Visual Coordinate to vector index.
    auto get_offset(Coordinate c) const -> std::size_t;

    /// Add more memory to the Bitmap to handle larger sizes.
    void resize(Dimension_t new_side_length);

    /// Return true if c is out of bounds of the memory allocated.
    auto out_of_bounds(Coordinate c) const -> bool;
};

}  // namespace gol
#endif  // TERMOX_DEMOS_GAME_OF_LIFE_BITMAP_HPP

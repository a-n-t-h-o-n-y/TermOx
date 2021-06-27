#ifndef TERMOX_DEMOS_GAME_OF_LIFE_BITSET_HPP
#define TERMOX_DEMOS_GAME_OF_LIFE_BITSET_HPP
#include <cstddef>
#include <cstdint>
#include <vector>

#include "coordinate.hpp"

namespace gol {

/// Dynamically Resizing 2D Bitset with std::int16_t Dimensions.
/** Can replace a std::(unordered_)set<Coordinate>, this is much faster. */
class Bitset {
   public:
    using Dimension_t = std::int16_t;
    using Container_t = std::vector<bool>;

   public:
    /// Keeps track of the current Coordinate the iterator is at.
    class Iterator {
       public:
        struct Value {
            Container_t::const_reference is_alive;
            Coordinate coordinate;
        };

       public:
        Iterator(Container_t::const_iterator i,
                 Dimension_t lower,
                 Dimension_t upper);

       public:
        auto operator++() -> Iterator&;

        auto operator*() const -> Value;

        friend auto operator<(Iterator const& x, Iterator const& y) -> bool;

        friend auto operator==(Iterator const& x, Iterator const& y) -> bool;

        friend auto operator!=(Iterator const& x, Iterator const& y) -> bool;

       private:
        Container_t::const_iterator i_;
        Dimension_t lower_;
        Dimension_t upper_;
        Coordinate c_{lower_, lower_};
    };

   public:
    using const_iterator = Iterator;

   public:
    void insert(Coordinate c);

    void remove(Coordinate c);

    [[nodiscard]] auto contains(Coordinate c) const -> bool;

    [[nodiscard]] auto size() const -> std::size_t { return values_.size(); }

    /// Sets all bits to false.
    void clear();

    /// Begin iterator to boolean and Coordinate.
    /** This container is usually fairly large. */
    [[nodiscard]] auto begin() const -> const_iterator;

    /// End iterator to boolean and Coordinate.
    /** This container is usually fairly large. */
    [[nodiscard]] auto end() const -> const_iterator;

   private:
    Dimension_t side_length_ = 100;
    Dimension_t half_length_ = side_length_ / 2;
    Container_t values_      = Container_t(side_length_ * side_length_, false);

    /// Lowest possible value for a Coordinate dimension.
    Dimension_t lower_bound_ = -half_length_;

    /// Largest possible value for a Coordinate dimension.
    Dimension_t upper_bound_ = half_length_ - 1;

   private:
    /// Visual Coordinate to vector index.
    auto get_offset(Coordinate c) const -> std::size_t;

    /// Add more memory to the Bitset to handle larger sizes.
    /** The size passed in should be the length of one side of a square. */
    void resize(Dimension_t new_side_length);

    /// Return true if c is out of bounds of the memory allocated.
    auto out_of_bounds(Coordinate c) const -> bool;
};

}  // namespace gol
#endif  // TERMOX_DEMOS_GAME_OF_LIFE_BITSET_HPP

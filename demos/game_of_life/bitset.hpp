#ifndef TERMOX_DEMOS_GAME_OF_LIFE_BITSET_HPP
#define TERMOX_DEMOS_GAME_OF_LIFE_BITSET_HPP
#include <algorithm>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <utility>
#include <vector>

#include "coordinate.hpp"

namespace gol {

/// Dynamically Resizing 2D Bitset with std::int16_t Dimensions.
/** Can replace a std::(unordered_)set<Coordinate>, this is much faster. */
class Bitset {
   private:
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
                 Dimension_t upper)
            : i_{i}, lower_{lower}, upper_{upper}
        {}

       public:
        auto operator++() -> Iterator&
        {
            ++i_;
            if (++c_.x > upper_) {
                c_.x = lower_;
                ++c_.y;
            }
            return *this;
        }

        auto operator*() const -> Value { return {*i_, c_}; }

        friend auto operator<(Iterator const& x, Iterator const& y) -> bool
        {
            return x.i_ < y.i_;
        }

        friend auto operator==(Iterator const& x, Iterator const& y) -> bool
        {
            return x.i_ == y.i_;
        }

        friend auto operator!=(Iterator const& x, Iterator const& y) -> bool
        {
            return x.i_ != y.i_;
        }

       private:
        Container_t::const_iterator i_;
        Dimension_t lower_;
        Dimension_t upper_;
        Coordinate c_{lower_, lower_};
    };

   public:
    using const_iterator = Iterator;

   public:
    void insert(Coordinate c)
    {
        if (this->out_of_bounds(c))
            this->resize(4 * std::max(std::abs(c.x), std::abs(c.y)));
        values_[this->get_offset(c)] = true;
    }

    void remove(Coordinate c)
    {
        if (this->out_of_bounds(c))
            return;
        values_[this->get_offset(c)] = false;
    }

    auto contains(Coordinate c) const -> bool
    {
        if (this->out_of_bounds(c))
            return false;
        return values_[this->get_offset(c)];
    }

    /// Sets all bits to false.
    void clear() { values_ = Container_t(side_length_ * side_length_, false); }

    /// Begin iterator to boolean and Coordinate.
    /** This container is usually fairly large. */
    auto begin() const -> const_iterator
    {
        return {std::cbegin(values_), lower_bound_, upper_bound_};
    }

    /// End iterator to boolean and Coordinate.
    /** This container is usually fairly large. */
    auto end() const -> const_iterator
    {
        return {std::cend(values_), lower_bound_, upper_bound_};
    }

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
    auto get_offset(Coordinate c) const -> std::size_t
    {
        return offset(c, side_length_, half_length_);
    }

    /// Visual Coordinate to vector index.
    static auto offset(Coordinate c,
                       Dimension_t length,
                       Dimension_t half_length) -> std::size_t
    {
        return (c.x + half_length) + ((c.y + half_length) * length);
        // c.x: index - half_length
        // c.y:  / length
    }

    /// Add more memory to the Bitmap to handle larger sizes.
    /// The size passed in should be the length of one side of a square.
    void resize(Dimension_t new_side_length)
    {
        auto const new_half_length = new_side_length / 2;
        auto new_values = Container_t(new_side_length * new_side_length, false);

        for (auto const cell : *this) {
            if (cell.is_alive) {
                new_values[offset(cell.coordinate, new_side_length,
                                  new_half_length)] = true;
            }
        }

        // auto c = Coordinate{lower_bound_, lower_bound_};
        // for (auto b : values_) {
        //     if (b)
        //         new_values[offset(c, new_side_length, new_half_length)] = true;
        //     if (++c.x > upper_bound_) {
        //         c.x = lower_bound_;
        //         ++c.y;
        //     }
        // }

        values_      = std::move(new_values);
        side_length_ = new_side_length;
        half_length_ = side_length_ / 2;
        lower_bound_ = -half_length_;
        upper_bound_ = half_length_ - 1;
    }

    /// Return true if c is out of bounds of the memory allocated.
    auto out_of_bounds(Coordinate c) const -> bool
    {
        return c.x < lower_bound_ || c.x > upper_bound_ || c.y < lower_bound_ ||
               c.y > upper_bound_;
    }
};

}  // namespace gol
#endif  // TERMOX_DEMOS_GAME_OF_LIFE_BITSET_HPP

#include "bitset.hpp"

#include <algorithm>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <iterator>
#include <utility>
#include <vector>

#include "coordinate.hpp"

namespace {

/// Visual Coordinate to vector index.
auto offset(gol::Coordinate c,
            gol::Bitset::Dimension_t length,
            gol::Bitset::Dimension_t half_length) -> std::size_t
{
    return (c.x + half_length) + ((c.y + half_length) * length);
    // c.x: index - half_length
    // c.y:  / length
}

}  // namespace

namespace gol {

Bitset::Iterator::Iterator(Container_t::const_iterator i,
                           Dimension_t lower,
                           Dimension_t upper)
    : i_{i}, lower_{lower}, upper_{upper}
{}

auto Bitset::Iterator::operator++() -> Iterator&
{
    ++i_;
    if (++c_.x > upper_) {
        c_.x = lower_;
        ++c_.y;
    }
    return *this;
}

auto Bitset::Iterator::operator*() const -> Value { return {*i_, c_}; }

auto operator<(Bitset::Iterator const& x, Bitset::Iterator const& y) -> bool
{
    return x.i_ < y.i_;
}

auto operator==(Bitset::Iterator const& x, Bitset::Iterator const& y) -> bool
{
    return x.i_ == y.i_;
}

auto operator!=(Bitset::Iterator const& x, Bitset::Iterator const& y) -> bool
{
    return x.i_ != y.i_;
}

void Bitset::insert(Coordinate c)
{
    if (this->out_of_bounds(c))
        this->resize(4 * std::max(std::abs(c.x), std::abs(c.y)));
    values_[this->get_offset(c)] = true;
}

void Bitset::remove(Coordinate c)
{
    if (this->out_of_bounds(c))
        return;
    values_[this->get_offset(c)] = false;
}

auto Bitset::contains(Coordinate c) const -> bool
{
    if (this->out_of_bounds(c))
        return false;
    return values_[this->get_offset(c)];
}

void Bitset::clear()
{
    values_ = Container_t(side_length_ * side_length_, false);
}

auto Bitset::begin() const -> const_iterator
{
    return {std::cbegin(values_), lower_bound_, upper_bound_};
}

auto Bitset::end() const -> const_iterator
{
    return {std::cend(values_), lower_bound_, upper_bound_};
}

auto Bitset::get_offset(Coordinate c) const -> std::size_t
{
    return ::offset(c, side_length_, half_length_);
}

void Bitset::resize(Dimension_t new_side_length)
{
    auto const new_half_length = new_side_length / 2;
    auto new_values = Container_t(new_side_length * new_side_length, false);

    for (auto const cell : *this) {
        if (cell.is_alive) {
            new_values[::offset(cell.coordinate, new_side_length,
                                new_half_length)] = true;
        }
    }

    values_      = std::move(new_values);
    side_length_ = new_side_length;
    half_length_ = side_length_ / 2;
    lower_bound_ = -half_length_;
    upper_bound_ = half_length_ - 1;
}

auto Bitset::out_of_bounds(Coordinate c) const -> bool
{
    return c.x < lower_bound_ || c.x > upper_bound_ || c.y < lower_bound_ ||
           c.y > upper_bound_;
}

}  // namespace gol

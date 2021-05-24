#include "bitmap.hpp"

#include <algorithm>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <iterator>
#include <utility>
#include <vector>

#include "cell.hpp"
#include "coordinate.hpp"

namespace {

/// Visual Coordinate to vector index.
auto offset(gol::Coordinate c,
            gol::Bitmap::Dimension_t length,
            gol::Bitmap::Dimension_t half_length) -> std::size_t
{
    return (c.x + half_length) + ((c.y + half_length) * length);
}

}  // namespace

namespace gol {

void Bitmap::insert(Coordinate c, Cell value)
{
    if (this->out_of_bounds(c))
        this->resize(4 * std::max(std::abs(c.x), std::abs(c.y)));
    values_[this->get_offset(c)] = value;
}

void Bitmap::erase(Coordinate c)
{
    if (this->out_of_bounds(c))
        return;
    values_[this->get_offset(c)] = Cell{Cell::Age_t(-1)};
}

auto Bitmap::contains(Coordinate c) const -> bool
{
    if (this->out_of_bounds(c))
        return false;
    return values_[this->get_offset(c)].age != Cell::Age_t(-1);
}

auto Bitmap::find(Coordinate c) const -> typename Container_t::const_iterator
{
    return std::next(std::cbegin(values_), this->get_offset(c));
}

auto Bitmap::find(Coordinate c) -> typename Container_t::iterator
{
    return std::next(std::begin(values_), this->get_offset(c));
}

void Bitmap::clear()
{
    values_ = Container_t(side_length_ * side_length_, Cell{Cell::Age_t(-1)});
}

auto Bitmap::get_offset(Coordinate c) const -> std::size_t
{
    return ::offset(c, side_length_, half_length_);
}

/// Add more memory to the Bitmap to handle larger sizes.
void Bitmap::resize(Dimension_t new_side_length)
{
    auto const new_half_length = new_side_length / 2;
    auto new_values =
        Container_t(new_side_length * new_side_length, Cell{Cell::Age_t(-1)});

    auto c = Coordinate{0, 0};
    for (auto v : values_) {
        if (++c.x == upper_bound_) {
            c.x = lower_bound_;
            ++c.y;
        }
        new_values[::offset(c, new_side_length, new_half_length)] =
            std::move(v);
    }

    values_      = std::move(new_values);
    side_length_ = new_side_length;
    half_length_ = side_length_ / 2;
    lower_bound_ = -half_length_;
    upper_bound_ = half_length_ - 1;
}

/// Return true if c is out of bounds of the memory allocated.
auto Bitmap::out_of_bounds(Coordinate c) const -> bool
{
    return c.x < lower_bound_ || c.x > upper_bound_ || c.y < lower_bound_ ||
           c.y > upper_bound_;
}

}  // namespace gol

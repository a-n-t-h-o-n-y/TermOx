#ifndef TERMOX_WIDGET_WIDGETS_GRAPH_HPP
#define TERMOX_WIDGET_WIDGETS_GRAPH_HPP
#include <algorithm>
#include <cassert>
#include <cmath>
#include <cstdint>
#include <iterator>
#include <memory>
#include <type_traits>
#include <utility>
#include <vector>

#include <termox/painter/glyph.hpp>
#include <termox/painter/painter.hpp>
#include <termox/widget/point.hpp>
#include <termox/widget/widget.hpp>

namespace ox {

/// Four point Boundary, edges are inclusive.
template <typename Number_t>
struct Boundary {
    Number_t west  = 0;
    Number_t east  = 1;
    Number_t north = 1;
    Number_t south = 0;
};

/// Bounded box that can display added Coordinates within the Boundary.
/** X axis is horizontal and increasing from left to right.
 *  Y axis is vertical and increasing from bottom to top. */
template <typename Number_t = double>
class Graph : public Widget {
   public:
    /// x is horizontal, y is vertical.
    struct Coordinate {
        Number_t x;
        Number_t y;
    };

   public:
    struct Parameters {
        Boundary<Number_t> boundary         = {};
        std::vector<Coordinate> coordinates = {};
    };

   public:
    /// Create a Graph with given Boundary and Coordinates.
    explicit Graph(Boundary<Number_t> b = {}, std::vector<Coordinate> x = {})
        : boundary_{b}, coordinates_{std::move(x)}
    {
        assert(b.west < b.east && b.south < b.north);
        auto constexpr empty_braille = U'⠀';
        this->set_wallpaper(empty_braille);
    }

    /// Create a Graph with given Parameters.
    explicit Graph(Parameters p) : Graph{p.boundary, std::move(p.coordinates)}
    {}

   public:
    /// Set a new Boundary and repaint the Widget.
    void set_boundary(Boundary<Number_t> b)
    {
        assert(b.west < b.east && b.south < b.north);
        boundary_ = b;
        this->update();
    }

    /// Return the currently set Boundary.
    [[nodiscard]] auto boundary() const -> Boundary<Number_t>
    {
        return boundary_;
    }

    /// Replace the current state with the given Coordinates vector \p x.
    void reset(std::vector<Coordinate> x)
    {
        coordinates_ = std::move(x);
        this->update();
    }

    /// Replace the current state with copies of Coordinates given by iterators.
    template <typename Iter1_t, typename Iter2_t>
    void reset(Iter1_t first, Iter2_t last)
    {
        static_assert(
            std::is_same_v<typename std::iterator_traits<Iter1_t>::value_type,
                           Coordinate>,
            "Must add with a iterators pointing to Coordinates.");
        coordinates_.clear();
        std::copy(first, last, std::back_inserter(coordinates_));
        this->update();
    }

    /// Add a single Coordinate to the Graph.
    void add(Coordinate c)
    {
        coordinates_.push_back(c);
        this->update();
    }

    /// Remove all points from the Graph and repaint.
    void clear()
    {
        coordinates_.clear();
        this->update();
    }

    /// Return a reference to the current container of Coordinates.
    [[nodiscard]] auto coordinates() const -> std::vector<Coordinate> const&
    {
        return coordinates_;
    }

   protected:
    auto paint_event(Painter& p) -> bool override
    {
        auto const area = this->area();
        auto const h_ratio =
            (double)area.width / distance(boundary_.west, boundary_.east);
        auto const v_ratio =
            (double)area.height / distance(boundary_.south, boundary_.north);

        for (auto const c : coordinates_) {
            auto const h_offset = h_ratio * distance(boundary_.west, c.x);
            auto const v_offset =
                area.height - v_ratio * distance(boundary_.south, c.y);
            if (h_offset >= area.width)  // h_offset can't be negative.
                continue;
            if (v_offset >= area.height || v_offset < 0)
                continue;
            auto const point = Point{(int)h_offset, (int)v_offset};
            auto const mask  = this->to_cell_mask(h_offset, v_offset);
            auto existing    = p.at(point);
            existing.symbol  = combine(existing.symbol, mask);
            p.put(existing, point);
        }
        return Widget::paint_event(p);
    }

   private:
    Boundary<Number_t> boundary_;
    std::vector<Coordinate> coordinates_;

   private:
    /// Finds the distance between two values. It's just subtraction.
    [[nodiscard]] static auto distance(Number_t smaller, Number_t larger)
        -> Number_t
    {
        return larger - smaller;
    }

    /// Returnt the cell mask cooresponding to the fractional parts of inputs.
    [[nodiscard]] static auto to_cell_mask(double h_offset, double v_offset)
        -> std::uint8_t
    {
        auto const h_cell = std::fmod(h_offset, 1.);
        auto const v_cell = std::fmod(v_offset, 1.);
        if (h_cell < 0.5) {
            if (v_cell < 0.25)
                return 0b00000001;
            else if (v_cell < 0.5)
                return 0b00000010;
            else if (v_cell < 0.75)
                return 0b00000100;
            else
                return 0b01000000;
        }
        else {
            if (v_cell < 0.25)
                return 0b00001000;
            else if (v_cell < 0.5)
                return 0b00010000;
            else if (v_cell < 0.75)
                return 0b00100000;
            else
                return 0b10000000;
        }
    }

    /// Additive unicode braille combining.
    [[nodiscard]] static auto combine(char32_t braille, std::uint8_t mask)
        -> char32_t
    {
        return braille | mask;
    }
};

/// Helper function to create a Graph instance.
template <typename Number_t = double>
[[nodiscard]] auto graph(
    Boundary<Number_t> b                                = {},
    std::vector<typename Graph<Number_t>::Coordinate> x = {})
    -> std::unique_ptr<Graph<Number_t>>
{
    return std::make_unique<Graph<Number_t>>(b, std::move(x));
}

/// Helper function to create a Graph instance.
template <typename Number_t = double>
[[nodiscard]] auto graph(typename Graph<Number_t>::Parameters p)
    -> std::unique_ptr<Graph<Number_t>>
{
    return std::make_unique<Graph<Number_t>>(std::move(p));
}

/// Return the input Boundary \p b, with each corner scaled by \p amount.
/** Only accepts finite values greater than zero for \p amount. */
template <typename Number_t>
[[nodiscard]] auto scale(Boundary<Number_t> b, double amount)
    -> Boundary<Number_t>
{
    assert(amount > 0. && std::isfinite(amount));
    return {Number_t(b.west * amount), Number_t(b.east * amount),
            Number_t(b.north * amount), Number_t(b.south * amount)};
}

/// Scrolls \p b horizontally left by \p amount.
/** \p amount has to be positive. */
template <typename Number_t>
[[nodiscard]] auto scroll_west(Boundary<Number_t> b, Number_t amount)
    -> Boundary<Number_t>
{
    assert(amount >= 0.);
    b.west -= amount;
    b.east -= amount;
    return b;
}

/// Scrolls \p b horizontally right by \p amount.
/** \p amount has to be positive. */
template <typename Number_t>
[[nodiscard]] auto scroll_east(Boundary<Number_t> b, Number_t amount)
    -> Boundary<Number_t>
{
    assert(amount >= 0.);
    b.west += amount;
    b.east += amount;
    return b;
}

/// Scrolls \p b vertically upwards by \p amount.
/** \p amount has to be positive. */
template <typename Number_t>
[[nodiscard]] auto scroll_north(Boundary<Number_t> b, Number_t amount)
    -> Boundary<Number_t>
{
    assert(amount >= 0.);
    b.north += amount;
    b.south += amount;
    return b;
}

/// Scrolls \p b vertically downwards by \p amount.
/** \p amount has to be positive. */
template <typename Number_t>
[[nodiscard]] auto scroll_south(Boundary<Number_t> b, Number_t amount)
    -> Boundary<Number_t>
{
    assert(amount >= 0.);
    b.north -= amount;
    b.south -= amount;
    return b;
}

/// Subtract cooresponding Boundary members.
template <typename Number_t>
[[nodiscard]] auto operator-(Boundary<Number_t> a, Boundary<Number_t> b)
    -> Boundary<Number_t>
{
    return {a.west - b.west, a.east - b.east, a.north - b.north,
            a.south - b.south};
}

/// Add cooresponding Boundary members.
template <typename Number_t>
[[nodiscard]] auto operator+(Boundary<Number_t> a, Boundary<Number_t> b)
    -> Boundary<Number_t>
{
    return {a.west + b.west, a.east + b.east, a.north + b.north,
            a.south + b.south};
}

}  // namespace ox
#endif  // TERMOX_WIDGET_WIDGETS_GRAPH_HPP

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

#include <termox/painter/color.hpp>
#include <termox/painter/glyph.hpp>
#include <termox/painter/painter.hpp>
#include <termox/widget/boundary.hpp>
#include <termox/widget/point.hpp>
#include <termox/widget/widget.hpp>

namespace ox {

/// Bounded box that can display added Coordinates within the Boundary.
/** X axis is horizontal and increasing from left to right.
 *  Y axis is vertical and increasing from bottom to top.
 *  Uses Braille characters, allowing up to 8 points per terminal cell. */
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
            if (h_offset >= area.width)  // h_offset already can't be negative.
                continue;
            if (v_offset >= area.height || v_offset < 0)
                continue;
            auto const point = Point{(int)h_offset, (int)v_offset};
            auto const mask  = this->to_cell_mask(h_offset, v_offset);
            auto current     = p.at(point);
            current.symbol   = combine(current.symbol, mask);
            p.put(current, point);
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
        auto const h_cell = h_offset - std::floor(h_offset);
        auto const v_cell = v_offset - std::floor(v_offset);
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

/// Bounded box that can display added Color Coordinates within the Boundary.
/** X axis is horizontal and increasing from left to right.
 *  Y axis is vertical and increasing from bottom to top.
 *  Uses half-block characters, allowing two points per terminal cell. */
template <typename Number_t = double>
class Color_graph : public Widget {
   public:
    /// x is horizontal, y is vertical.
    struct Coordinate {
        Number_t x;
        Number_t y;
    };

   public:
    struct Parameters {
        Boundary<Number_t> boundary                           = {};
        std::vector<std::pair<Coordinate, Color>> coordinates = {};
    };

   public:
    /// Create a Color_graph with given Boundary and Coordinates, Colors.
    explicit Color_graph(Boundary<Number_t> b                        = {},
                         std::vector<std::pair<Coordinate, Color>> x = {})
        : boundary_{b}, coordinates_{std::move(x)}
    {
        assert(b.west < b.east && b.south < b.north);
    }

    /// Create a Color_graph with given Parameters.
    explicit Color_graph(Parameters p)
        : Color_graph{p.boundary, std::move(p.coordinates)}
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
    void reset(std::vector<std::pair<Coordinate, Color>> x)
    {
        coordinates_ = std::move(x);
        this->update();
    }

    /// Replace with copies of <Coordinate, Color>s given by iterators.
    /** Iter1_t must have a value type of std::pair<Coordinate, Color>. */
    template <typename Iter1_t, typename Iter2_t>
    void reset(Iter1_t first, Iter2_t last)
    {
        static_assert(
            std::is_same_v<typename std::iterator_traits<Iter1_t>::value_type,
                           std::pair<Coordinate, Color>>,
            "Must add with a iterators pointing to Coordinates, Color pair.");
        coordinates_.clear();
        std::copy(first, last, std::back_inserter(coordinates_));
        this->update();
    }

    /// Add a single <Coordinate, Color> to the Graph.
    void add(std::pair<Coordinate, Color> p)
    {
        coordinates_.push_back(p);
        this->update();
    }

    /// Remove all points from the Graph and repaint.
    void clear()
    {
        coordinates_.clear();
        this->update();
    }

    /// Return a reference to the current container of Coordinates.
    [[nodiscard]] auto coordinates() const
        -> std::vector<std::pair<Coordinate, Color>> const&
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

        for (auto const& [coord, color] : coordinates_) {
            auto const h_offset = h_ratio * distance(boundary_.west, coord.x);
            auto const v_offset =
                area.height - v_ratio * distance(boundary_.south, coord.y);
            if (h_offset >= area.width)  // h_offset already can't be negative.
                continue;
            if (v_offset >= area.height || v_offset < 0)
                continue;
            auto const point  = Point{(int)h_offset, (int)v_offset};
            auto const is_top = is_top_region(v_offset);
            auto const glyph  = combine(p.at(point), is_top, color);
            p.put(glyph, point);
        }
        return Widget::paint_event(p);
    }

   private:
    Boundary<Number_t> boundary_;
    std::vector<std::pair<Coordinate, Color>> coordinates_;

   private:
    /// Finds the distance between two values. It's just subtraction.
    [[nodiscard]] static auto distance(Number_t smaller, Number_t larger)
        -> Number_t
    {
        return larger - smaller;
    }

    /// Return true if the given offset cooresponds to the top region.
    /** False if is bottom half. */
    [[nodiscard]] static auto is_top_region(double v_offset) -> bool
    {
        return (v_offset - std::floor(v_offset)) < 0.5;
    }

    /// Returns a new Glyph dependent on \p is_top and \p c.
    [[nodiscard]] static auto combine(Glyph current, bool is_top, Color c)
        -> Glyph
    {
        if (current.symbol == U' ')
            current = U'▀' | fg(Color::Background);
        if (is_top)
            current |= fg(c);
        else
            current |= bg(c);
        return current;
    }
};

/// Helper function to create a Color_graph instance.
template <typename Number_t = double>
[[nodiscard]] auto color_graph(
    Boundary<Number_t> b = {},
    std::vector<std::pair<typename Color_graph<Number_t>::Coordinate, Color>>
        x = {}) -> std::unique_ptr<Color_graph<Number_t>>
{
    return std::make_unique<Color_graph<Number_t>>(b, std::move(x));
}

/// Helper function to create a Color_graph instance.
template <typename Number_t = double>
[[nodiscard]] auto color_graph(typename Color_graph<Number_t>::Parameters p)
    -> std::unique_ptr<Color_graph<Number_t>>
{
    return std::make_unique<Color_graph<Number_t>>(std::move(p));
}

/// Bounded box that can display added Color Coordinates within a static Bound.
/** X axis is horizontal and increasing from left to right.
 *  Y axis is vertical and increasing from bottom to top.
 *  Uses half-block characters, allowing two points per terminal cell. */
template <typename Number_t,
          Number_t west,
          Number_t east,
          Number_t north,
          Number_t south>
class Color_graph_static_bounds : public Widget {
    static_assert(west < east && south < north);

   public:
    /// x is horizontal, y is vertical.
    struct Coordinate {
        Number_t x;
        Number_t y;
    };

   public:
    struct Parameters {
        std::vector<std::pair<Coordinate, Color>> coordinates = {};
    };

   public:
    /// Create a Color_graph_static_bounds with given Coordinates and Colors.
    explicit Color_graph_static_bounds(
        std::vector<std::pair<Coordinate, Color>> x = {})
        : coordinates_{std::move(x)}
    {}

    /// Create a Color_graph_static_bounds with given Parameters.
    explicit Color_graph_static_bounds(Parameters p)
        : Color_graph_static_bounds{std::move(p.coordinates)}
    {}

   public:
    /// Return the currently set Boundary.
    [[nodiscard]] static constexpr auto boundary() -> Boundary<Number_t>
    {
        return boundary_;
    }

    /// Replace the current state with the given Coordinates vector \p x.
    void reset(std::vector<std::pair<Coordinate, Color>> x)
    {
        coordinates_ = std::move(x);
        this->update();
    }

    /// Replace with copies of <Coordinate, Color>s given by iterators.
    /** Iter1_t must have a value type of std::pair<Coordinate, Color>. */
    template <typename Iter1_t, typename Iter2_t>
    void reset(Iter1_t first, Iter2_t last)
    {
        static_assert(
            std::is_same_v<typename std::iterator_traits<Iter1_t>::value_type,
                           std::pair<Coordinate, Color>>,
            "Must add with a iterators pointing to Coordinates, Color pair.");
        coordinates_.clear();
        std::copy(first, last, std::back_inserter(coordinates_));
        this->update();
    }

    /// Add a single <Coordinate, Color> to the Graph.
    void add(std::pair<Coordinate, Color> p)
    {
        coordinates_.push_back(p);
        this->update();
    }

    /// Remove all points from the Graph and repaint.
    void clear()
    {
        coordinates_.clear();
        this->update();
    }

    /// Return a reference to the current container of Coordinates.
    [[nodiscard]] auto coordinates() const
        -> std::vector<std::pair<Coordinate, Color>> const&
    {
        return coordinates_;
    }

   protected:
    auto paint_event(Painter& p) -> bool override
    {
        constexpr double h_distance = distance(boundary_.west, boundary_.east);
        constexpr double v_distance =
            distance(boundary_.south, boundary_.north);

        auto const area    = this->area();
        auto const h_ratio = (double)area.width / h_distance;
        auto const v_ratio = (double)area.height / v_distance;

        for (auto const& [coord, color] : coordinates_) {
            auto const h_offset = h_ratio * distance(boundary_.west, coord.x);
            auto const v_offset =
                area.height - (v_ratio * distance(boundary_.south, coord.y));
            if (h_offset >= area.width)  // h_offset already can't be negative.
                continue;
            if (v_offset >= area.height || v_offset < 0)
                continue;
            auto const point  = Point{(int)h_offset, (int)v_offset};
            auto const is_top = is_top_region(v_offset);
            auto const glyph  = combine(p.at(point), is_top, color);
            p.put(glyph, point);
        }
        return Widget::paint_event(p);
    }

   private:
    static constexpr auto boundary_ =
        Boundary<Number_t>{west, east, north, south};

    std::vector<std::pair<Coordinate, Color>> coordinates_;

   private:
    /// Finds the distance between two values. It's just subtraction.
    [[nodiscard]] static constexpr auto distance(Number_t smaller,
                                                 Number_t larger) -> Number_t
    {
        return larger - smaller;
    }

    /// Return true if the given offset cooresponds to the top region.
    /** False if is bottom half. */
    [[nodiscard]] static auto is_top_region(double v_offset) -> bool
    {
        return (v_offset - std::floor(v_offset)) < 0.5;
    }

    /// Returns a new Glyph dependent on \p is_top and \p c.
    [[nodiscard]] static auto combine(Glyph current, bool is_top, Color c)
        -> Glyph
    {
        if (current.symbol == U' ')
            current = U'▀' | fg(Color::Background);
        if (is_top)
            current |= fg(c);
        else
            current |= bg(c);
        return current;
    }
};

/// Helper function to create a Color_graph_static_bounds instance.
template <typename Number_t,
          Number_t west,
          Number_t east,
          Number_t north,
          Number_t south>
[[nodiscard]] auto color_graph_static_bounds(
    std::vector<std::pair<
        typename Color_graph_static_bounds<Number_t, west, east, north, south>::
            Coordinate,
        Color>> x = {})
    -> std::unique_ptr<
        Color_graph_static_bounds<Number_t, west, east, north, south>>
{
    return std::make_unique<
        Color_graph_static_bounds<Number_t, west, east, north, south>>(
        std::move(x));
}

/// Helper function to create a Color_graph_static_bounds instance.
template <typename Number_t,
          Number_t west,
          Number_t east,
          Number_t north,
          Number_t south>
[[nodiscard]] auto color_graph_static_bounds(
    typename Color_graph_static_bounds<Number_t, west, east, north, south>::
        Parameters p)
    -> std::unique_ptr<
        Color_graph_static_bounds<Number_t, west, east, north, south>>
{
    return std::make_unique<
        Color_graph_static_bounds<Number_t, west, east, north, south>>(
        std::move(p));
}

}  // namespace ox
#endif  // TERMOX_WIDGET_WIDGETS_GRAPH_HPP

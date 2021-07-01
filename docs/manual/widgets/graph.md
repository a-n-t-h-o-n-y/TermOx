# Graph Widget

[`<termox/widget/widgets/graph.hpp>`](../../../include/termox/widget/widgets/graph.hpp)

A bounded box that can display dots at given coordinates. The X axis
is horizontal and increasing from left to right, the Y axis is vertical and
increasing from bottom to top. The bounds can change at runtime. Uses unicode
braille characters(`⣿`), allowing up to eight points per terminal cell.

## `Graph`

```cpp
template <typename Number_t = double>
class Graph : public Widget {
   public:
    // x is horizontal, y is vertical.
    struct Coordinate {
        Number_t x;
        Number_t y;
    };

    struct Parameters {
        Boundary<Number_t> boundary         = {};
        std::vector<Coordinate> coordinates = {};
    };

   public:
    // Create a Graph with given Boundary and Coordinates.
    Graph(Boundary<Number_t> b = {}, std::vector<Coordinate> x = {});

    // Create a Graph with given Parameters.
    Graph(Parameters);

    // Set a new Boundary and repaint the Widget.
    void set_boundary(Boundary<Number_t> b);

    // Return the currently set Boundary.
    auto boundary() const -> Boundary<Number_t>;

    // Replace the current state with the given Coordinates vector \p x.
    void reset(std::vector<Coordinate> x);

    // Replace the current state with copies of Coordinates given by iterators.
    template <typename Iter1_t, typename Iter2_t>
    void reset(Iter1_t first, Iter2_t last);

    // Add a single Coordinate to the Graph.
    void add(Coordinate c);

    // Remove all points from the Graph and repaint.
    void clear();

    // Return a reference to the current container of Coordinates.
    auto coordinates() const -> std::vector<Coordinate> const&;
};
```

## `Color_graph`

A bounded box that can display colored squares at given coordinates. The X axis
is horizontal and increasing from left to right, the Y axis is vertical and
increasing from bottom to top. The bounds can change at runtime. Uses half-block
characters(`▀`), allowing two points per terminal cell.

```cpp
template <typename Number_t = double>
class Color_graph : public Widget {
   public:
    // x is horizontal, y is vertical.
    struct Coordinate {
        Number_t x;
        Number_t y;
    };

    struct Parameters {
        Boundary<Number_t> boundary                           = {};
        std::vector<std::pair<Coordinate, Color>> coordinates = {};
    };

   public:
    // Create a Color_graph with given Boundary and Coordinates, Colors.
    Color_graph(Boundary<Number_t> b                        = {},
                std::vector<std::pair<Coordinate, Color>> x = {});

    // Create a Color_graph with given Parameters.
    Color_graph(Parameters);

    // Set a new Boundary and repaint the Widget.
    void set_boundary(Boundary<Number_t> b);

    // Return the currently set Boundary.
    auto boundary() const -> Boundary<Number_t>;

    // Replace the current state with the given Coordinates vector \p x.
    void reset(std::vector<std::pair<Coordinate, Color>> x);

    // Replace with copies of <Coordinate, Color>s given by iterators.
    /* Iter1_t must have a value type of std::pair<Coordinate, Color>. */
    template <typename Iter1_t, typename Iter2_t>
    void reset(Iter1_t first, Iter2_t last);

    // Add a single <Coordinate, Color> to the Graph.
    void add(std::pair<Coordinate, Color> p);

    // Remove all points from the Graph and repaint.
    void clear();

    // Return a reference to the current container of Coordinates.
    auto coordinates() const -> std::vector<std::pair<Coordinate, Color>> const&
};
```

## `Color_graph_static_bounds`

A bounded box that can display colored squares at given coordinates. The X axis
is horizontal and increasing from left to right, the Y axis is vertical and
increasing from bottom to top. The boundary is set at compile time and has
performance benefits over `Color_graph` if your use case does not change the
boundary at runtime. Uses half-block characters(`▀`), allowing two points per
terminal cell.

```cpp
template <typename Number_t, Number_t west, Number_t east, Number_t north, Number_t south>
class Color_graph_static_bounds : public Widget {
   public:
    // x is horizontal, y is vertical.
    struct Coordinate {
        Number_t x;
        Number_t y;
    };

    struct Parameters { std::vector<std::pair<Coordinate, Color>> coordinates = {}; };

   public:
    // Create a Color_graph_static_bounds with given Coordinates and Colors.
    Color_graph_static_bounds(std::vector<std::pair<Coordinate, Color>> x = {});

    // Create a Color_graph_static_bounds with given Parameters.
    Color_graph_static_bounds(Parameters);

    // Return the currently set Boundary.
    static constexpr auto boundary() -> Boundary<Number_t>;

    // Replace the current state with the given Coordinates vector \p x.
    void reset(std::vector<std::pair<Coordinate, Color>> x);

    // Replace with copies of <Coordinate, Color>s given by iterators.
    /* Iter1_t must have a value type of std::pair<Coordinate, Color>. */
    template <typename Iter1_t, typename Iter2_t>
    void reset(Iter1_t first, Iter2_t last);

    // Add a single <Coordinate, Color> to the Graph.
    void add(std::pair<Coordinate, Color> p);

    // Remove all points from the Graph and repaint.
    void clear();

    // Return a reference to the current container of Coordinates.
    auto coordinates() const -> std::vector<std::pair<Coordinate, Color>> const&;
};
```

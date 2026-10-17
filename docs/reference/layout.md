# Layouts and decorations

[API reference](../reference.md) · [Guides](../README.md)

Size policies, container storage, borders, and dividers.

## On this page

- [ox::SizePolicy](#oxsizepolicy)
- [Layout Concepts](#layout-concepts)
- [ox::Row](#oxrow)
- [ox::Column](#oxcolumn)
- [ox::Suspended](#oxsuspended)
- [ox::Padding, ox::Stack, ox::ScrollView](#oxpadding-oxstack-oxscrollview)
- [Layout calculation](#layout-calculation)
- [ox::Border](#oxborder)
- [ox::Bordered](#oxbordered)
- [ox::Lines](#oxlines)
- [ox::Divider](#oxdivider)

## ox::SizePolicy

[`#include <ox/core/layout.hpp>`](../../include/ox/core/layout.hpp)

Provides a description of how a Widget can be sized within its parent layout.

```cpp
Column{
    Label{"A TextBox"} | SizePolicy::fixed(1),
    TextBox{} | SizePolicy::flex(),
    Label{"Footer"} | SizePolicy::fixed(5),
};
```

A Row uses each child's policy for width; a Column uses it for height. Suspended
takes a requested Area directly. See [layout calculation](../layout.md) for proportional
shrinking, weighted growth, largest-remainder rounding, and input validation.

Pipe (`|`) operator overloading provided for Widgets to set their SizePolicy.

```cpp
struct SizePolicy {
    int minimum = 0;
    int maximum = std::numeric_limits<int>::max();
    float flexibility = 1.f;

    static constexpr auto fixed(int size) -> SizePolicy;
    static constexpr auto flex(float flex = 1.f) -> SizePolicy;
    static constexpr auto bounded(int min, int max) -> SizePolicy;
    static constexpr auto min(int min) -> SizePolicy;
    static constexpr auto max(int max) -> SizePolicy;
};
```

## Layout Concepts

[`#include <ox/layout.hpp>`](../../include/ox/layout.hpp)

A few concepts are provided to help identify the types of containers that can be used
within Row and Column layouts.

```cpp
/**
 * tuple, pair, array, etc...
 */
template <typename T>
concept TupleLike = requires {
    { std::tuple_size<T>::value } -> std::convertible_to<std::size_t>;
};

/**
 * vector, set, list, etc...
 */
template <typename T>
concept DynamicContainer = not TupleLike<std::remove_reference_t<T>> &&
                           std::ranges::sized_range<T> && std::ranges::forward_range<T>;

template <typename T>
concept LayoutContainer = DynamicContainer<T> || TupleLike<T>;

template <typename T>
concept WidgetDerived = std::is_base_of_v<Widget, T>;
```

## ox::Row

Call `refresh()` after changing children, visibility, or policies. Const get_child
and ordinary tuple/array/vector/list composition are supported. See
[decorations and layouts](../decorations-layouts.md#row-and-column-refresh).

[`#include <ox/layout.hpp>`](../../include/ox/layout.hpp)

```cpp
template <LayoutContainer Container>
class Row : public Widget;
```
A layout that arranges its children in a horizontal row. The children are arranged from
left to right, with the first child of the container on the left and the last child on
the right. Each child is given the same height as the Row, and its width is determined
by the child's SizePolicy.

The Row layout is a `Widget` itself, so it can be used within other layouts as a child.

```cpp
auto row = Row{
    Button{{.label = {.text = "A Button"}}} | SizePolicy::fixed(5),
    TextBox{} | SizePolicy::flex(),
    Button{{.label = {.text = "B Button"}}} | SizePolicy::fixed(5),
};

auto row2 = Row<std::vector<Button>>{};
row2.children.push_back(Button{{.label = {.text = "A Button"}}} | SizePolicy::fixed(5));
row2.children.push_back(Button{{.label = {.text = "B Button"}}} | SizePolicy::flex());
row2.children.push_back(Button{{.label = {.text = "C Button"}}} | SizePolicy::fixed(5));
```

### Constructors

```cpp
explicit Row(Container container = {});

template <WidgetDerived... Widgets>
explicit Row(Widgets&&... ws);
```

The first constructor takes a container of Widgets, which can be a tuple, pair, array,
or any other container that meets the `LayoutContainer` concept.

The second constructor takes a variadic list of Widgets and moves them into a
`std::tuple` container. This is provided for convenience.

### Public members

```cpp
Container children;
```

Direct access to the children of the Row layout.

### `get_child` Free Function

```cpp
template <std::size_t I, TupleLike Container>
auto get_child(Row<Container>& row) -> auto&
```

Returns the `I`th child of the Row layout. This is provided for convenience and
uniformity with other Layouts. This is only available for `std::tuple` container
layouts.

## ox::Column

Call `refresh()` after changing children, visibility, or policies. See
[decorations and layouts](../decorations-layouts.md#row-and-column-refresh) for caching
and mutation/callback behavior.

[`#include <ox/layout.hpp>`](../../include/ox/layout.hpp)

```cpp
template <LayoutContainer Container>
class Column : public Widget;
```

A layout that arranges its children in a vertical column. The children are arranged from
top to bottom, with the first child of the container at the top and the last child at
the bottom. Each child is given the same width as the Column, and its height is
determined by the child's SizePolicy.

The Column layout is a `Widget` itself, so it can be used within other layouts as a
child.

```cpp
auto col = Column{
    Label{"A Label"} | SizePolicy::fixed(1),
    TextBox{} | SizePolicy::flex(),
    Label{"Footer"} | SizePolicy::fixed(5),
};

auto col2 = Column<std::vector<Button>>{};
col2.children.push_back(Button{{.label = {.text = "A Button"}}} | SizePolicy::fixed(5));
col2.children.push_back(Button{{.label = {.text = "B Button"}}} | SizePolicy::flex());
col2.children.push_back(Button{{.label = {.text = "C Button"}}} | SizePolicy::fixed(5));
```

### Constructors

```cpp
explicit Column(Container container = {});

template <WidgetDerived... Widgets>
explicit Column(Widgets&&... ws);
```

The first constructor takes a container of Widgets, which can be a tuple, pair, array,
or any other container that meets the `LayoutContainer` concept.

The second constructor takes a variadic list of Widgets and moves them into a
`std::tuple` container. This is provided for convenience.

### Public members

```cpp
Container children;
```

Direct access to the children of the Column layout.

### `get_child` Free Function

```cpp
template <std::size_t I, TupleLike Container>
auto get_child(Column<Container>& row) -> auto&
```

Returns the `I`th child of the Column layout. This is provided for convenience and
uniformity with other Layouts. This is only available for `std::tuple` container
layouts.

## ox::Suspended

[`#include <ox/layout.hpp>`](../../include/ox/layout.hpp)

```cpp
template <WidgetDerived ChildType>
class Suspended : public Widget;
```

A layout holding a single Widget centered within the available area. Its requested
size is clamped to the available dimensions.

```cpp
auto sus = Suspended{
    TextBox{{
        .style = {.background = colors::bright_black},
        .focus_policy = FocusPolicy::Strong,
    }},
    {.width = 40, .height = 5},
};
```

### Constructors

```cpp
explicit Suspended(ChildType&& child, Area requested, Glyph fill = {U' '});
```

The constructor takes a child, a requested size, and a fill glyph for the surrounding
margins. Change `requested_size` and call `refresh()` to update its dimensions.
Refresh validates before changing child geometry and notifies the child even when
geometry is unchanged. Painting composes `fill_outside(canvas, child.bounds(), fill_glyph)`.

### Public members

```cpp
ChildType child;
Area requested_size;
Glyph fill_glyph;
```

### `get_child` Free Function

```cpp
template <WidgetDerived ChildType>
auto get_child(Suspended<ChildType>& sus) -> auto&
```

Returns the child Widget of the Suspended layout. This is provided for convenience and
uniformity with other Layouts.

## ox::Padding, ox::Stack, ox::ScrollView

`<ox/container.hpp>` supplies an inset child with background painting, a selected-page
container, and a clipped scrolling child. See [container composition](../containers.md).

## Layout calculation

[`#include <ox/core/layout.hpp>`](../../include/ox/core/layout.hpp)

`distribute_length(policies, available)` returns integer lengths from a span of
`SizePolicy` values. `layout_row(policies, area)` and `layout_column(policies, area)`
return consecutive rectangles. `centered_rect(available, requested)` returns a clamped,
centered rectangle. These functions have no widget or terminal dependencies.

See [layout calculation](../layout.md) for allocation rules and edge cases.

## ox::Border

[`#include <ox/bordered.hpp>`](../../include/ox/bordered.hpp)

`struct Border;`

Visual description of a Border surrounding a Widget. To be used with `Bordered` class.

```cpp
struct Border {
    shape::Box box = shape::Box::square();
    Style style = {};  // Only applies to box.
    Label::Options label = {};
};
```

The `Border::style` member only applies to the drawing of the Box. The Label has its own
internal Style it will use to paint its text. Alignment of the Label will properly align
along the top of the border. If no text is set on the Label, it will display only the
Box.

## ox::Bordered

[`#include <ox/bordered.hpp>`](../../include/ox/bordered.hpp)

```cpp
template <WidgetDerived ChildWidget>
class Bordered : public Widget;
```

Wraps a Widget in a Border. The child receives `border_content_rect(available)`, inset
one cell from each edge. Tiny areas can leave an empty child interior.

This is technically a layout, the wrapped Widget is accessed via the `Bordered::child` member.

A Widget can be easily wrapped in a Border by using the `operator|` overload:

```cpp
template <WidgetDerived WidgetType>
auto operator|(WidgetType w, Border b) -> Bordered<WidgetType>;
```

The `operator|` overload will create a new Bordered object that will take ownership of
the passed in Widget.

```cpp
    auto bordered = MyWidget{} | Border{.box = shape::Box::round(), .label = {.text = "A Label"}};
    auto& my_widg = bordered.child;
```

### Constructors

```cpp
struct Options {
    ChildWidget child = {};
    Border border = {};
};

Bordered(Options x);
```

Takes ownership of the child Widget.

### Public members

```cpp
ChildWidget child;
Border border;
```

Access to the wrapped child Widget and Border.

### `get_child` Free Function

```cpp
template <std::size_t I, WidgetDerived WidgetType>
auto get_child(Bordered<WidgetType>& bordered) -> auto&;
```

Returns the `I`th child of the wrapped Widget, provided for convenience and uniformity with other Layouts.

## ox::Lines

[`#include <ox/bordered.hpp>`](../../include/ox/bordered.hpp)

`struct Lines;`

Visual line data, used by Divider class.

```cpp
struct Lines {
    char32_t vertical;
    char32_t horizontal;

    static constexpr auto light() -> Lines { return {U'│', U'─'}; }
    static constexpr auto bold() -> Lines { return {U'┃', U'━'}; }
    static constexpr auto twin() -> Lines { return {U'║', U'═'}; }
    static constexpr auto dashed() -> Lines { return {U'╎', U'╌'}; }
    static constexpr auto dotted() -> Lines { return {U'┆', U'┄'}; }
    static constexpr auto ascii() -> Lines { return {U'|', U'-'}; }
    static constexpr auto empty() -> Lines { return {U' ', U' '}; }
};
```

## ox::Divider

[`#include <ox/bordered.hpp>`](../../include/ox/bordered.hpp)

`class Divider : public Widget;`

A line widget with explicit `vertical` direction (false by default) and an overlaid
plain label. Vertical labels stack graphemes along the line. See
[decorations and layouts](../decorations-layouts.md) for geometry and clipping.

```cpp
Column{
    Button{},
    Divider{{
        .lines = Lines::bold(),
        .style = {.foreground = colors::bright_black},
    }},
    Button{},
};
```

### Constructors

```cpp
struct Options {
    bool vertical = false;
    Lines lines = Lines::light();
    Style style = {};
    Label::Options label = {};
};

Divider(Options x);
```

The `Divider::style` member only applies to the Line, and not the Label, The Label has
its own internal Style.

### Public members

```cpp
bool vertical;
Lines lines;
Style style;
Label::Options label;
```

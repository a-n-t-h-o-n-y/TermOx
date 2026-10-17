# Geometry, text, and style types

[API reference](../reference.md) · [Guides](../README.md)

Coordinates, alignment, colors, styles, traits, and glyphs.

## On this page

- [ox::Point](#oxpoint)
- [ox::Area](#oxarea)
- [ox::Align](#oxalign)
- [ox::Trait and ox::Traits](#oxtrait-and-oxtraits)
- [ox::Color and ox::Style](#oxcolor-and-oxstyle)
- [ox::Glyph and ox::GlyphString](#oxglyph-and-oxglyphstring)

## ox::Point

[`#include <ox/core/terminal.hpp>`](../../include/ox/core/terminal.hpp)

`struct Point;`

An `x` (horizontal) and `y` (vertical) coordinate on a 2D grid.

```cpp
auto p = Point{.x = 5, .y = 8};
```

```cpp
struct Point {
    int x;
    int y;

    constexpr auto operator==(Point const& other) const -> bool = default;
    constexpr auto operator!=(Point const& other) const -> bool = default;
    constexpr auto operator+(Point const& other) const -> Point;
    constexpr auto operator-(Point const& other) const -> Point;
    constexpr auto operator<(Point const& other) const -> bool;
};
```

`operator+` and `operator-` operate on each corresponding element. ie, `x` is added to
`x` and `y` is added to `y`.

## ox::Area

[`#include <ox/core/terminal.hpp>`](../../include/ox/core/terminal.hpp)

`struct Area;`

Holds `width` and `height` for a 2D grid.

```cpp
auto a = Area{.width = 4, .height = 7};
```

```cpp
struct Area {
    int width;
    int height;

    constexpr auto operator==(Area const& other) const -> bool = default;
    constexpr auto operator!=(Area const& other) const -> bool = default;
};
```

## ox::Align

[`#include <ox/align.hpp>`](../../include/ox/align.hpp)

`Align` describes alignment along the text direction: `Start`, `Center`, or `End`.
For horizontal text these mean left/center/right; for vertical labels they mean
top/middle/bottom. `Left` and `Right` are aliases for Start and End.

```cpp
Label{{.text = "Hello", .align = Align::Center}};
auto offset = align_offset(Align::Center, 10, 4); // 3 terminal columns.
```

`align_offset` requires nonnegative available/used terminal-column counts. Oversized
content can produce a negative offset. Invalid enum values throw `std::invalid_argument`.
Use `centered_rect` for centered layout geometry.

## ox::Trait and ox::Traits

[`#include <ox/core/glyph.hpp>`](../../include/ox/core/glyph.hpp)

These are Escape's validated text appearance attributes. `Trait` includes `Bold`,
`Dim`, `Italic`, `Underline`, `Blink`, `Inverse`, `Invisible`, `CrossedOut`, and
`DoubleUnderline`. `Standout` combines bold and inverse; `None` is empty.
`Traits` combines attributes and provides `contains`, `insert`, and `remove`.

```cpp
auto traits = Trait::Bold | Trait::Inverse;
auto glyph = U'x' | traits;
glyph = glyph | esc::without(Trait::Bold);
```

## ox::Color and ox::Style

These types come directly from Escape:

```cpp
using Color = std::variant<esc::DefaultColor, IndexedColor, RGB>;

struct Style {
    Color background = default_color;
    Color foreground = default_color;
    Traits traits = {};
};
```

Use `colors::blue` and other named palette colors, `IndexedColor{235}` for a palette
slot, `RGB{0x773283}` for true color, or `RGB{HSL{120, 90, 70}}` for an HSL conversion.
`fg(color)` and `bg(color)` select a color channel. Widgets expose `style`,
`style`, `placeholder_style`, or `focused_style` as appropriate.

## ox::Glyph and ox::GlyphString

```cpp
struct Glyph {
    char32_t symbol = U' ';
    Style style = {};
};
using GlyphString = std::vector<Glyph>;
```

A glyph is a styled Unicode scalar. Frames group scalars into complete graphemes and reserve their terminal column
footprints, including continuation cells for wide graphemes.
Use Escape's public conversion and glyph functions for validated text:

```cpp
auto label = esc::glyphs("Hello") | Trait::Bold | fg(colors::cyan);
auto literal = U"Hello"_gs;
auto scalar = U'•' | fg(colors::blue);
put(canvas, {0, 0}, label);
```

`GlyphRange` is Escape's concept for ranges of glyphs. Styling a glyph range returns
a glyph string. Styling plain text with Escape's `operator|` creates a composed
output value; use `esc::glyphs(text)` first when painting to a TermOx canvas.

# Drawing and rendering

[API reference](../reference.md) · [Guides](../README.md)

Shapes, drawing functions, pixel images, frames, canvases, and rendering.

## On this page

- [ox::shape::HLine](#oxshapehline)
- [ox::shape::VLine](#oxshapevline)
- [ox::shape::Box](#oxshapebox)
- [ox::put(...)](#oxput)
- [ox::fill(...)](#oxfill)
- [ox::clear(...)](#oxclear)
- [ox::PixelGrid](#oxpixelgrid)
- [ox::Frame and ox::Canvas](#oxframe-and-oxcanvas)
- [ox::Renderer](#oxrenderer)

## ox::shape::HLine

[`#include <ox/put.hpp>`](../../include/ox/put.hpp)

`struct HLine;`

A horizontal line that you can paint to a Canvas with `put(...)`.

```cpp
auto length = 5;
put(c, {.x = 5, .y = 2}, shape::HLine{.symbol = U'-'}, length);
```

```cpp
struct HLine {
    char32_t symbol = U'─';
};
```

## ox::shape::VLine

[`#include <ox/put.hpp>`](../../include/ox/put.hpp)

`struct VLine;`

A vertical line that you can paint to a Canvas with `put(...)`.

```cpp
auto length = 5;
put(c, {.x = 5, .y = 2}, shape::VLine{.symbol = U'|'}, length);
```

```cpp
struct VLine {
    char32_t symbol = U'│';
};
```

## ox::shape::Box

[`#include <ox/put.hpp>`](../../include/ox/put.hpp)

`struct Box;`

A 2D rectangle outline that you can paint to a Canvas with `put(...)`.

```cpp
auto size = Area{.width = 10, .height = 5};
put(c, {.x = 5, .y = 2}, shape::Box::square(), size);
```

```cpp
struct Box {
    /// {top left, top right, bottom left, bottom right}
    std::array<char32_t, 4> corners;

    /// {N, S, E, W}
    std::array<char32_t, 4> walls;

    static constexpr Box square();
    static constexpr Box half_square();
    static constexpr Box round();
    static constexpr Box half_round();
    static constexpr Box bold();
    static constexpr Box dashed();
    static constexpr Box dotted();
    static constexpr Box ascii();
    static constexpr Box half_twin();
    static constexpr Box twin();
    static constexpr Box half_bold();
    static constexpr Box corners_square();
    static constexpr Box corners_round();
    static constexpr Box brackets();
    static constexpr Box brackets_round();
    static constexpr Box brackets_bold();
    static constexpr Box brackets_twin();
    static constexpr Box twin_horizontal();
    static constexpr Box twin_vertical();
};
```

## ox::put(...)

[`#include <ox/put.hpp>`](../../include/ox/put.hpp)

A function overload set to put Glyphs on a Canvas object.

```cpp
auto paint(Canvas c) -> Cursor override {
    put(c, {.x = 5, .y = 10}, 'X');
    put(c, {.x = 6, .y = 10}, 'X' | fg(colors::blue));
    put(c, {.x = 7, .y = 10}, "Hello!");

    put(c, {.x = 0, .y = 0}, shape::HLine{}, this->size.width);
    put(c, {.x = 0, .y = 0}, shape::VLine{}, 3, Style{.background = RGB{0x337283}});
    put(c, {.x = 0, .y = 1}, shape::Box::dashed(), {.width = 10, .height = 4});

    put(c, shape::Box::bold());  // Outlines the Canvas.

    return {};
}
```

A Canvas is a light-weight 2D grid of Glyph objects. It provides a direct mapping to a
sub-region of the `Frame`.

`put(...)` functions do bounds checking, and anything painted out of bounds of the
Canvas will be ignored. These functions are provided as convenience, as an alternative
with no bounds checking, individual Glyphs can be painted to the screen with
`Canvas::operator[]`. A Widget's Canvas starts at the top left with `Point{.x = 0, .y =
0}` and ends at the bottom right with `Point{.x = this->size.width - 1, .y =
this->size.height - 1}`.

Overloads without a `Color` or `Style` will modify the `Glyph::symbol` members in the
Canvas, keeping the existing Style. Overloads with `Color` will overwrite both the
symbol and the `Glyph::style::foreground` members, and overloads with `Style` will
overwrite the symbol and the `Glyph::style` members.

```cpp
void put(Canvas c, Point at, Glyph const& item);
void put(Canvas c, Point at, Character auto item);
void put(Canvas c, Point at, GlyphRange auto const& item);
void put(Canvas c, Point at, std::string_view element);
void put(Canvas c, Point at, std::u32string_view element);

void put(Canvas c, Point at, shape::HLine item, int length);
void put(Canvas c, Point at, shape::HLine item, int length, Color foreground);
void put(Canvas c, Point at, shape::HLine item, int length, Style const& style);

void put(Canvas c, Point at, shape::VLine item, int length);
void put(Canvas c, Point at, shape::VLine item, int length, Color foreground);
void put(Canvas c, Point at, shape::VLine item, int length, Style const& style);

void put(Canvas c, Point at, shape::Box const& item, Area size);
void put(Canvas c, Point at, shape::Box const& item, Area size, Color foreground);
void put(Canvas c, Point at, shape::Box const& item, Area size, Style const& style);

void put(Canvas c, shape::Box const& item);
void put(Canvas c, shape::Box const& item, Color foreground);
void put(Canvas c, shape::Box const& item, Style const& style);
```

## ox::fill(...)

[`#include <ox/put.hpp>`](../../include/ox/put.hpp)

A function overload set to fill a 2D space on a Canvas. It fills the entire Canvas
object given to it.

```cpp
auto paint(Canvas c) -> Cursor override {
    fill(c, 'X');
    fill(c, fg(RGB{0x33A421}));
    fill(c, Trait::Bold | Trait::Standout);

    auto sub_region = Canvas{
        .buffer = c.buffer,
        .at = c.at + {.x = 3, .y = 0}, 
        .size = {.width = 5, .height = 7},
    };
    fill(sub_region, 'X' | bg(colors::red) | fg(colors::black));

    return {};
}
```

The overloads are similar to `put(...)`, where the Glyph overload will overwrite
everything, the `char32_t` overload will only overwrite the `Glyph::symbol` member,
etc... To fill a sub-section of the Canvas, create a new Canvas object with the desired
dimensions and position. 

```cpp
void fill(Canvas c, Glyph g);
void fill(Canvas c, Style b);
void fill(Canvas c, char32_t ch);
void fill(Canvas c, ColorBG bg);
void fill(Canvas c, ColorFG fg);
void fill(Canvas c, Traits ts);
```

## ox::clear(...)

[`#include <ox/put.hpp>`](../../include/ox/put.hpp)

Resets all Glyphs in a Canvas to the default empty Glyph.

```cpp
auto paint(Canvas c) -> Cursor override {
    fill(c, fg(RGB{0x33A421}));
    clear(c);

    return {};
}
```

This function should not be needed in most circumstances, the Canvas is provided cleared
on entry to the `Widget::paint(Canvas)` event handler, unless a subclass or parent
Widget has written over it.

```cpp
void clear(Canvas c);
```

## ox::PixelGrid

[`#include <ox/pixelgrid.hpp>`](../../include/ox/pixelgrid.hpp)

`class PixelGrid : public Widget;`

An owned row-major RGB image. Each terminal cell holds two pixels using `▄`:
upper pixel as background, lower pixel as foreground. Odd final rows use the
public `odd_pixel` color (black by default). Native cell size is
`pixel_cell_size(pixel_size())`; positioning and clipping use ordinary Canvas views.

```cpp
auto grid = ox::PixelGrid{ox::Area{10, 11}, ox::RGB{0}};
grid.set_pixel({0, 0}, ox::RGB{0xff0000}); // Requests repaint when changed.
grid.reset({20, 12}, ox::RGB{0x123456});
grid.set_image({2, 1}, {ox::RGB{0xff0000}, ox::RGB{0x0000ff}});
```

`pixel_size()` and `pixels()` expose dimensions and mutable/const spans.
`at(Point)` provides checked access; no-argument `at()` retains Widget geometry.
Direct span/reference edits and changes to `odd_pixel` require an application
repaint request. Image replacement invalidates spans and references. Moves transfer
Widget identity and image storage and leave an empty, reusable source.

Dimensions must be nonnegative; storage must contain exactly `width * height`
RGB values. Counts use checked arithmetic and reject sizes beyond the addressable
byte range. Invalid dimensions/storage throw `std::invalid_argument`, impossible
counts throw `std::length_error`, and invalid coordinates throw `std::out_of_range`.
`reset` and `set_image` preserve the current image if validation/allocation fails.

Custom widgets can compose the same drawing without owning a PixelGrid:

```cpp
void draw(ox::Canvas canvas, ox::Area size, std::span<ox::RGB const> pixels)
{
    ox::paint_pixels(canvas, size, pixels);
}
```

The painter validates storage before writing, visits only visible cells, and
leaves margins unchanged. Visible output requires a one-column lower-half block
under the Canvas width policy. `pixel_count` and `pixel_index` provide checked
row-major arithmetic for callers using their own vectors.

See the [`bmp_view.cpp`](../../examples/bmp_view.cpp) example. Its decoder is kept
in the example and supports uncompressed 1/4/8/16/24/32-bit BMPs and 16/32-bit
bitfields, including top-down images, with bounds-checked headers, rows and palettes.

## ox::Frame and ox::Canvas

[`#include <ox/core/frame.hpp>`](../../include/ox/core/frame.hpp)
[`#include <ox/core/canvas.hpp>`](../../include/ox/core/canvas.hpp)

`Frame` owns cells containing complete graphemes, styles, and column widths.
Continuation cells mark the rest of a wide glyph's footprint. Cell access is read-only;
use `write()` or the drawing functions to preserve footprint invariants.

`Canvas` borrows a frame and exposes local coordinates, inherited clipping, and
`subcanvas(offset, size)`. All drawing operations clip without changing the requested
geometry. Resizing resets the frame; existing canvas objects safely intersect their
retained geometry with the new bounds.

```cpp
auto frame = Frame{{20, 5}};
auto canvas = Canvas{frame};
auto panel = canvas.subcanvas({2, 1}, {10, 3});
put(panel, {0, 0}, "Hello 界");
```

See [the frame and canvas guide](../internals/frame-and-canvas.md) for coordinate checks, resize,
Unicode width policies, complete-footprint clipping, and borrowing rules.

## ox::Renderer

[`#include <ox/core/renderer.hpp>`](../../include/ox/core/renderer.hpp)

`Renderer(profile)` owns an Escape capability profile and reusable presentation storage.
`render(frame, cursor, foreground, background)` returns borrowed encoded bytes without
terminal I/O. Adjacent changes form runs; unchanged frames produce no bytes. Cursor
position and effective style are retained across calls. The caller writes the bytes
and manages visibility and flushing. Call `invalidate()` if the bytes were discarded,
output failed, or another writer changed terminal state.

See [the renderer guide](../internals/renderer.md) for buffer lifetime, Unicode boundaries,
capability adaptation, resize behavior, and failure recovery.

# Draw custom content

Override a widget's `paint(Canvas)` to draw in its local terminal cells. A canvas
already carries the inherited clip and glyph-width policy; the application owns the
frame and presents it to the terminal.

```cpp
#include <ox/ox.hpp>

class Notice : public ox::Widget {
public:
    auto paint(ox::Canvas canvas) -> ox::Cursor override
    {
        ox::fill(canvas, U' ' | ox::bg(ox::RGB{0x182d24}));
        ox::put(canvas, {1, 1}, "Hello, terminal", {.foreground = ox::colors::white});
        return {};
    }
};

int main()
{
    auto notice = Notice{};
    return ox::Application{notice}.run();
}
```

Use `paint_background` for a container fill and `paint_overlay` for decorations that
go over its children. [Writing a custom widget](widget-api.md#painting) owns the paint
order, input effects, and cursor rules.

## Local coordinates and clipping

`canvas.size()` is the logical widget extent. `subcanvas(offset, size)` creates a
local view inside it; it cannot draw beyond the parent clip. `put` clips complete
Unicode graphemes without wrapping. Keep application text as UTF-8 strings; terminal
columns are not byte offsets. For storage, footprint replacement, and checked
coordinate arithmetic, see [Frames and canvases](internals/frame-and-canvas.md).

## Styles and glyphs

`put` accepts UTF-8/UTF-32 strings, glyph ranges, and line/box shapes. `fill` accepts
a styled glyph, a character, or an appearance change. A character fill preserves
existing styles; style/color fills preserve text. `clear` uses default-styled spaces.

A `Style` replaces a complete appearance. A `StylePatch` changes selected components;
missing colors retain the existing channel. Glyph ranges can take `Style defaults`:
only unspecified glyph colors inherit defaults, glyph traits combine with default
traits, and an explicit patch applies last. See the generated
[reference](https://a-n-t-h-o-n-y.github.io/TermOx/) for overloads and validation.

For outlines and titles, use [Borders and dividers](decorations.md).

## Pixel images

`PixelGrid` owns an RGB image. Each terminal cell displays two vertically adjacent
pixels using a half-block character, so a 20 × 20 image occupies 20 columns and
10 rows at its native size.

```cpp
#include <ox/ox.hpp>

int main()
{
    auto image = ox::PixelGrid{ox::Area{20, 20}, ox::RGB{0x182d24}};
    image.set_pixel({5, 5}, ox::RGB{0xffcc88});
    return ox::Application{image}.run();
}
```

Use `set_pixel` to update a pixel and request repaint, or `set_image` to replace
the image with row-major RGB data. Direct edits through `pixels()` or `at()` need
an application repaint request. Custom widgets can draw their own RGB storage with
`paint_pixels(canvas, size, pixels)`.

The [BMP viewer](../examples/bmp-viewer.md) demonstrates loading, zooming, and
panning. See the [API reference](https://a-n-t-h-o-n-y.github.io/TermOx/) for
PixelGrid and pixel drawing contracts.

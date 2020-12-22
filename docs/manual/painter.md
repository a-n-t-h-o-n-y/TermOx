# Painter

The Painter class provides methods for drawing Glyphs to the screen, this is
used in the `paint_event()` handler.

Each invocation of the paint event handler should create a new Painter object,
taking a reference to the Widget that will be painted to. All coordinates passed
to the Painter object will be local to the passed in Widget.

## Methods

### `void put(Glyph g, Point at)`

Places a Glyph at a local Point within the Widget, overriding any previously
placed Glyph.

### `void put(Glyph_string gs, Point at)`

Places the first Glyph of a Glyph_string at a local Point within the Widget with
all other Glyphs following from left to right, overriding any previously palced
Glyphs that the string would overlap with. If the string goes out of bounds,
those Glyphs are not drawn.

### `void fill(Glyph g, Point top_left, Area size)`

Fills in a Rectangle with the given Glyph. The top left corner is given by the
Point and the Area is the size of the space to fill.

### `void line(Glyph g, Point a, Point b)`

Fills in a line of the given Glyph from Point `a` to Point `b`. Diagonals are
not implemented yet.

## Example

```cpp
bool paint_event() override
{
    auto p = Painter{*this};
    p.put(L'X' | Trait::Bold, {3, 5});
    p.line(L'-', {0, 0}, {0, this->height() - 1});
    p.fill(L'#', Point{7, 2}, Area{5, 5});
    return Widget::paint_event();
}
```

## See Also

- [Reference](https://a-n-t-h-o-n-y.github.io/TermOx/classox_1_1Painter.html)

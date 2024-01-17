# Title Widget

[`<termox/widget/widgets/title.hpp>`](../../../include/termox/widget/widgets/title.hpp)

A single width/height Widget that can display a single Glyph.

```cpp
class Tile : public Widget {
   public:
    struct Parameters { Glyph tile = U' '; };

   public:
    Tile(Glyph tile = U' ');

    Tile(Parameters);

    void set_tile(Glyph tile);

    auto tile() -> Glyph;
};
```

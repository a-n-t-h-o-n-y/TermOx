# Color Select Widget

[`<termox/widget/widgets/color_select.hpp>`](../../../include/termox/widget/widgets/color_select.hpp)

Selectable colors tiles for each color in the currently set palette. Auto
updates when `Terminal::set_palette()` succeeds.

```cpp
class Color_select : public layout::Vertical<Color_line> {
   public:
    struct Parameters { Color_tile::Display display = Color_tile::Display::None; };

    sl::Signal<void(Color)> color_selected;

   public:
    Color_select(Color_tile::Display display = Color_tile::Display::None);

    Color_select(Parameters );
};

class Color_tile : public Button {
   public:
    enum class Display { None, Number };

    struct Parameters {
        Color color;
        Display display = Display::None;
    };

   public:
    Color_tile(Color c, Display display = Display::None);
    Color_tile(Parameters);
};

using Color_line = layout::Horizontal<Color_tile>;
```

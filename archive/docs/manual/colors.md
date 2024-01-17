# Colors

There are 16 pre-defined Color names, these are offered for convenience, and can
be given different names. These names are accessed via the `Color` struct, for
example `Color::Red`.

- `Black`
- `Dark_red`
- `Green`
- `Brown`
- `Dark_blue`
- `Violet`
- `Light_blue`
- `Light_gray`
- `Dark_gray`
- `Red`
- `Light_green`
- `Yellow`
- `Blue`
- `Orange`
- `Gray`
- `White`

Colors beyond these can be created by constructing a Color with a value between
[16 - 255], though these are only useful with a Color Palette that supports more
than 16 colors.

Colors can be applied to any Brush as either a background or a foreground color.

## Color Palette

Color Palettes can be used to define which color is attached to each Color
value.

`Color_definition`s are used to define Colors. There are three ways to use this
struct:

- XTerm-like Color Palette Index
- True Color
- Dynamic Color

All three can be mixed together in a `Palette`, which is a type alias for
`std::vector<Color_definition>`.

### XTERM-lib Color Palette Index

The first method uses the 256 colors that ANSI/XTERM like terminals provide.

A listing of the 256 ANSI colors can be found
[here](https://wikipedia.org/wiki/ANSI_escape_code#Colors), under the 8-bit
heading.

[This](https://jonasjacek.github.io/colors/) is another useful chart, with the
values used for each color.

```cpp
auto const green = Color_definition{Color::Green, Color_index{28}};
```

The above line of code will tie `Color::Green` with the `Color_index` 28.

### True Color

The second use of `Color_definition` uses 'True Color' values, these are 24 bit
RGB or HSL values that allow up to 16 million different colors.

```cpp
auto const green = Color_definition{Color::Green,  RGB{0x5ac54f}};
auto const grey  = Color_definition{Color::Gray,   RGB{123, 123, 123}};
auto const pink  = Color_definition{Color::Violet, HSL{324, 100, 50}};
```

### Dynamic Colors

Dynamic Colors are animated colors. Defined as a struct containing an interval
(in ms) and a `std::function<True_color()>` to get the color after the interval
has passed.

These take a bit of work to define, so the library provides a few pre-defined
dynamic colors:

- [Rainbow](rainbow.md)
- [Fade](fade.md)

```cpp
auto const rb  = Color_definition{Color{45},
                    dynamic::rainbow(period, saturation, lightness)};

auto const fd  = Color_definition{Color{46},
                    dynamic::fade<dynamic::Sine>(HSL{50, 25, 25}, HSL{100, 50, 50})};
```

### Example Color Palette

Creating a `Palette` with each of the three `Color_definition` types and
custom Color names:

```cpp
namespace three_color {

constexpr auto Yellow  = Color{0};
constexpr auto Blue    = Color{1};
constexpr auto Rainbow = Color{2};

inline auto const palette = Palette {
    {Yellow,  Color_index{228}  },
    {Blue,    RGB{0x20283d}     },
    {Rainbow, dynamic::rainbow()}
};

} // namespace three_color

// Later...
Terminal::set_palette(three_color::palette);

// Later...
auto const glyph = U'X' | bg(three_color::Rainbow);
```

Palettes can be set by calling the `Terminal::set_palette(...)` method.

### Library Color Palettes

There are 24 pre-defined palettes in the library:

- `amstrad_cpc`
- `apple_ii`
- `ashes`
- `basic`
- `basic8`
- `cga`
- `commodore_64`
- `commodore_vic20`
- `dawn_bringer16`
- `dawn_bringer32`
- `en4`
- `gameboy`
- `gameboy_pocket`
- `macintosh_ii`
- `msx`
- `nes`
- `savanna`
- `secam`
- `stormy6`
- `teletext`
- `thomson_m05`
- `windows`
- `windows_console`
- `zx_spectrum`

## See Also

- [Reference](https://a-n-t-h-o-n-y.github.io/TermOx/structox_1_1Color.html)

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

- ANSI/XTERM Color
- True Color
- Dynamic Color

All three can be mixed together in a `Palette`, which is a type alias for
`std::vector<Color_definition>`.

### ANSI/XTERM Values

The first method uses the 256 colors that ANSI/XTERM terminals provide.

A listing of the 256 ANSI colors can be found
[here](https://wikipedia.org/wiki/ANSI_escape_code#Colors), under the 8-bit
heading.

[This](https://jonasjacek.github.io/colors/) is another useful chart, with the
values used for each color.

```cpp
auto const green = Color_definition{Color::Green, ANSI{28}};
```

The above line of code will tie `Color::Green` with the ANSI color at 28.

### True Color

The second use of `Color_definition` uses true color values, this allows the
program to redefine ANSI color values with RGB, HSL, or HEX values.

```cpp
auto const green = Color_definition{Color::Green,  ANSI{18}, 0x5ac54f};
auto const grey  = Color_definition{Color::Gray,   ANSI{19}, RGB{123, 123, 123}};
auto const pink  = Color_definition{Color::Violet, ANSI{20}, HSL{324, 100, 50}};
```

In the above code, the ANSI values are the terminal color values that are being
re-defined. Some terminals do not reset these when the program exits, so it is
suggested to not use the terminal's default colors (ANSI values 0-15), since
this will overwrite those.

### Dynamic Colors

Dynamic Colors are animated colors. Defined as a struct containing an interval
(in ms) and a `std::function<True_color()>` to get the color after the interval
has passed.

These take a bit of work to define, so the library provides a few pre-defined
dynamic colors:

- [Rainbow](rainbow.md)
- [Fade](fade.md)

```cpp
auto const rb  = Color_definition{Color{45}, ANSI{100},
                    dynamic::rainbow(period, saturation, lightness)};

auto const fd  = Color_definition{Color{46}, ANSI{101},
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
    {Yellow,  ANSI{228}                     },
    {Blue,    ANSI{16}, True_color{0x20283d}},
    {Rainbow, ANSI{17}, dynamic::rainbow()  }
};

} // namespace three_color

// Later...
System::terminal.set_palette(three_color::palette);

// Later...
auto const glyph = L'X' | bg(three_color::Rainbow);
```

Palettes can support up to 181 colors at once. They can be set by calling the
`System::terminal.set_palette(...)` method.

### Library Color Palettes

There are 24 pre-defined palettes in the library:

- amstrad_cpc
- apple_ii
- ashes
- basic
- basic8
- cga
- commodore_64
- commodore_vic20
- dawn_bringer16
- dawn_bringer32
- en4
- gameboy
- gameboy_pocket
- macintosh_ii
- msx
- nes
- savanna
- secam
- stormy6
- teletext
- thomson_m05
- windows
- windows_console
- zx_spectrum

## See Also

- [Reference](https://a-n-t-h-o-n-y.github.io/TermOx/structox_1_1Color.html)

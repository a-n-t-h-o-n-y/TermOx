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

Colors beyond these can be created by constructing a `Color` with a value
between [16 - 255], though these are only useful with a Color Palette that
supports more than 16 colors.

Colors can be renamed, just create a new variable with the desired color name:

```cpp
auto constexpr Turquoise = Color{4};
```

Colors can be applied as either background or foreground colors to the default
Brush of a Widget, or to individual Glyphs.

## Color Palette

Color palettes can be used to redefine what color is displayed for each `Color`
value. There are two ways to define a color palette in CPPurses, the first
method uses the pre-defined 256 colors that ANSI/XTerm terminals provide, and
the second method allows RGB/Hex true color values to be specified for each
`Color`.

Both palette types can be set by calling the `System::terminal.set_palette(...)`
method.

Palettes support up to 181 colors at once.

### ANSI / XTerm

An `ANSI_palette` is created by pairing each `Color` with an `ANSI` color value.

```cpp
auto const my_palette = ANSI_palette{{
    {Color::Black,       ANSI{234}},
    {Color::Dark_red,    ANSI{88}},
    {Color::Green,       ANSI{28}},
    {Color::Brown,       ANSI{94}},
    {Color::Dark_blue,   ANSI{18}},
    {Color{5},           ANSI{90}},
    {Color{6},           ANSI{45}},
    {Color::Light_gray,  ANSI{250}},
    {Color::Dark_gray,   ANSI{239}},
    {Color::Red,         ANSI{160}},
    {Color::Light_green, ANSI{46}},
    {Color::Yellow,      ANSI{227}},
    {Color::Blue,        ANSI{27}},
    {Color::Orange,      ANSI{214}},
    {Color::Gray,        ANSI{246}},
    {Color::White,       ANSI{231}},
}};

System::terminal.set_palette(my_palette);
```


A listing of the 256 ANSI colors can be found
[here](https://wikipedia.org/wiki/ANSI_escape_code#Colors), under the 8-bit
heading.

[This](https://jonasjacek.github.io/colors/) is another useful chart, with the
values used for each color.

### True Color

A `True_color_palette` pairs an ANSI color definition with a `True_color` value,
either in RBG or Hex. This is similar to the above ANSI definition, but with a
third column. R, G, and B values must be between [0 - 255].

```cpp
auto const my_palette = True_color_palette{{
    {{Color{0},      ANSI{16}}, True_color{0x050505}},
    {{Color{1},          {17}},           {0xff0040}},
    {{Color::Green,      {18}},           {0x5ac54f}},
    {{Color{3},          {19}},           {0xdad45e}},
    {{Color{4},          {20}},           {0xfdfff7}},
    {{Color{5},          {21}}, True_color{30, 0, 205}},
    {{Color::Light_blue, {22}},           {0xfef4b6}},
    {{Color::Light_gray, {23}},           {0xb4b4b4}},
    {{Color{8},          {24}},           {0x1a1a1a}},
    {{Color{9},          {25}},           {0x48b6fc}},
    {{Color{10},         {26}},           {0xd6d4ce}},
    {{Color{11},         {27}},           {0xa0893f}},
    {{Color{12},         {28}},           {0x0f0f0f}},
    {{Color{13},         {29}},           {0xfcbf8d}},
    {{Color{14},         {30}},           {0x90d4d4}},
    {{Color{15},         {31}},           {0xffffff}},
}};


System::terminal.set_palette(my_palette);
```

The ANSI value is the terminal color value that is being altered and is needed
for flexiblity. When switching between multiple palettes at runtime, this may be
useful to control.

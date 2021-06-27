# Borders

A Border can be wrapped around a single Widget to provide a one cell thick
Glyph border.

```cpp
using Wall = std::optional<Glyph>;

struct Border {
    Wall north;
    Wall south;
    Wall east;
    Wall west;
    Glyph nw_corner;  // North West
    Glyph ne_corner;  // North East
    Glyph sw_corner;  // South West
    Glyph se_corner;  // South East
};
```

Walls that do not contain a value do not form a border around the Widget on that
side. Any corner touching a Wall without a value is likewise not used.

Borders can be combined with Colors and Traits using the pipe operator(`|`).
This will modify all walls and corners of the border. Other Border modifying
functions are the `drop_` and `take_` families. The drop functions will remove
the named Wall from the Border. The take functions remove every Wall except the
named Wall from the Border.

```cpp
border::rounded() | fg(Color::Orange) | Trait::Inverse | pipe::drop_east();
```

To decorate a Widget with a Border, the class template `Bordered` is provided:

```cpp
struct Textboxes : HPair<Textbox, Bordered<Textbox>> {
    Textbox& box_1 = this->first;
    Textbox& box_2 = this->second | border::rounded() | fixed_width(17) | wrapped();
    // pipe::wrapped() returns a reference to the Widget wrapped in the Border.
    // The fixed width of 17 applies to the Border and wrapped Widget together.
};
```

There is a `Passive` class template specialization for `Bordered` that will give
the Bordered object the same size policy as the wrapped Widget, plus space for
the Border Glyphs:

```cpp
struct Textboxes : HPair<Textbox, Passive<Bordered<Textbox>>> {
    Textbox& box_1 = this->first;
    Textbox& box_2 = this->second | wrapped() | fixed_width(15);
    // box_2 will have width 15 and border with box_2 will have width 17.
};
```

## Predefined Borders

These are located in the `ox::border` namespace. They can be combined with a
`Bordered` object and the pipe operator to assign a given `Border` look.

`bordered_textbox | border::rounded();`

```
   squared()        rounded()      plus_corners()     asterisk()
    ┌────┐           ╭────╮           +────+           ******
    │    │           │    │           │    │           *    *
    └────┘           ╰────╯           +────+           ******

   dashed_1()       dashed_2()       dashed_3()       dashed_4()
    ┌╶╶╶╶┐           ┌╌╌╌╌┐           ┌┄┄┄┄┐           ┌┈┈┈┈┐
    ╷    ╷           ╎    ╎           ┆    ┆           ┊    ┊
    └╶╶╶╶┘           └╌╌╌╌┘           └┄┄┄┄┘           └┈┈┈┈┘

bold_dashed_1()  bold_dashed_2()  bold_dashed_3()  bold_dashed_4()
    ┏╺╺╺╺┓           ┏╍╍╍╍┓           ┏┅┅┅┅┓           ┏┉┉┉┉┓
    ╻    ╻           ╏    ╏           ┇    ┇           ┋    ┋
    ┗╺╺╺╺┛           ┗╍╍╍╍┛           ┗┅┅┅┅┛           ┗┉┉┉┉┛

   block_1()        block_2()        block_3()        block_4()
    ██████           ▓▓▓▓▓▓           ▒▒▒▒▒▒           ░░░░░░
    █    █           ▓    ▓           ▒    ▒           ░    ░
    ██████           ▓▓▓▓▓▓           ▒▒▒▒▒▒           ░░░░░░

 half_block()                        doubled()         bold()      none()
          # Can't be fully rendered   ╔════╗           ┏━━━━┓
    ▌     # in ascii without          ║    ║           ┃    ┃
    ▙▄▄▄▄ # inverting chars.          ╚════╝           ┗━━━━┛
```

Most fonts will connect the adjacent walls and corners completely.

## Pipe Methods

These functions are combined with a `Border` and the pipe operator(`|`).

- `drop_north()`
- `drop_south()`
- `drop_east()`
- `drop_west()`
- `take_north()`
- `take_south()`
- `take_east()`
- `take_west()`

## See Also

- [Reference](https://a-n-t-h-o-n-y.github.io/TermOx/classox_1_1Bordered.html)

<!-- TODO replace the above reference link if it is not valid. -->

# Brush

A `Brush` is used to define the look of Glyphs on the terminal screen.  A
Brush contains a list of [Traits](traits.md) (such as Bold, Inverse, Italic,
...), and a background and foreground [Color](colors.md).

Each Glyph has its own Brush. Widget also owns a Brush, which is applied to each
Glyph painted by the Widget; if the Glyph's Brush does not have a color set, it
takes the Widget Brush's Color, and imprints its Traits on top of the Glyph's
Traits.

## Modifying Methods

#### `Brush(Traits...)`

The constructor will take any number of Traits and background and foreground
colors. Calling the `bg(Color)` and `fg(Color)` methods will return a color type
that is marked as either a background or foreground color, respectively.

```cpp
auto b = Brush{Trait::Bold, bg(Color::Dark_gray), fg(Color::Yellow)};
```

#### `void add_traits(Traits...)`

Add any number of Traits or Colors to an existing Brush.

#### `void remove_traits(Traits...)`

Remove any number of Traits, not Colors.

#### `void set_background(Color)`

Set the background Color.

#### `void set_foreground(Color)`

Set the foreground Color.

#### `void remove_background()`

Remove the background Color.

#### `void remove_foreground()`

Remove the foreground Color.

#### `void clear_traits()`

Removes all Traits, not Colors.

## Pipe Methods

- `operator|(..., Background_color)`
- `operator|(..., Foreground_color)`
- `remove_background()`
- `remove_foreground()`
- `operator|(..., Trait)`
- `discard(Trait)`
- `clear_traits()`

## See Also

- [Reference](https://a-n-t-h-o-n-y.github.io/TermOx/classox_1_1Brush.html)
- [Traits](traits.md)
- [Colors](colors.md)
- [Wallpaper](wallpaper.md)

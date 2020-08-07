# Brush

A `Brush` is used to define the look of `Glyph`s on the terminal screen.  It
contains Colors and Traits. Both `Glyph` and `Widget` have a Brush. A Widget's
Brush will be imprinted onto each Glyph placed by that Widget. This
imprinting acts as a default look. If a Glyph does not have a background color
defined, it will take the Widget's background color, etc...

A Brush contains a list of Traits (such as Bold, Inverse, Italic...), a
background Color, and a foreground Color. The Colors are optional, deferring to
defaults if not set.

## Modifying Methods

#### `Brush(Traits...)`

The constructor will take any number of Traits and background and foreground
colors. Calling the `background(Color)` and `foreground(Color)` methods will
return a color type that is marked as either a background or foreground color,
respectively.

```cpp
auto b = Brush{Trait::Bold, background(Color::Dark_gray), foreground(Color::Yellow)};
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

- `bg(Color)`
- `fg(Color)`
- `remove_background()`
- `remove_foreground()`
- `add(Trait)`
- `discard(Trait)`
- `clear_traits()`

## See Also

- [Wallpaper](wallpaper.md)
- [Traits](traits.md)
- [Colors](colors.md)

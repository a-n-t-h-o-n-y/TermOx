# Brush

A `Brush` is used to define the look of `Glyph`s on the terminal screen.  It
contains Colors and Traits. Both `Glyph` and `Widget` have a Brush. A Widget's
Brush will be imprinted onto each Glyph placed by that Widget. This
imprinting acts as a default look. If a Glyph does not have a background color
defined, it will take the Widget's background color, etc...

A Brush contains a list of Traits (such as Bold, Inverse, Italic...), a
background Color, and a foreground Color. The Colors are `std::optional<Color>`
objects.

## See Also

- [Wallpaper](wallpaper.md)
- [Traits](traits.md)
- [Colors](colors.md)

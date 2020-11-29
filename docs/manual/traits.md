# Traits

The `Trait` enum is used within Brush objects to alter the appearance of Glyphs
on screen.

Not all Traits are supported by all terminals.

There are 8 Traits:

- `Bold`
- `Italic`
- `Underline`
- `Standout`
- `Dim`
- `Inverse`
- `Invisible`
- `Blink`

These can be applied to a Brush with the `Brush::add_traits(Trait...)` method,
this will take any number of Traits and apply them to the Brush.

## See Also

- [Brush](brush.md)

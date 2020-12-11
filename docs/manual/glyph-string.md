# Glyph String

A `Glyph_string` is a vector-like container of `Glyphs`. Most methods of
`std::vector` are avaliable for `Glyph_string`.

## User Defined Literal

The UDL `_gs` can be used with `char` string literals and `wchar_t` string
literals to create `Glyph_strings`.

## Pipe Operator

Traits and Colors can be used with the pipe operator to alter the Brush of each
Glyph in the `Glyph_string`.

```cpp
auto const foo = L"hello!" | Trait::Bold | bg(Color::Light_green);
foo | fg(Color::Violet);
```

## See Also

- [Reference](https://a-n-t-h-o-n-y.github.io/CPPurses/classcppurses_1_1Glyph__string.html)
- [Glyph](glyph.md)
- [Brush](brush.md)

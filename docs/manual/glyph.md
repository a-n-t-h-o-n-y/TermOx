# Glyph

Each cell on the terminal can be represented with a Glyph. A Glyph is composed
of a `wchar_t` and a `Brush` object.

## Construction

There are a few ways to create a Glyph.

With the constructors:

```cpp
constexpr Glyph(wchar_t sym, Brush b);
```

```cpp
template <typename... Traits>
constexpr Glyph(wchar_t sym, Traits&&... traits);
```

With `wchar_t` in combination with the pipe operator:

```cpp
auto const bold_x = L'X' | Trait::Bold;
```

## Pipe Operator

Glyphs and `wchar_t` can be combined with Traits and Colors using the pipe
operator `|`. These are modifying operations when applied to l-value Glyphs.

### Traits

```cpp
auto fancy_x = L'X' | Trait::Bold | Trait::Inverse;
fancy_x | Trait::Italic;
```

### Colors

```cpp
auto const red_x     = L'X' | fg(Color::Red);
auto const bg_blue_x = L'X' | bg(Color::Blue);
```

## See Also

- [Reference](https://a-n-t-h-o-n-y.github.io/TermOx/structox_1_1Glyph.html)
- [Brush](brush.md)

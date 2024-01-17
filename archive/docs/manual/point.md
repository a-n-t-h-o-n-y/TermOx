# Point

The `Point` struct is used to define a two-dimensional position on the physical
terminal screen. Each unit is equal to a single text cell. `Point` is either
used with respect to the top-left corner of the terminal screen (Global), or
with respect to the top-left corner of the a Widget (Local). `x` is the
horizontal dimension and `y` is the vertical dimension. Each unit of length is
equal to a single text cell on the terminal screen.

```cpp
struct Point {
    std::size_t x = 0;
    std::size_t y = 0;
};
```

## See Also

- [Reference](https://a-n-t-h-o-n-y.github.io/TermOx/structox_1_1Point.html)

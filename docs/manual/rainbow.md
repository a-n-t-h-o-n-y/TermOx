# Rainbow Dynamic Color

The rainbow dynamic color is provided via the `rainbow(...)` free function in
the `termox/painter/dynamic_colors.hpp` header.

```cpp
auto rainbow(Dynamic_color::Period_t period = std::chrono::milliseconds{40},
             std::uint8_t saturation        = 75,
             std::uint8_t lightness         = 75) -> Dynamic_color;
```

This will rotate through the Hue of the HSL color wheel, with the given
`saturation` and `lightness` fixed. The `period` is not the period of time to
rotate through the entire wheel, but rather the period of time between
increments(by one) of the Hue.

This creates a `Dynamic_color` that can be used in a `Palette`.

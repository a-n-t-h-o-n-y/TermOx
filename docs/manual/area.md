# Area

The `Area` struct is used to define the two-dimensions of a Widget's physical
screen space. Each unit of length is equal to a single text cell on the terminal
screen.

```cpp
struct Area {
    std::size_t width;   // Horizontal
    std::size_t height;  // Vertical
};
```

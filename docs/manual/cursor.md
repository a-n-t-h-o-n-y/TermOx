# Cursor

`Widget` holds an instance of the `Cursor` class named `cursor`. This provides
basic info about the cursor for the given Widget.

## Reference

#### `sl::Signal<void(Point)> moved;`

This signal is called every time the cursor is moved.

#### `bool enabled() const;`

Returns `true` if the cursor is able to display on screen for this Widget.

#### `std::size_t x() const;`

Returns the local `x` coordinate of the cursor.

#### `std::size_t y() const;`

Returns the local `y` coordinate of the cursor.

#### `Point position() const;`

Returns the `Point` where the cursor currently is.

#### `void enable(bool enable = true);`

Enable(or disable) the cursor, if enabled it will display when this Widget is in
focus.

#### `void disable(bool disable = true);`

Disable(or enable) the cursor, if enabled it will display when this Widget is in
focus.

#### `void toggle();`

Enable the cursor if disabled, or disable it if enabled.

#### `void set_x(std::size_t x);`

Set the local x coordinate of the cursor.

#### `void set_y(std::size_t y);`

Set the local y coordinate of the cursor.

#### `void set_position(Point p);`

Set the local position of the cursor.

## See Also

- [Reference](https://a-n-t-h-o-n-y.github.io/TermOx/classox_1_1Cursor.html)

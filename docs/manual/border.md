# Border

Each Widget has a `Border` object, named `border`, that can be used to outline
the interior edges of the Widget. Borders can be enabled and disabled, they also
hold a `Segments` struct, which holds a Glyph for each wall/corner of the
Border.

The space for the Border comes from the Widget's Area. If a Border is enabled,
the geometry of the Widget is affected, the various access methods of Widget
account for this:

### `Widget::area() -> Area`

This returns the area of the Widget without the Border, the space that can be
painted to. This area starts at `Widget::inner_top_left() -> Point`.

### `Widget::outer_area() -> Area`

This returns the entire area of the Widget, including Border space.

### `Widget::inner_top_left() -> Point`

This returns the top left coordinate of the widget, not including the Border.

### `Widget::top_left() -> Point`

This returns the top left coordinate of the Widget, including any Border space.

## Pipe Methods

### Enabling Border and Walls

These enable/disable segments of the Border.

- `bordered()`
- `not_bordered()`
- `north_border()`
- `south_border()`
- `east_border()`
- `west_border()`
- `north_east_border()`
- `north_west_border()`
- `south_east_border()`
- `south_west_border()`
- `north_south_border()`
- `east_west_border()`
- `corners_border()`
- `no_corners_border()`
- `no_walls_border()`

### Setting Wall Look

These do not enable the border to display automatically.

- `walls(Traits...)`
- `north_wall(Glyph)`
- `north_wall(Traits...)`
- `south_wall(Glyph)`
- `south_wall(Traits...)`
- `east_wall(Glyph)`
- `east_wall(Traits...)`
- `west_wall(Glyph)`
- `west_wall(Traits...)`
- `north_south_walls(Glyph)`
- `north_south_walls(Traits...)`
- `east_west_walls(Glyph)`
- `east_west_walls(Traits...)`
- `north_east_corner(Glyph)`
- `north_east_corner(Traits...)`
- `north_east_walls(Glyph)`
- `north_east_walls(Traits...)`
- `north_west_corner(Glyph)`
- `north_west_corner(Traits...)`
- `north_west_walls(Glyph)`
- `north_west_walls(Traits...)`
- `south_east_corner(Glyph)`
- `south_east_corner(Traits...)`
- `south_east_walls(Glyph)`
- `south_east_walls(Traits...)`
- `south_west_corner(Glyph)`
- `south_west_corner(Traits...)`
- `south_west_walls(Glyph)`
- `south_west_walls(Traits...)`

### Pre-Formatted Borders

These do not enable the border to display automatically.

- `squared_corners()`
- `rounded_corners()`
- `plus_corners()`
- `asterisk_walls()`
- `doubled_walls()`
- `bold_walls()`
- `dashed_walls_1()`
- `dashed_walls_2()`
- `dashed_walls_3()`
- `dashed_walls_4()`
- `bold_dashed_walls_1()`
- `bold_dashed_walls_2()`
- `bold_dashed_walls_3()`
- `bold_dashed_walls_4()`
- `block_walls_1()`
- `block_walls_2()`
- `block_walls_3()`
- `block_walls_4()`
- `half_block_walls()`
- `half_block_inner_walls_1()`
- `half_block_inner_walls_2()`
- `block_corners()`
- `floating_block_corners()`

## See Also

- [Reference](https://a-n-t-h-o-n-y.github.io/TermOx/classox_1_1Border.html)

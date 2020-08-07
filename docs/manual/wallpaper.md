# Wallpaper

Each Widget has a Wallpaper Glyph. This is the Glyph used to fill in any empty
space not touched by the Painter in `paint_event()`.

Wallpaper is optional, and if not set will not fill empty space.

## Methods

#### `Widget::set_wallpaper(Glyph)`

Will set the Wallpaper to the given Glyph.

#### `Widget::set_wallpaper(std::nullopt_t)`

By passing in `std::nullopt`, this will unset the Wallpaper.

#### `Widget::paint_wallpaper_with_brush(bool paints = true)`

If this is enabled, it will apply the Widget's default Brush to the Wallpaper.
This is defaulted to `true` at Widget construction.

## Pipe Methods

- `wallpaper(Glyph)`
- `wallpaper(std::nullopt_t)`
- `wallpaper_with_brush()`
- `wallpaper_without_brush()`

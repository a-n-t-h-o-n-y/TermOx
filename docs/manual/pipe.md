# Pipe Namespace

The pipe namespace provides operations that allow a declarative approach to
modifying Widgets. The pipe operator `|` is used to chain together calls to
these methods.

```cpp
using namespace ox::pipe;
my_widget | click_focus() | fixed_height(1) | bg(Color::Blue) | fg(Color::Red)
          | bind_key(Key::H, [](auto& w){ log(w.name()); });
```

It is common to use these operations within a Widget's constructor, from there
you can use `*this` to start the chain.

## Collections

Beyond just a single Widget, you can apply these operations to collections of
Widgets. The `pipe::children()` and `pipe::descendants()` methods will give you
collections that can be used with the pipe operator, the operation will be
applied to every Widget in the collection.

### Filtering

A collection can be filtered with a predicate. The predicate function should
take a `Layout::Child_t const&` and return a bool. This will remove any Widgets
where the predicate returns false.

```cpp
template <typename F>
filter(F&& predicate);
```

```cpp
using namespace ox::pipe;
my_widget | children() | filter([](const& w) { return w.brush.has_trait(Trait::Bold); })
          | wallpaper(L'#' | fg(Color::Violet));
```

Filtering can also happen based on the Widget's type. This filter will only
allow Widgets that can be `dynamic_cast`ed to the given type to pass through.
This does use `dynamic_cast`, so it is best this isn't used in any performance
sensitive code.

```cpp
template <typename Widget_t>
filter();
```

```cpp
using namespace ox::pipe;
my_widget | descendants() | filter<Button>() | on_press([&]{ my_widget | bg(Color::Red); });
```

### `for_each`

The `pipe::for_each(...)` method will apply the given function to each Widget in
the collection, this can take a non-const reference to the Widget type and
perform modifying operations.

```cpp
template <typename F>
for_each(F&& f)
```

```cpp
using namespace ox::pipe;
my_widget | children() | for_each([](auto& w){ log(w.name()); });
```

## Operations

### General

- `name(std::string name)`
- `install_filter(Widget& filter)`
- `remove_filter(Widget& filter)`

### Animation

- `animate(Animation_engine::Period_t period)`
- `animate(FPS fps)`
- `disanimate()`

### Wallpaper

- `wallpaper(Glyph g)`
- `wallpaper(std::nullopt_t)`
- `wallpaper_with_brush()`
- `wallpaper_without_brush()`

### Brush

- `bg(Color)`
- `fg(Color)`
- `add(Trait)`
- `discard(Trait)`
- `clear_traits()`

### Cursor

- `show_cursor()`
- `hide_cursor()`
- `put_cursor(Point)`

### Focus Policy

- `focus(Focus_policy)`
- `no_focus()`
- `tab_focus()`
- `click_focus()`
- `strong_focus()`
- `direct_focus()`

### Size Policy

#### Width

- `fixed_width(std::size_t hint)`
- `minimum_width(std::size_t hint)`
- `maximum_width(std::size_t hint)`
- `preferred_width(std::size_t hint)`
- `expanding_width(std::size_t hint)`
- `minimum_expanding_width(std::size_t hint)`
- `ignored_width()`
- `width_hint(std::size_t hint)`
- `width_min(std::size_t min)`
- `width_max(std::size_t max)`
- `width_stretch(double stretch)`
- `can_ignore_width_min()`
- `cannot_ignore_width_min()`
- `passive_width(bool x = true)`

#### Height

- `fixed_height(std::size_t hint)`
- `minimum_height(std::size_t hint)`
- `maximum_height(std::size_t hint)`
- `preferred_height(std::size_t hint)`
- `expanding_height(std::size_t hint)`
- `minimum_expanding_height(std::size_t hint)`
- `ignored_height()`
- `height_hint(std::size_t hint)`
- `height_min(std::size_t min)`
- `height_max(std::size_t max)`
- `height_stretch(double stretch)`
- `can_ignore_height_min()`
- `cannot_ignore_height_min()`
- `passive_height(bool x = true)`

### Border

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

#### Border Glyph and Trait Setters

- `walls(Trait...)`
- `north_wall(Glyph)`
- `north_wall(Trait...)`
- `south_wall(Glyph)`
- `south_wall(Trait...)`
- `east_wall(Glyph)`
- `east_wall(Trait...)`
- `west_wall(Glyph)`
- `west_wall(Trait...)`
- `north_south_walls(Glyph)`
- `north_south_walls(Trait...)`
- `east_west_walls(Glyph)`
- `east_west_walls(Trait...)`
- `north_east_corner(Glyph)`
- `north_east_corner(Trait...)`
- `north_east_walls(Glyph)`
- `north_east_walls(Trait...)`
- `north_west_corner(Glyph)`
- `north_west_corner(Trait...)`
- `north_west_walls(Glyph)`
- `north_west_walls(Trait...)`
- `south_east_corner(Glyph)`
- `south_east_corner(Trait...)`
- `south_east_walls(Glyph)`
- `south_east_walls(Trait...)`
- `south_west_corner(Glyph)`
- `south_west_corner(Trait...)`
- `south_west_walls(Glyph)`
- `south_west_walls(Trait...)`
- `squared_corners()`
- `rounded_corners()`

#### Pre-Built Border Designs

- `plus_corners()`
- `asterisk_walls()`
- `doubled_walls()`
- `bold_walls()`
- `dashed_walls_1()`
- `bold_dashed_walls_1()`
- `dashed_walls_2()`
- `bold_dashed_walls_2()`
- `dashed_walls_3()`
- `bold_dashed_walls_3()`
- `dashed_walls_4()`
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

### Signal Handlers

- `on_enable(Handler)`
- `on_disable(Handler)`
- `on_child_added(Handler)`
- `on_child_removed(Handler)`
- `on_child_polished(Handler)`
- `on_move(Handler)`
- `on_resize(Handler)`
- `on_mouse_press(Handler)`
- `on_left_click(Handler)`
- `on_middle_click(Handler)`
- `on_right_click(Handler)`
- `on_mouse_release(Handler)`
- `on_mouse_double_click(Handler)`
- `on_mouse_move(Handler)`
- `on_key_press(Handler)`
- `bind_key(Key::Code key, Handler)`
- `on_focus_in(Handler)`
- `on_focus_out(Handler)`
- `on_paint(Handler)`
- `on_timer(Handler)`
- `on_destroyed(Handler)`
- `on_color_selected(Handler)`
- `on_press(Handler)`

### From Widget Library

- `active_page(std::size_t index)`
- `label(Glyph_string)`
- `divider(Glyph)`
- `word_wrap(bool enable)`
- `contents(Glyph_string)`
- `align_left()`
- `align_center()`
- `align_right()`
- `ghost(Color)`
- `dynamic_width(bool enable)`

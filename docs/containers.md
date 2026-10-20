# Layout and containers

Use `Row` and `Column` to compose widgets, then add padding, borders, or scrolling
around the result. [Getting Started](getting_started.md) has a complete first project.
For the allocation algorithm and callback rules, see [layout internals](internals/layout.md).

## Size policies

`width_policy` and `height_policy` control each axis independently. Common choices
are `SizePolicy::fixed(cells)`, `min(cells)`, `max(cells)`, and `flex(weight)`.
Single-line controls normally reserve one row; flexible widgets use spare space.
`widget | SizePolicy` sets both axes, so edit individual policies for a one-axis constraint.

A widget's `measure(available, glyph_width_policy)` reports its natural content size without
arranging or painting. Containers combine measurements with the axis policies.
When minimum sizes cannot fit, the layout shrinks children and gaps to the available
area. The [allocation rules](internals/layout.md#allocation-rules) explain caps,
weights, and rounding.

## Row and Column

```cpp
#include <ox/ox.hpp>

auto controls = ox::Row{ox::Label{"Name"}, ox::LineEdit{}};
controls.gap = 1;
controls.cross_align = ox::CrossAlign::Center;
auto padded = ox::Padding{std::move(controls), std::array{1, 2, 1, 2}};
auto pages = ox::Stack{std::move(padded), ox::TextBox{{.text = "Details"}}};
pages.select(1);
```

Rows place children horizontally; columns place them vertically. `gap` separates
visible children. `cross_align` selects `Stretch`, `Start`, `Center`, or `End` on
the other axis. Invisible children and root overlays consume no normal layout space.

Layouts retain ordinary tuples, pairs, arrays, vectors, lists, or other sized forward
ranges. Stored widget values are owned. A tuple of references borrows widgets instead:

```cpp
auto name = ox::LineEdit{};
auto save = ox::Button{{.label = {.text = "Save"}}};
auto row = ox::Row{std::tie(name, save)};
```

The borrowed widgets must outlive the layout and application using them. `children`
remains directly accessible; `get_child<I>(layout)` also accesses fixed tuple children,
including through a border. Const enumeration and const `get_child` are supported.

## Changing a layout

After editing `children`, a child's visibility, spacing, alignment, or size policy,
call `refresh()` on the layout whose children changed. It arranges at the current
bounds and requests application layout/repaint. Text and value setters request their
own documented effects; direct public field edits are not observed automatically.

```cpp
save.visible = false;
row.gap = 2;
row.refresh();
```

An explicit `arrange(layout, bounds)` also notifies descendants when bounds are
unchanged. During an arrangement hook, refresh the nested layout you changed.
Measurement must be side-effect-free; arrangement mutations must eventually settle.
See [refresh and callback mutations](internals/layout.md#refresh-and-callback-mutations)
for reentrancy, exception behavior, and retained plans.

## Padding

`Padding` owns one `child`, top/right/bottom/left `padding`, and a background `style`.
Its measurement adds the insets; arrangement clips the child to the interior.
Negative insets are rejected, and tiny views produce an empty interior. The background
paints before the child. Use [Border](decorations.md#borders) for an outline or title.

## Stack

`Stack` accepts the same tuple/range storage as Row and Column. Only its selected page
participates in the application tree: focus, timers, painting, and pointer capture
follow that page. Read `selected_index()` and change it with `select(optional<size_t>)`.
A change emits `on_select`; invalid indices throw, and `nullopt` selects no page.
Pages retain their own `visible` flags. Refresh after changing a dynamic page container.

For a labeled strip controlling a Stack, use [Tabs](selection-and-application.md#tabs).

## ScrollView

`ScrollView` owns one child and a clipped two-axis viewport. Supply `requested_size`
when a custom child needs a specific content extent. Read `content_size()`,
`viewport_size()`, and `scroll_offset()`; use `set_scroll_offset(Point)` or `reveal(Rect)`.
The child is arranged at a negative offset while input remains local to the child.
Wheel, arrow, page, Home, and End bindings adjust the viewport.

```cpp
auto image = ox::ScrollView{ox::PixelGrid{ox::Area{80, 80}}};
ox::arrange(image, {{}, {20, 10}});
image.set_scroll_offset({10, 5});
```

See [Link scrolling](scrolling.md) to connect either axis to a scrollbar, including
content/viewport notifications and custom scrollable widgets.

## Suspended

Use `Suspended` to center one owned child at a requested size:

```cpp
#include <ox/layout.hpp>

auto panel = ox::Suspended{ox::Widget{}, ox::Area{12, 4}};
ox::arrange(panel, {{}, {20, 10}});
panel.requested_size = {16, 6};
panel.refresh();
```

The requested extent is clamped to the available area. The child's size policy is
not consulted; the wrapper's own policy still controls its allocation by its parent.
Refreshing recomputes geometry even when parent bounds are unchanged. Invalid
requested dimensions leave child geometry intact. Direct `fill_glyph` changes need
a repaint request. Margins use `fill_outside`, described in
[frame and canvas internals](internals/frame-and-canvas.md#drawing).

## Split panes

Use `SplitPane` for two resizable panels:

```cpp
auto panes = ox::SplitPane{ox::TreeView{}, ox::TextBox{}, .3};
```

The pane owns both widgets and starts with 30% of the space assigned to the first.
Drag the divider, or focus it and use arrows, Home, and End. Set `vertical = true`
to stack the panels instead of placing them side by side.

`set_ratio` clamps a finite fraction to [0, 1] and emits `on_change` on a change.
Resizing preserves the requested ratio and each child's state and focus.
`first_minimum` and `second_minimum` constrain layout; when both cannot fit,
available space is shared proportionally. The divider occupies one cell.

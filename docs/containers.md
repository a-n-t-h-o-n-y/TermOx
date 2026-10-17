# Container composition

## On this page

- [Measurement and size policies](#measurement-and-size-policies)
- [Row and Column](#row-and-column)
- [Padding](#padding)
- [Stack](#stack)
- [ScrollView](#scrollview)
- [Paint layers and pointer capture](#paint-layers-and-pointer-capture)
- [Suspended](#suspended)
- [Shared scrolling interface](#shared-scrolling-interface)

## Measurement and size policies

Widgets measure their natural content with `measure(available, width_policy)`.
Measurement does not arrange or paint. `width_policy` and `height_policy` independently
control minimum, maximum, and flexible allocation. Single-line controls default to one
row; flexible widgets can fill remaining space. Applying `widget | SizePolicy` sets
both axes; set individual policies for one-axis constraints.

## Row and Column

Row and Column keep ordinary tuple/range storage, measured child sizes, reusable layout
plans, and move-aware child identities. Set `gap` for spacing and `cross_align` to
`Stretch`, `Start`, `Center`, or `End`. Under insufficient space, gaps and children shrink
within the available area. `refresh()` rearranges after public configuration/container
changes and schedules root layout/repaint when attached. Measurement overrides should
be side-effect-free, and arrangement hooks must settle their own mutations.

```cpp
#include <ox/ox.hpp>

auto controls = ox::Row{ox::Label{"Name"}, ox::LineEdit{}};
controls.gap = 1;
controls.cross_align = ox::CrossAlign::Center;
auto padded = ox::Padding{std::move(controls), std::array{1, 2, 1, 2}};
auto pages = ox::Stack{std::move(padded), ox::TextBox{{.text = "Details"}}};
pages.select(1);
```

## Padding

Padding owns one `child`, T/R/B/L `padding`, and a background `style`. Measurement adds
insets, arrangement clips the child to the interior, and background paints before it.
Negative insets are rejected; tiny views produce an empty interior.

## Stack

Stack keeps pages in the same ordinary containers as Row/Column. Only its selected page
participates in the tree, so focus, timers, painting, and pointer capture follow the
active page. `selected_index()` reads state; `select(optional<size_t>)` changes it and
emits `on_select`. Invalid indices throw; null selects no page. Pages retain their own
`visible` flags. Refresh after editing a dynamic pages container.

## ScrollView

ScrollView owns one `child`, measured content, and a clipped two-axis viewport. Supply
`requested_size` when a custom child needs a specific extent. Read `content_size()`,
`viewport_size()`, and `scroll_offset()`; call `set_scroll_offset(Point)` or `reveal(Rect)`.
The child is arranged at a negative offset, preserving local input coordinates and
inherited clipping. Wheel/arrow/page/home/end bindings adjust the viewport.
`on_scroll(Point, Area, Area)` reports changed offset/content/viewport. `link(view, bar)`
synchronizes either scrollbar axis and returns both connection identifiers.

```cpp
auto image = ox::ScrollView{ox::PixelGrid{ox::Area{80, 80}}};
ox::arrange(image, {{}, {20, 10}});
image.set_scroll_offset({10, 5});
auto bar = ox::ScrollBar{};
auto connections = ox::link(image, bar);
```

## Paint layers and pointer capture

Application painting runs background, own content, children, then overlay for each
widget. Use `paint_background` for fills and `paint_overlay` for decorations over children.
The focused widget's `paint` result supplies its cursor.

Pointer capture routes motion and matching release to the gesture owner even outside
its bounds; hover still follows hit testing. Capture follows moves and cancels on
hiding, disabling, removal, or destruction. Controls clear gesture state through
`pointer_cancel`. Application and standalone Focus roots also follow moves.

See [selection and application structure](selection-and-application.md) for ListView,
Select/ComboBox, Tabs, root overlays, menus, dialogs, modal scopes, and scoped shortcuts.

## Suspended

```cpp
#include <ox/layout.hpp>

auto panel = ox::Suspended{ox::Widget{}, ox::Area{12, 4}};
ox::arrange(panel, {{}, {20, 10}});
panel.requested_size = {16, 6};
panel.refresh();
```

Suspended owns its child. Its refresh recomputes centered, clamped geometry and notifies
the child even when geometry is unchanged, matching Row and Column. Painting composes
fill_outside; normal application traversal paints the child. Invalid requested sizes
leave child geometry unchanged. Direct fill_glyph changes need a repaint request.

## Shared scrolling interface

`Scrollable<T>` in `<ox/scrollbar.hpp>` describes a widget with `content_size()` and
`viewport_size()` returning `Area`, `scroll_offset()` returning `Point`,
`set_scroll_offset(Point)` returning whether it changed, and
`on_scroll(Point offset, Area content, Area viewport)`. Units are terminal columns
and rows. Implement this interface on a custom widget to use the same `link` function.
TextBox, LineEdit, ListView, DataTable, ScrollView, RadioGroup, TreeView, and LogView
satisfy the concept. TreeView and LogView expose it directly; callers need not reach
into their child widgets. RadioGroup scrolls vertically only: its x offset is always
zero and its content width equals the viewport width.

```cpp
auto log = ox::LogView{};
auto bar = ox::ScrollBar{}; // vertical by default
ox::link(log, bar);
```

`link` immediately synchronizes the bar's range and position. Geometry/content changes
and scrolling then synchronize both directions. One generic implementation handles all
widgets; it tracks their lifetimes and follows widget moves. Returned connection IDs
allow explicit disconnection from `view.on_scroll` and `bar.on_scroll`. Connections
remain active when the returned IDs are discarded. A horizontal bar uses
`ScrollBar{{.vertical = false}}`; link it separately to the same view.

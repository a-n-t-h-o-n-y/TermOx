# Container composition

Widgets measure their natural content with `measure(available, width_policy)`.
Measurement does not arrange or paint. `width_policy` and `height_policy` independently
control minimum, maximum, and flexible allocation. Single-line controls default to one
row; flexible widgets can fill remaining space. Applying `widget | SizePolicy` sets
both axes; set individual policies for one-axis constraints.

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

Padding owns one `child`, T/R/B/L `padding`, and a background `style`. Measurement adds
insets, arrangement clips the child to the interior, and background paints before it.
Negative insets are rejected; tiny views produce an empty interior.

Stack keeps pages in the same ordinary containers as Row/Column. Only its selected page
participates in the tree, so focus, timers, painting, and pointer capture follow the
active page. `selected_index()` reads state; `select(optional<size_t>)` changes it and
emits `on_select`. Invalid indices throw; null selects no page. Pages retain their own
`visible` flags. Refresh after editing a dynamic pages container.

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

Application painting runs background, own content, children, then overlay for each
widget. Use `paint_background` for fills and `paint_overlay` for decorations over children.
The focused widget's `paint` result supplies its cursor.

Pointer capture routes motion and matching release to the gesture owner even outside
its bounds; hover still follows hit testing. Capture follows moves and cancels on
hiding, disabling, removal, or destruction. Controls clear gesture state through
`pointer_cancel`. Application and standalone Focus roots also follow moves.

Signal emission snapshots entries: newly connected slots wait until the next emission,
disconnected pending slots are skipped, and destroying the signal safely cancels pending
callbacks. Mutable callbacks retain their state between emissions; copies own independent
callback state. TermOx vendors this implementation in `external/signals-light`. Named
Connection configuration and borrowed lifetime tracking remain unchanged.

See [selection and application structure](selection-and-application.md) for ListView,
Select/ComboBox, Tabs, root overlays, menus, dialogs, modal scopes, and scoped shortcuts.

# Layout

Use `Row` and `Column` to arrange widgets, then add padding, borders, or scrolling.
Short snippets below go inside `main()` with `<ox/ox.hpp>` included.

- [Rows, columns, and sizing](#rows-and-columns)
- [Changing a layout](#changing-a-layout)
- [Padding, borders, and dividers](#padding-borders-and-dividers)
- [Pages and split panes](#pages-and-split-panes)
- [Scrolling](#scrolling)
- [Centered content](#centered-content)

## Rows and columns

```cpp
#include <ox/ox.hpp>

auto controls = ox::Row{ox::Label{"Name"}, ox::LineEdit{}};
controls.gap = 1;
controls.cross_align = ox::CrossAlign::Center;
```

Rows place children horizontally; columns place them vertically. `gap` separates
visible children. `cross_align` selects `Stretch`, `Start`, `Center`, or `End` on
the other axis. Invisible children and root overlays consume no layout space.

Layouts accept tuples, pairs, arrays, vectors, lists, or other sized forward ranges.
See [widget ownership](core-concepts.md#widgets-and-ownership) for value storage
and borrowed children.

### Size policies

`width_policy` and `height_policy` control each axis independently. Common choices
are `SizePolicy::fixed(cells)`, `min(cells)`, `max(cells)`, and `flex(weight)`.
Single-line controls normally reserve one row; flexible widgets use spare space.
`widget | SizePolicy` sets both axes, so edit individual policies for a one-axis constraint.

Containers combine natural content measurements with those policies. If minimum
sizes cannot fit, children and gaps shrink to the available area. See
[layout calculation](internals/layout.md#allocation-rules) for the allocation algorithm.

## Changing a layout

After editing children, visibility, spacing, alignment, or size policies, call
`refresh()` on the affected layout. Using `controls` from above:

```cpp
controls.gap = 2;
controls.cross_align = ox::CrossAlign::End;
controls.refresh();
```

This arranges children at the current bounds and requests repaint. Refresh a nested
layout when its children change, even if its own bounds remain the same.

## Padding, borders, and dividers

`Padding` owns one child. Its insets are top, right, bottom, left:

```cpp
auto padded = ox::Padding{std::move(controls), std::array{1, 2, 1, 2}};
```

`widget | Border{...}` adds an outline and optional title:

```cpp
#include <ox/ox.hpp>
using namespace ox;

int main()
{
    auto panel = Column{
        Label{"First"},
        Divider{{.label = {.text = "Section"}}},
        Label{"Second"}} | Border{.label = {.text = "Panel"}};
    return Application{panel}.run();
}
```

The bordered wrapper owns the widget as `child` and exposes its decoration as
`border`. Borders reserve one cell on each side; padding reserves its insets.
Small allocations can leave an empty interior. Border titles stay horizontal.

Put a horizontal `Divider` in a `Column`, or set `vertical = true` for one in a
`Row`. Its label follows the line's direction. `Lines::ascii()` uses ASCII strokes.
See [Border](https://a-n-t-h-o-n-y.github.io/TermOx/structox_1_1_border.html) and [Divider](https://a-n-t-h-o-n-y.github.io/TermOx/classox_1_1_divider.html) for decoration options.

## Pages and split panes

`Stack` shows one page at a time:

```cpp
auto pages = ox::Stack{std::move(padded), ox::TextBox{{.text = "Details"}}};
pages.select(1);
```

Use `select(index)` to switch or `select(std::nullopt)` to hide all pages.
Only the selected page participates in
focus, timers, input, and painting. Refresh after editing a dynamic page container.

For pages with a labeled strip, use `Tabs` instead:

```cpp
auto pages = ox::Tabs{
    std::tuple{ox::LineEdit{}, ox::TextBox{}},
    std::vector<std::string>{"Name", "Notes"},
};
```

Ctrl-Tab/Shift-Ctrl-Tab switches pages, including from an editor. When changing the
page container, update labels with `set_labels` and refresh. [Tabs](https://a-n-t-h-o-n-y.github.io/TermOx/classox_1_1_tabs.html) documents
page selection and label requirements.

For two resizable panels, use `SplitPane`:

```cpp
auto panes = ox::SplitPane{ox::TreeView{}, ox::TextBox{}, .3};
```

This starts with 30% of the space assigned to the first panel. Drag the divider or
focus it and use arrows, Home, and End. Set `vertical = true` to stack the panels.
`first_minimum` and `second_minimum` constrain their sizes; when both cannot fit,
available space is shared proportionally. See [SplitPane](https://a-n-t-h-o-n-y.github.io/TermOx/classox_1_1_split_pane.html) for ratio updates.

## Scrolling

`ScrollView` gives a child a clipped, two-axis viewport. Supply `requested_size`
when a custom child needs a specific content extent:

```cpp
auto image = ox::ScrollView{ox::PixelGrid{ox::Area{80, 80}}};
ox::arrange(image, {{}, {20, 10}});
image.set_scroll_offset({10, 5});
```

Use `set_scroll_offset` to move the viewport or `reveal` to bring a rectangle into
view. Text editors, lists, tables, trees, and logs already scroll their own content;
they do not need another ScrollView just to attach a scrollbar.

### Link a scrollbar

```cpp
#include <ox/ox.hpp>

int main()
{
    auto text = ox::TextBox{{.text = "First\nSecond\nThird\nFourth\nFifth\nSixth",
                             .read_only = true}};
    auto vertical = ox::ScrollBar{};
    auto root = ox::Row{std::tie(text, vertical)};
    ox::link(text, vertical);
    return ox::Application{root}.run();
}
```

`link(view, bar)` synchronizes the range and position in both directions, including
content changes and resizing. Each axis needs its own bar. Set `vertical = false`
before linking a horizontal bar; RadioGroup supports only vertical scrolling.

Connect once per axis. The link tracks both widgets across moves and skips destroyed
targets. Dropping the returned identifiers leaves the connections active. Keep them
only if you need to disconnect:

```cpp
auto log = ox::LogView{};
auto bar = ox::ScrollBar{};
auto ids = ox::link(log, bar);
// Later, while both widgets are alive:
log.on_scroll.disconnect(ids.first);
bar.on_scroll.disconnect(ids.second);
```

Scroll positions change immediately even when the thumb is animated. For custom
views, implement the [scrollable interface](custom-widgets.md#scrollable-widgets).
See [ScrollBar](https://a-n-t-h-o-n-y.github.io/TermOx/classox_1_1_scroll_bar.html) for range and thumb behavior.

## Centered content

`Suspended` centers one owned child at a requested size:

```cpp
#include <ox/layout.hpp>

auto panel = ox::Suspended{ox::Widget{}, ox::Area{12, 4}};
ox::arrange(panel, {{}, {20, 10}});
panel.requested_size = {16, 6};
panel.refresh();
```

The requested extent is clamped to the available area. The child's size policy is
not consulted; the wrapper's policy controls the space allocated by its parent.
Call `refresh()` after changing `requested_size`. See [Suspended](https://a-n-t-h-o-n-y.github.io/TermOx/classox_1_1_suspended.html) for fill and
geometry details.

Next, use [Widgets](widgets.md) to choose controls or [Recipes](recipes.md) to combine them.

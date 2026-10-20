# Borders and dividers

Wrap a widget with `Border` for an outline and title. Put `Divider` between
sections of a layout. Both accept a label and a style for the decoration.

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

## Borders

`widget | Border{...}` creates a `Bordered` wrapper that owns the widget. Access it
through `child` and change its outline through `border`. The border reserves one
cell on each side; tiny views can leave an empty interior.

The title sits along the top edge between the corners. Its alignment and style are
independent of the box style. Titles stay horizontal, and their label size policies
are unused. Call `refresh()` after editing public configuration.

## Dividers

Put a horizontal Divider in a Column, or set `vertical = true` for one in a Row.
The default size is one cell across the line; larger views center it on that axis.
Labels follow the divider's direction. For vertical labels, `Align::Start`,
`Center`, and `End` mean top, middle, and bottom. See [Labels](labels.md) for Unicode
fitting. `Lines::ascii()` uses `|` vertically and `-` horizontally.

## Paint decoration text

Custom widgets can use `paint_border` and `paint_divider` to draw decorations
directly. `put_label` draws text without clearing the surrounding area;
`paint_label` clears its view first. These helpers obey the canvas clip and the
[label fitting rules](labels.md#measurement-and-fitting). See the
[API reference](https://a-n-t-h-o-n-y.github.io/TermOx/) for their arguments.

## Related guides

- [Layout and containers](containers.md): ownership, spacing, and changing layouts.
- [Widget presentation](presentation.md): focus, selection, and control indicators.
- [Custom drawing](drawing.md): paint functions and clipped canvases.

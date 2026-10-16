# Examples

Configure the project using [Getting Started](../docs/getting_started.md). Build
an example by its target name, then run the executable from `build/examples/`:

```sh
cmake --build build --target TermOx.examples.datatable
./build/examples/TermOx.examples.datatable
```

Examples are excluded from the default build. Ctrl-C exits interactive programs.
With a multi-configuration generator, build with `--config Release` and run from
`build/examples/Release/` instead.

## Start here

| Program | What it shows | Read alongside it |
| --- | --- | --- |
| [counter.cpp](counter.cpp) | Layout ownership, button activation, a tracked connection | [Getting Started](../docs/getting_started.md#read-the-counter) |
| [datatable.cpp](datatable.cpp) | Sorting, row activation, linked scrolling | [Table walkthrough](datatable.md) |
| [scratch.cpp](scratch.cpp) | Radio selection connected to a text box | [Selection widgets](../docs/selection-and-application.md) |

Targets are `TermOx.examples.counter`, `TermOx.examples.datatable`, and
`TermOx.examples.scratch` respectively.

## More programs

The target for each source below is `TermOx.examples.<filename without .cpp>`.
The linked guides describe the relevant APIs; only the table example above has
a step-by-step application walkthrough.

| Source | Purpose | Related guide |
| --- | --- | --- |
| [accumulator.cpp](accumulator.cpp) | Numeric input and a running total | [Widget API](../docs/widget-api.md) |
| [bmp_view.cpp](bmp_view.cpp) | Load a BMP file and display its pixels; pass the filename as an argument | [PixelGrid reference](../docs/reference.md) |
| [selection.cpp](selection.cpp) | Lists, choices, tabs, menus, and dialogs | [Selection widgets](../docs/selection-and-application.md) |
| [views.cpp](views.cpp) | Values, trees, logs, split panes, forms, and file selection | [Composite widgets](../docs/value-and-composite-widgets.md) |
| [showcase.cpp](showcase.cpp) | An interactive widget tour | [Theme and motion](../docs/theme-and-motion.md#showcase) |
| [polish.cpp](polish.cpp) | Compare optional widget appearance settings | [Presentation](../docs/presentation.md) |
| [stress.cpp](stress.cpp) | Adjustable workloads and timing reports | [Stress Lab](stress.md) |
| [pong.cpp](pong.cpp) | Pong | [Timers and event processing](../docs/event-loop.md) |
| [snake.cpp](snake.cpp) | Snake | [Timers and event processing](../docs/event-loop.md) |
| [pinbox.cpp](pinbox.cpp) | Interactive custom drawing | [Frames and canvases](../docs/frame-and-canvas.md) |

`bmp_reader.hpp` is a helper used by the BMP viewer, not a separate executable.

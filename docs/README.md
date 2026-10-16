# Documentation

Start with [Getting Started](getting_started.md) to build a program, or follow the
[table walkthrough](../examples/datatable.md) to add sorting and scrolling.
The [examples index](../examples/README.md) lists runnable programs.

## Find an interface

The [API reference](reference.md) describes public interfaces and links to their
headers. The topic guides below explain how related interfaces work together.

| Topic | Guide |
| --- | --- |
| Widget state, events, painting, and lifetime | [Widget API](widget-api.md) |
| Padding, stacks, and scroll views | [Container composition](containers.md) |
| Borders, labels, rows, and columns | [Decorations and layouts](decorations-layouts.md) |
| Buttons, checkboxes, and radio groups | [Controls](controls.md) |
| Tables and scrollbars | [Data tables](data-tables.md) |
| Lists, selects, tabs, popups, dialogs, and menus | [Selection and application structure](selection-and-application.md) |
| Values, trees, logs, split panes, forms, files, and command search | [Values and composite widgets](value-and-composite-widgets.md) |
| Shared actions and keyboard shortcuts | [Commands](commands.md) |
| Colors, animation, rules, and keyboard help | [Theme and motion](theme-and-motion.md) |
| Gutters, stripes, indicators, and other appearance options | [Widget presentation](presentation.md) |

## Application and drawing internals

These guides cover custom widgets, event handling, and the rendering pipeline.

| Topic | Guide |
| --- | --- |
| Dispatch, repaint requests, timers, and background work | [Application and event loop](event-loop.md) |
| Event propagation, pointer capture, and focus | [Routing and focus](routing-and-focus.md) |
| Unicode text, selection, history, and text views | [Text editing](text-editing.md) |
| Size allocation and placement | [Layout calculation](layout.md) |
| Frame storage, coordinates, clipping, and drawing | [Frames and canvases](frame-and-canvas.md) |
| Frame comparison and terminal output | [Renderer](renderer.md) |

## Maintenance

[Contributing](contributing.md) covers builds and checks, with links to benchmark
results and the API audit. Migration notes remain in the relevant topic guides;
start with the [widget API migration section](widget-api.md#migration).

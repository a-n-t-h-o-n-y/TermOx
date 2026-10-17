# Documentation

Use [Getting Started](getting_started.md) to build your first program. The
[API reference](reference.md) lists interfaces by topic and links to their headers.
The guides below explain how those interfaces work together.

## Tutorials and examples

| Start with | What it covers |
| --- | --- |
| [Getting Started](getting_started.md) | Build the counter, connect a signal, and use TermOx from CMake |
| [Examples index](../examples/README.md) | Source files, build targets, demos, and games |

## Widgets and layout

Read Widget API for the common contract, then choose a guide for the widgets you
need. Connections and lifetime explains callback ownership in more detail.

| Guide | Contents |
| --- | --- |
| [Widget API](widget-api.md) | State, measurement, events, painting, timers, and lifetime |
| [Connections and lifetime](connections.md) | Signal subscriptions, tracked arguments, and callback ownership |
| [Container composition](containers.md) | Rows, columns, padding, stacks, scroll views, and shared scrolling |
| [Decorations and layouts](decorations-layouts.md) | Borders, dividers, label orientation, and layout refresh |
| [Controls](controls.md) | Buttons, checkboxes, radio groups, and shared input helpers |
| [Data tables](data-tables.md) | Rows, columns, sorting, selection, clipping, and scrollbars |
| [Selection widgets](selection-and-application.md) | Lists, selects, tabs, popups, dialogs, and menus |
| [Values and composite widgets](value-and-composite-widgets.md) | Numeric input, values, trees, logs, split panes, forms, files, and command search |
| [Commands](commands.md) | Shared actions and keyboard shortcuts |

## Appearance

| Guide | Contents |
| --- | --- |
| [Theme and motion](theme-and-motion.md) | Palette roles, overrides, animation, rules, and keyboard help |
| [Widget presentation](presentation.md) | Gutters, stripes, indicators, density, and decoration options |

## Application and drawing

Use these guides for custom widgets, background work, or composing the core
without an Application.

| Guide | Contents |
| --- | --- |
| [Application and event loop](event-loop.md) | Dispatch, repaint, posting background work, and timers |
| [Routing and focus](routing-and-focus.md) | Event propagation, pointer capture, focus, and callback mutations |
| [Text editing](text-editing.md) | Unicode text, selection, history, text layout, and editor composition |
| [Layout calculation](layout.md) | Size allocation and placement |
| [Frames and canvases](frame-and-canvas.md) | Frame storage, coordinates, clipping, and drawing contracts |
| [Renderer](renderer.md) | Frame comparison and terminal output |

## Contributing

- [Contributing](contributing.md): build checks, documentation conventions, and maintenance records.
- [Performance](performance.md): benchmark commands and recorded optimization results.

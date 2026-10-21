# Internals

These pages explain the mechanisms beneath the public widget APIs. For application
recipes, start with the [guides](../README.md); use the generated
[API reference](https://a-n-t-h-o-n-y.github.io/TermOx/) for member contracts.

| Mechanism | What it explains |
| --- | --- |
| [Layout](layout.md) | Allocation, rounding, retained plans, refresh, and callback mutations |
| [Routing and focus](routing-and-focus.md) | Tree snapshots, coordinates, bubbling, modal scopes, capture, and focus recovery |
| [Activation and selection](controls.md) | Shared input helpers, row selection, and row drawing |
| [Event loop](event-loop.md) | Dispatch batches, wakeups, terminal waiting, and custom drivers |
| [Text editing and layout](text.md) | Grapheme boundaries, history, wrapping, viewport functions, and caches |
| [Frames and canvases](frame-and-canvas.md) | Owned cells, wide-grapheme footprints, borrowed views, and clipping |
| [Renderer](renderer.md) | Frame comparison, row revisions, terminal encoding, and failure recovery |

The application arranges and routes through a widget tree, paints clipped canvases
into a frame, and passes that frame to the renderer. Text layout supplies the same
byte-to-cell mapping for painting, caret movement, and hit testing.

## Performance and development

- [Performance](performance.md): benchmark commands, recorded workloads, and optimization results.
- [Contributing](../contributing.md): current build, test, and documentation checks.

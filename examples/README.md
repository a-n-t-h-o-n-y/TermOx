# Examples

Configure the project using [Getting Started](../docs/getting_started.md). Build
an app by its target name, then run the executable from `build/examples/`:

```sh
cmake --build build --target TermOx.examples.pong
./build/examples/TermOx.examples.pong
```

Examples are excluded from the default build. Ctrl-C exits interactive programs.
With a multi-configuration generator, build with `--config Release` and run from
`build/examples/Release/` instead.

The target for each source below is `TermOx.examples.<filename without .cpp>`.
The linked guides describe the relevant APIs.

## Apps

| Source | Purpose | Related guide |
| --- | --- | --- |
| [pong.cpp](pong.cpp) | Pong | [Timers and event processing](../docs/event-loop.md) |
| [snake.cpp](snake.cpp) | Snake | [Timers and event processing](../docs/event-loop.md) |
| [pinbox.cpp](pinbox.cpp) | Interactive custom drawing | [Frames and canvases](../docs/frame-and-canvas.md) |
| [bmp_view.cpp](bmp_view.cpp) | Load a BMP file and display its pixels; pass the filename as an argument | [PixelGrid reference](../docs/reference/drawing.md#oxpixelgrid) |
| [accumulator.cpp](accumulator.cpp) | Numeric input and a running total | [Widget API](../docs/widget-api.md) |

`bmp_reader.hpp` is a helper used by the BMP viewer, not a separate executable.

## Development tools

| Source | Purpose | Related guide |
| --- | --- | --- |
| [showcase.cpp](showcase.cpp) | An interactive widget tour | [Theme and motion](../docs/theme-and-motion.md#showcase) |
| [stress.cpp](stress.cpp) | Adjustable workloads and timing reports | [Stress Lab](stress.md) |

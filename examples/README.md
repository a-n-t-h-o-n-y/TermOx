# Examples

From a configured TermOx checkout ([setup](../README.md#build-and-run-the-showcase)),
build an app by its target name and run it from `build/examples/`:

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
| [pinbox.cpp](pinbox.cpp) | Interactive custom drawing | [Custom drawing](../docs/drawing.md) |
| [bmp_view.cpp](bmp_view.cpp) | Load a BMP file and display its pixels; pass the filename as an argument | [PixelGrid reference](../docs/reference/drawing.md#oxpixelgrid) |
| [accumulator.cpp](accumulator.cpp) | Numeric input and a running total | [Writing a custom widget](../docs/widget-api.md) |

The [Showcase](showcase.md) also includes a [BMP viewer](bmp-viewer.md) with a bundled fractal,
file opening, zoom, panning, and image details. `bmp_reader.hpp` and `bmp_image.hpp`
are shared image helpers, not separate executables.

## Development tools

| Source | Purpose | Related guide |
| --- | --- | --- |
| [showcase.cpp](showcase.cpp) | Forms, data views, text, themes, and a BMP viewer | [Showcase walkthrough](showcase.md) |
| [stress.cpp](stress.cpp) | Adjustable workloads and timing reports | [Stress Lab](stress.md) |

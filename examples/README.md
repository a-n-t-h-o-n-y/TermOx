# Examples

From a configured TermOx checkout ([setup](../README.md#build-and-run-explorer)),
build an app by its target name and run it from `build/examples/`:

```sh
cmake --build build --target TermOx.examples.pong
./build/examples/TermOx.examples.pong
```

Examples are excluded from the default build. Ctrl-C exits interactive programs.
With a multi-configuration generator, build with `--config Release` and run from
`build/examples/Release/` instead.

The target for each source below is `TermOx.examples.<filename without .cpp>`.

| Source | Purpose | Related guide |
| --- | --- | --- |
| [pong.cpp](pong.cpp) | Pong | [Timers](../docs/core-concepts.md#timers) |
| [snake.cpp](snake.cpp) | Snake | [Timers](../docs/core-concepts.md#timers) |
| [pinbox.cpp](pinbox.cpp) | Interactive custom drawing | [Custom drawing](../docs/custom-widgets.md#painting) |
| [accumulator.cpp](accumulator.cpp) | Numeric input and a running total | [Writing a custom widget](../docs/custom-widgets.md) |
| [explorer.cpp](explorer.cpp) | File browser with text and image previews | [Tables and trees](../docs/widgets.md#tables-and-trees) |

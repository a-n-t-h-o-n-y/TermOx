# Contributing

Keep changes to public interfaces, their documentation, and the examples that use
them together.

## Build and test

You need Git, CMake 3.18 or newer, and a C++20 compiler and standard library with
`std::format` and coroutine support. From the repository root:

```sh
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --target TermOx.tests.unit TermOx.tests.explorer
ctest --test-dir build --output-on-failure
```

The first configure downloads pinned dependencies. The unit target also builds
the separate text-allocation test executable. For a multi-configuration generator,
add `--config Release` to builds and `-C Release` to CTest.
See [build troubleshooting](getting_started.md#common-build-and-run-problems) if
your toolchain cannot configure or compile the project.

On macOS or Linux, check interactive input and terminal restoration with:

```sh
cmake --build build --target TermOx.tests.renderer_terminal TermOx.tests.event_loop_terminal \
    TermOx.examples.pong TermOx.examples.pinbox \
    TermOx.examples.snake TermOx.examples.accumulator \
    TermOx.examples.explorer TermOx.tests.explorer
python3 tests/smoke_examples.py build
ctest --test-dir build -R '^TermOx.tests.explorer$' --output-on-failure
python3 tests/smoke_explorer.py build
```

## Documentation changes

Compile changed C++ examples, including the counter in both the README and Getting
Started. Keep those two copies in sync. Build any app whose walkthrough
changes, and check its keyboard and mouse instructions in a terminal.
Use relative links so the documentation works in a checkout and on GitHub.

Images should come from current code. Explorer's `--json` mode exports a painted
frame. Regenerate the README image from a temporary sample directory:

```sh
cmake --build build --target TermOx.examples.explorer
python3 docs/render-explorer.py docs/img/explorer.svg
```

The SVG uses actual frame text, widths, and colors. Check it at README display size.

Build the generated reference using [the Doxygen instructions](doxygen/README.md).
The reference workflow checks comments on pull requests and publishes the HTML
automatically from the repository's default branch.

## Documentation layout

Keep the user guides in seven pages: Getting started, Core concepts, Layout,
Widgets, Appearance, Recipes, and Custom widgets. The [index](README.md) gives
new readers a starting order. Add sections to these guides before creating another
page. Keep the root README brief.

Guides explain tasks and concepts with useful examples. Preserve ownership, thread,
validation, and callback details where they affect correct use. Keep exhaustive
member descriptions, defaults, and edge cases beside public declarations in
`include/ox/`, published in the [API reference](https://a-n-t-h-o-n-y.github.io/TermOx/).
Check that a contract is documented there before removing its only explanation.

Explain each topic in one place and link to it elsewhere. When moving a section,
update incoming links and check anchors as well as file paths. Use default checkout
commands and links that remain valid after a merge. App walkthroughs stay beside
their sources in `examples/`.

## Implementation and performance

- [Internals](internals/README.md): layout, event routing, text storage, frames, and rendering.
- [Performance](internals/performance.md): benchmark commands, environments, and recorded measurements.
- [Stress Lab](../examples/stress.md): adjustable workloads and headless timing.

Historical measurements describe the recorded runs. Use the current build and test
commands above when checking a change.

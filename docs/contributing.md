# Contributing

Keep changes to public interfaces, their documentation, and the examples that use
them together.

## Build and test

You need Git, CMake 3.18 or newer, and a C++20 compiler and standard library with
`std::format` and coroutine support. From the repository root:

```sh
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --target TermOx.tests.unit
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
    TermOx.examples.bmp_view
python3 tests/smoke_examples.py build
```

## Documentation changes

Compile changed C++ examples, including the counter in both the README and Getting
Started. Keep those two copies in sync. Build any app whose walkthrough
changes, and check its keyboard and mouse instructions in a terminal.
Use relative links so the documentation works in a checkout and on GitHub.

Images should come from current code. The Showcase's `--json` mode exports a
painted frame with motion disabled. Regenerate the README image and BMP viewer image
from the repository root:

```sh
cmake --build build --target TermOx.examples.showcase
python3 docs/render-showcase.py docs/img/showcase.svg
python3 docs/render-showcase.py docs/img/bmp-viewer.svg --page 4 --title "BMP viewer in TermOx Showcase"
```

These SVGs use actual frame text, widths, and colors; no terminal capture or image
package is needed. Check the result at README display size after regenerating it.

Build the generated reference using [the Doxygen instructions](doxygen/README.md).
The reference workflow checks comments on pull requests and publishes the HTML
automatically from the repository's default branch.

## Documentation layout

Public member contracts live in Doxygen comments in `include/ox/` and appear in the
generated [API reference](https://a-n-t-h-o-n-y.github.io/TermOx/). Keep signatures,
defaults, and member contracts there; guides explain how to use related interfaces.
The root README contains the small inline program and build entry points.
Getting Started must remain a complete first-project path.

Start guides with a small example and common operations. Explain ownership,
thread restrictions, validation, and callback lifetimes where they affect use.
Link to existing explanations instead of repeating them, and reserve tables of
contents for long pages.

Practical guides live directly under `docs/`; implementation mechanisms and measured
performance live under `docs/internals/`. App walkthroughs live beside their sources
in `examples/`. Each rule should have one primary explanation, linked from other
pages. In particular:

| Topic | Primary page |
| --- | --- |
| Layout composition and refresh use | [Layout and containers](containers.md) |
| Allocation and callback mechanics | [Layout internals](internals/layout.md) |
| Borders and dividers | [Decorations](decorations.md) |
| Label orientation and fitting | [Labels](labels.md) |
| Focus/selection patches and control density | [Widget presentation](presentation.md) |
| Scroll links and thumb behavior | [Scrolling](scrolling.md) |
| Form commit and validation order | [Forms](forms.md) |
| Worker lifetime and posting UI updates | [Background work](background-work.md) |

Add guides to the [documentation index](README.md), update incoming links when moving
a page, and check anchors as well as file paths. Use default checkout commands
and links that remain valid after a merge.

## Background material

- [Performance](internals/performance.md): benchmark commands, recorded environments, and
  measurements from optimization work.
- [API audit record](internals/api-audit.md): validation results recorded during the drawing,
  layout, and connection audit.
- [Stress Lab](../examples/stress.md): adjustable workloads and headless timing.

Validation counts in historical notes describe those runs, not the current test
suite. Use the commands above to check your checkout.

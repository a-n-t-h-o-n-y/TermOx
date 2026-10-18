# Contributing

Build the project using [Getting Started](getting_started.md). Keep changes to
public interfaces, their documentation, and the examples that use them together.

## Run the tests

From the repository root:

```sh
cmake --build build --target TermOx.tests.unit
ctest --test-dir build --output-on-failure
```

The unit target also builds the separate text-allocation test executable.

On macOS or Linux, check interactive input and terminal restoration with:

```sh
cmake --build build --target TermOx.tests.renderer_terminal TermOx.tests.event_loop_terminal \
    TermOx.examples.pong TermOx.examples.pinbox \
    TermOx.examples.snake TermOx.examples.accumulator \
    TermOx.examples.bmp_view
python3 tests/smoke_examples.py build
```

## Documentation changes

Compile the README code block when changing it. Build any app whose walkthrough
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
automatically from the configured publishing branch.

## Documentation layout

Public member contracts live in Doxygen comments in `include/ox/`. The generated
reference is the method-level source; `reference.md` and `reference/` keep topic
summaries and guide links. The root README contains the small inline program and
build entry points. Getting Started must remain a complete first-project path.

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
a page, and check anchors as well as file paths. Keep the branch in the README,
Getting Started clone command, and Doxygen publishing instructions consistent.

## Background material

- [Performance](internals/performance.md): benchmark commands, recorded environments, and
  measurements from optimization work.
- [API audit record](internals/api-audit.md): validation results recorded during the drawing,
  layout, and connection audit.
- [Stress Lab](../examples/stress.md): adjustable workloads and headless timing.

Validation counts in historical notes describe those runs, not the current test
suite. Use the commands above to check your checkout.

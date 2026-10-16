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
    TermOx.examples.snake TermOx.examples.accumulator TermOx.examples.scratch
python3 tests/smoke_examples.py build
```

## Documentation changes

Keep the README example identical to [counter.cpp](../examples/counter.cpp) so it
can be compiled as part of the normal examples build. Build any example whose
walkthrough changes, and check its keyboard and mouse instructions in a terminal.
Use relative links so the documentation works in a checkout and on GitHub.

Screenshots should come from the current code. The README's table image is a
rendering of the example's headless frame, with a dark background and monospace
font. Older images remain in `docs/img/` for reference; they are not used as
version-three screenshots.

## Background material

- [Performance](performance.md): benchmark commands, recorded environments, and
  measurements from optimization work.
- [API audit](api-audit.md): drawing, layout, and connection conventions, with
  migration notes and validation results from that audit.
- [Stress Lab](../examples/stress.md): adjustable workloads and headless timing.

Validation counts in historical notes describe those runs, not the current test
suite. Use the commands above to check your checkout.

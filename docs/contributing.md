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

Screenshots should come from the current code. The README's table image is a
rendering of a painted DataTable frame, with a dark background and monospace
font. Other images are stored in `docs/img/`.

Build the generated reference using [the Doxygen instructions](doxygen/README.md).
The reference workflow checks comments on pull requests and publishes the HTML
automatically from the configured publishing branch.

## Documentation layout

`reference.md` indexes the interface entries in `reference/`. Topic guides explain
contracts and composition; walkthroughs live beside their example source in
`examples/`. Add an entry to the [guides index](README.md) when adding a guide.

## Background material

- [Performance](performance.md): benchmark commands, recorded environments, and
  measurements from optimization work.
- [API audit record](api-audit.md): validation results recorded during the drawing,
  layout, and connection audit.
- [Stress Lab](../examples/stress.md): adjustable workloads and headless timing.

Validation counts in historical notes describe those runs, not the current test
suite. Use the commands above to check your checkout.

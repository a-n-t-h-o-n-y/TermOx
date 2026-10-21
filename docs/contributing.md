# Contributing

## Build and test

From the repository root, using the [required toolchain](../README.md#build-and-run-explorer):

```sh
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --target TermOx.tests.unit TermOx.tests.explorer
ctest --test-dir build --output-on-failure
```

For multi-configuration generators, add `--config Release` to builds and `-C Release`
to CTest. See [troubleshooting](getting_started.md#common-build-and-run-problems) for setup issues.

For input or terminal changes, also run the POSIX terminal checks:

```sh
cmake --build build --target TermOx.tests.renderer_terminal TermOx.tests.event_loop_terminal \
    TermOx.examples.pong TermOx.examples.pinbox TermOx.examples.snake \
    TermOx.examples.accumulator TermOx.examples.explorer
python3 tests/smoke_examples.py build
python3 tests/smoke_explorer.py build
```

## Making changes

Update relevant tests, examples, and documentation alongside the code. Document
public API behavior beside its declarations in `include/ox/`; use the guides for
concepts and examples. Link to existing explanations instead of repeating them.

Compile changed C++ snippets and check documentation links. The counter appears in
both the README and Getting Started; keep those copies in sync.

To refresh the README image after changing Explorer:

```sh
cmake --build build --target TermOx.examples.explorer
python3 docs/render-explorer.py docs/img/explorer.svg
```

## Useful links

- [API reference build](doxygen/README.md)
- [Internals](internals/README.md)
- [Performance benchmarks](internals/performance.md)

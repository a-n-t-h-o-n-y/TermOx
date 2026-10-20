# Stress Lab

Build in **Release**:

```sh
cmake -S . -B build-release -DCMAKE_BUILD_TYPE=Release
cmake --build build-release --target TermOx.examples.stress
./build-release/examples/TermOx.examples.stress
```

Defaults: 10,000 records per data view, 1,000 buttons / signal listeners / timers / animations,
a 2048×2048 RGB image, eight units of work per tick, and a 33 ms target interval.
Datasets are generated once at startup. No files are written or external services used.

| Page | Work being stressed |
| --- | --- |
| Logs | Bounded Unicode batches, eviction, tail following, retained text analysis and layout measurement |
| Tree | Large sibling sets, selection/path lookup, a collapse/reopen cycle every 30 ticks; expand individual branches manually |
| Table | Large row set, cell updates, selection/scrolling, stable sorting every 60 ticks |
| Search | Decoded search cache, equivalent matches, changed rankings, empty results and list cache reuse |
| Text | Very wide Unicode line plus a large multiline document, insert/undo, byte-to-cell lookup, wrapping and selection/paste when paused |
| Pixels | Large owned image with a moving viewport, clipped native pixel painting and incremental pixel mutation |
| Layout | Nested rows of buttons, geometry changes, focus traversal, scrolling/clipping and activation signals |
| Frames | Unchanged content, one moving cell, full color changes, scrolling wide/combining/emoji text |
| Dispatch | Signal fanout, posted event bursts and many tracked timers firing on a one-second cadence and shared-clock animations |

Press **Alt-1 … Alt-9** to switch pages; **Ctrl-Tab** also advances. **Tab** moves
focus normally, and mouse/keyboard navigation remains available inside the widgets.

- **Alt-P:** pause/resume generated work; pause to search, edit, select or scroll manually.
- **Alt-+** (or **Alt-=**) / **Alt--:** double/halve batch size, from 1 to 1,024.
- **Alt-A:** run all workloads, including hidden pages, or just the visible workload. All mode updates every model and paints the active page.
- **Alt-B:** execute one burst of all workloads, even while paused.
- **Alt-M:** cycle the four frame modes.
- **Alt-W:** toggle the text editor between no wrapping and word wrapping.
- **Ctrl-C:** exit and restore the terminal.

The footer shows the visible workload’s CPU time (last, mean and peak), observed timer gap, target
interval and late tick count. A gap exceeding 1.5× the interval counts as late. Work
CPU time excludes subsequent application synchronization, painting, encoding and
terminal I/O; timer gaps reflect those costs too. These are not FPS measurements.
The batch setting scales log records, tree selections, searches, text edit cycles and
signal/event bursts; table/layout actions remain one operation per tick, while pixel
viewport stride increases. Timer and animation counts follow `--widgets`, not batch size.

For heavier runs, increase dimensions and frequency separately so the bottleneck is
clear. The larger settings deliberately consume more memory and may stall a tick.

```sh
# Larger models, faster updates; datasets remain inspectable when paused.
./build-release/examples/TermOx.examples.stress --size 100000 --batch 32 --interval 16
# All workloads, more widget traversal/signal slots/timers, and a larger image.
./build-release/examples/TermOx.examples.stress --all --widgets 4000 --pixels 4096
```

`--size` accepts 100–1,000,000, `--widgets` 10–10,000, `--pixels` 16–8,192,
`--interval` 1–1,000 ms, and `--page` 1–9. Use `--help` for all options. Changing
batch size live changes work rate; changing model/image/widget sizes requires a restart.
A million tree branches also own two leaves each; the configured size is not a byte
or total-memory limit. List layout caches retain their largest previous size.

## Headless measurements

```sh
./build-release/examples/TermOx.examples.stress --benchmark --ticks 60 > stress.csv
./build-release/examples/TermOx.examples.stress --benchmark --all --size 100000 --ticks 20
./build-release/examples/TermOx.examples.stress --check
python3 tests/smoke_stress.py build-release
```

The CSV covers nine pages, with four separate frame modes (twelve scenarios total),
at 120×38 cells. Two warmup ticks precede the measured samples. Columns report mean
microseconds for workload update, application step (queued events/timers/layout/focus),
painting, and frame comparison/encoding, plus output bytes per frame. The cached
encoding columns measure rendering the exact same frame again and require zero bytes.
`--all` includes every job in each scenario's update time. Timers use advancing virtual
time in these headless runs; timings contain CPU work, not sleeps. One-second timer
firings need enough ticks/interval to fall within the sample window.

There is **no terminal output or terminal-emulator cost** in these timings. Live
performance also depends on terminal size, color support, font and emulator. Headless
samples are diagnostic, not statistically controlled before/after speedup claims;
use the [recorded benchmark workflow](../docs/internals/performance.md) for those comparisons.

`--check` exercises all pages, all frame modes, zero/tiny/ordinary resizes, streaming
capacity, text undo, queued event delivery, timers/animations, button activation and repeated
unchanged frame encoding. The Python smoke additionally checks live input, controls,
resizing and terminal restoration, including the default workload size. These exercise
core hot paths; external filesystem throughput, terminal escape parsing and every
individual small control are covered elsewhere rather than duplicated here.

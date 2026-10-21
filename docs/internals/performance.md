# Performance benchmarks

Use the headless benchmarks to measure library work without terminal I/O.

## Build and run

Run these commands from the repository root:

```sh
cmake -S . -B build-release -DCMAKE_BUILD_TYPE=Release
cmake --build build-release --target TermOx.tests.benchmarks -j 4
./build-release/tests/TermOx.tests.benchmarks
```

With no arguments, the executable runs text, routing, renderer, paint, and
application workloads. Pass one flag to select a suite:

| Flag | Measures |
| --- | --- |
| `--routing` | Event routing and focus in widget trees |
| `--editing` | Document edits, selection, paste, and undo/redo |
| `--layout` | Text wrapping, resizing, tabs, and cached layout |
| `--renderer` | Frame comparison and terminal encoding |
| `--paint` | Frame clearing and widget painting |
| `--application` | Complete headless application frames |
| `--table` | DataTable updates and viewport scaling |
| `--views` | LogView streaming, TreeView changes, and CommandPalette searches |

For example, save text layout results as CSV:

```sh
./build-release/tests/TermOx.tests.benchmarks --layout > layout.csv
```

Container arrangement and grid painting have separate executables:

```sh
cmake --build build-release --target \
    TermOx.tests.arrangement_benchmarks TermOx.tests.pixel_benchmarks -j 4
./build-release/tests/TermOx.tests.arrangement_benchmarks
./build-release/tests/TermOx.tests.pixel_benchmarks
```

Exact workloads and input sizes are in the benchmark sources:
[general](../../tests/benchmarks.cpp),
[arrangement](../../tests/arrangement_benchmarks.cpp), and
[pixels](../../tests/pixel_benchmarks.cpp).

## Compare revisions

Build both library revisions with the same benchmark source, compiler, flags, and
dependencies. Finish both builds before measuring, and run on an idle machine.
Changing the driver or workload between revisions makes the comparison unreliable.

```sh
python3 tests/benchmark_paint_pipeline.py /path/to/before-benchmarks \
    ./build-release/tests/TermOx.tests.benchmarks /tmp/benchmark-results
```

The default suites are `renderer`, `paint`, and `application`. Use `--modes` to
select from those or `layout`, `table`, and `views`:

```sh
python3 tests/benchmark_paint_pipeline.py /path/to/before-benchmarks \
    ./build-release/tests/TermOx.tests.benchmarks /tmp/layout-results \
    --modes layout application
```

For the standalone executables, pass the corresponding before/after binaries and
use `--modes arrangement` or `--modes pixels`. The script does not support the
routing or editing suites; run those directly for each revision.

Each suite runs in three fresh processes per revision, alternating execution
order. The script checks that workload names, iteration counts, and checksums
match across every run, then writes:

- `paint-pipeline-samples.csv`: all process samples.
- `paint-pipeline-<mode>-before.csv` and `paint-pipeline-<mode>-after.csv`:
  medians of the process results.

## Read the results

The general and arrangement benchmarks use ten warmup operations followed by five
timed samples. Their CSV columns are:

| Column | Meaning |
| --- | --- |
| `workload` | Operation and input size or scenario |
| `iterations` | Operations per timed sample |
| `median_us_per_operation` | Median of the five samples, in microseconds per operation |
| `checksum` | Accumulated result across timed samples; should match between revisions |
| `allocations_per_operation` | Ordinary C++ allocation calls per operation, averaged across samples |

Allocation counts measure calls, not bytes, and exclude aligned allocations.
Edits and frame mutations are included when they are part of the workload.
Renderer timings include encoding but no writes to a terminal or emulator.

The grid benchmark measures clipped PixelGrid images and animated DotGrid/PixelGrid
frames, using five samples of 20 paints. Clipped-image timings include a checksum
scan; animated grids prepare their data and scan checksums outside the timed section.
Neither includes allocation counts, image decoding, or resampling.

Small timing changes may be noise. These results describe the measured workloads;
interactive performance also depends on terminal size, font, emulator, and I/O.
Previous CSV measurements remain in [benchmark-results](../../tests/benchmark-results/).

## Measure event-loop waiting

This separate probe requires a POSIX PTY:

```sh
cmake --build build-release --target TermOx.tests.event_loop_terminal -j 4
python3 tests/benchmark_event_loop.py \
    ./build-release/tests/TermOx.tests.event_loop_terminal
```

It compares polling and readiness waiting in an 80×24 PTY, with five alternating
samples per mode. CSV output reports idle iterations per second and posted-event
dispatch latency in microseconds; medians go to stderr. It also checks terminal
mode restoration. Latency depends on OS scheduling and does not measure frame rate.

## Phase at large sizes

```sh
cmake --build build-release --target TermOx.examples.phase -j 4
./build-release/examples/TermOx.examples.phase --benchmark --scene 0 \
    --width 480 --height 160 --frames 120
```

Scenes 0–3 are Orbit, Bloom, Loom, and Flock. After 30 warmup frames, CSV output
reports average simulation, painting, and encoding time in milliseconds, plus
output bytes per frame. Terminal I/O is excluded; large animated canvases can be
limited by the emulator and output bandwidth.

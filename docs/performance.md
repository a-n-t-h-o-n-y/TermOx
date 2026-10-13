# Headless performance benchmarks

Build and run the benchmark target explicitly using a Release configuration and the
same C++20 toolchain used for the library:

```sh
cmake -S . -B build-release -DCMAKE_BUILD_TYPE=Release
cmake --build build-release --target TermOx.tests.benchmarks -j 4
./build-release/tests/TermOx.tests.benchmarks
```

The executable prints CSV: workload, iterations per sample, median microseconds per
operation, and a checksum over all five timed samples. Each workload first runs ten
warmup operations. Construction and initial layout/rendering are outside the timer;
the insert/erase and frame mutation workloads include their mutations. There is no
terminal I/O. Checksums keep the results observable and should agree across versions.
Run on an idle machine with identical compiler flags and dependencies. Do not run
builds concurrently with measurements. These are local workload timings, not claims
about interactive frame rates or terminal latency.

## Measured optimization pass

Recorded October 13, 2026 on macOS x86_64 with Clang 22.1.2, libc++ 22, and Release
`-O3 -DNDEBUG`, without LTO or sanitizers. The baseline uses the text and application
implementation from `e16f976`; both executables use the same benchmark source and
unchanged dependencies and library components. All workload checksums match.
Raw results: [before](../tests/benchmark-results/before.csv) and
[after](../tests/benchmark-results/after.csv).

TextLayout now records byte-boundary coordinates during layout construction and uses
binary searches for caret and hit testing. Coordinate storage adds `sizeof(Point)`
per grapheme boundary. The layout pass overwrites coordinates when word wrapping
backtracks, and preserves newline and soft-wrap caret semantics. Unchanged updates
still compare the owned text snapshot; editing still resegments the complete document.

Application synchronization still snapshots the widget tree to detect direct mutations
and preserve callback safety. It now skips attachment reconciliation when the ordered
membership is unchanged, removing quadratic membership searches from stable event
handling. Changes to geometry, visibility, enabled state, focus, and late-created timers
continue through the existing synchronization and registration paths. Reconciliation
for changed membership still uses repeated linear searches.

Renderer and editing are measured controls; their implementation is unchanged in this
pass. Small differences in those timings are run-to-run noise.

| Workload | Before µs | After µs |
| --- | ---: | ---: |
| text.position.1000-lines | 0.932 | 0.054 |
| text.offset.1000-lines | 0.548 | 0.083 |
| text.position.10000-columns | 168.852 | 0.040 |
| text.offset.10000-columns | 164.814 | 0.106 |
| text.layout.rebuild | 2913.667 | 2693.582 |
| text.layout.unchanged | 2.126 | 2.077 |
| text.edit.insert-erase | 847.310 | 837.663 |
| routing.idle.100 | 49.423 | 39.291 |
| routing.key.100 | 75.537 | 60.502 |
| routing.idle.1000 | 748.368 | 384.091 |
| routing.key.1000 | 1131.019 | 595.319 |
| renderer.unchanged.80x24 | 15.896 | 15.805 |
| renderer.single-cell.80x24 | 16.144 | 16.244 |
| renderer.full-frame.80x24 | 114.100 | 115.010 |
| renderer.unchanged.200x60 | 98.391 | 98.428 |
| renderer.single-cell.200x60 | 98.760 | 99.242 |
| renderer.full-frame.200x60 | 708.064 | 711.950 |

The text document is 1,000 repeated multilingual lines with combining marks, a wide
character, and tabs, wrapped to 40 cells. The wide-line workloads use 10,000 ASCII
columns with distributed queries. Layout rebuild alternates between widths 40 and 41;
unchanged updates keep width 41. Editing repeatedly inserts and erases at the midpoint
with bounded history enabled. Routing uses a flat tree with 100 or 1,000 focusable
children and measures idle steps and an unhandled key that bubbles to the root.
Rendering compares 80×24 and 200×60 frames: unchanged, one alternating cell, and
alternating bold/default styles across the entire frame.

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

## Initial optimization pass

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

At this milestone, application synchronization still snapshots the widget tree to detect direct mutations
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

## Routing and focus optimization pass

The routing-only command runs the expanded workload set:

```sh
./build-release/tests/TermOx.tests.benchmarks --routing
```

This pass compares `244379a` against the optimized routing code, with the same
expanded benchmark source, compiler, flags, and dependencies described above.
The baseline executable was saved before changing the library. Every workload checksum
matches. Raw results: [routing before](../tests/benchmark-results/routing-before.csv)
and [routing after](../tests/benchmark-results/routing-after.csv).

| Workload | Before µs | After µs |
| --- | ---: | ---: |
| routing.idle.100 | 39.713 | 11.700 |
| routing.key.100 | 61.099 | 18.593 |
| routing.mouse.100 | 64.241 | 16.108 |
| routing.tab.100 | 53.351 | 22.186 |
| routing.paste.100 | 90.929 | 19.243 |
| routing.reorder.100 | 45.928 | 29.097 |
| routing.idle.1000 | 379.557 | 99.406 |
| routing.key.1000 | 591.375 | 150.931 |
| routing.mouse.1000 | 609.100 | 150.825 |
| routing.tab.1000 | 501.263 | 158.194 |
| routing.paste.1000 | 885.463 | 177.300 |
| routing.reorder.1000 | 572.573 | 237.087 |
| routing.key.depth32 | 7266.523 | 2051.684 |

Mouse movement repeatedly hits the last child. Tab cycles through visible candidates;
paste sends a complete unhandled chunk that bubbles to the root. Reordering rotates
children once per operation and includes synchronization. The deep key workload bubbles
through 32 nested containers plus the leaf and root, alongside 1,000 unrelated siblings.
All routing workloads use 100 operations per sample after ten warmups.

Traversal and attachment buffers retain capacity between synchronizations. A default
leaf widget returns an empty generator without allocating a coroutine. Stable trees
skip attachment work; changed membership uses a sorted vector of live widget pointers
and binary searches instead of repeated full membership scans. Event delivery retains
only the original ancestry and its bounds. Saved indices avoid searching for recipients
when their positions in the tree remain unchanged; reordered recipients are located in
the current tree. Hover paths allocate only when the path changes. Focus navigation
uses a filtered range instead of allocating a candidate list, and focus transitions
reuse their traversal buffer after outgoing callbacks.

Public widget mutations are still detected by rebuilding the tree around callbacks.
Tree traversal remains linear, and a deep unhandled event still synchronizes after each
receiver. Painting still retains a full snapshot because it visits the entire tree.
The additional reusable buffers retain memory proportional to the largest traversed
tree. The public `is_interactive()` query still creates a fresh snapshot, including
when timer eligibility or entering hover callbacks require it.

Validation includes the full unit suite and pseudo-terminal smoke checks, plus 21
non-exception routing cases under ASan/UBSan. The sanitizer run instruments all TermOx
sources and the routing tests; Escape uses the normal Release build. Exception cases
run in the normal unit suite
because the installed sanitizer runtime cannot unwind C++ exceptions.

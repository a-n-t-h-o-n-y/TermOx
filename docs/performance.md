# Headless performance benchmarks

Build and run the benchmark target explicitly using a Release configuration and the
same C++20 toolchain used for the library:

```sh
cmake -S . -B build-release -DCMAKE_BUILD_TYPE=Release
cmake --build build-release --target TermOx.tests.benchmarks -j 4
./build-release/tests/TermOx.tests.benchmarks
```

The executable prints CSV: workload, iterations per sample, median microseconds per
operation, a checksum over all five timed samples, and ordinary C++ allocations per
operation. Older recorded snapshots omit the allocation column. Each workload first runs ten
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
still compare the owned text snapshot; at this milestone editing still resegments the
complete document.

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

## Text editing optimization pass

Run the editing-only workloads with:

```sh
./build-release/tests/TermOx.tests.benchmarks --editing
```

This pass compares `9e11170` against the optimized editing core using the same expanded
benchmark executable source, compiler flags, and dependencies. The baseline executable
was saved before changing the library. The benchmark replaces ordinary scalar and
array `operator new`/`delete` to count successful C++ allocations during each timed
sample; aligned allocation and external `malloc` calls are not counted. This counting
is confined to the single-threaded benchmark executable. Both versions include it,
and setup, warmups, and measurement storage are excluded from the counts.

Every workload checksum matches. Raw results: [editing before](../tests/benchmark-results/editing-before.csv)
and [editing after](../tests/benchmark-results/editing-after.csv).

| Workload | Before µs | After µs | Before allocations/op | After allocations/op |
| --- | ---: | ---: | ---: | ---: |
| editing.insert-erase.1000.begin | 854.575 | 64.145 | 36.004 | 0.008 |
| editing.insert-erase.1000.middle | 889.208 | 48.693 | 36.004 | 0.008 |
| editing.insert-erase.1000.end | 924.778 | 30.593 | 36.004 | 0.008 |
| editing.undo-redo.1000 | 859.363 | 47.661 | 36.000 | 0.000 |
| editing.selection.1000 | 829.675 | 49.257 | 40.000 | 3.000 |
| editing.paste.1000 | 832.359 | 47.480 | 39.000 | 2.000 |
| editing.insert-erase.10000.begin | 11611.603 | 1007.720 | 44.016 | 0.032 |
| editing.insert-erase.10000.middle | 11476.197 | 861.094 | 44.016 | 0.032 |
| editing.insert-erase.10000.end | 11445.935 | 773.473 | 44.016 | 0.032 |
| editing.undo-redo.10000 | 11478.366 | 855.100 | 44.000 | 0.000 |
| editing.selection.10000 | 10991.603 | 862.280 | 48.000 | 3.000 |
| editing.paste.10000 | 10882.461 | 857.137 | 47.000 | 2.000 |
| editing.insert-erase.single-line | 118.780 | 107.334 | 32.004 | 0.008 |
| editing.insert-erase.no-history | 116.699 | 107.412 | 32.000 | 0.000 |

Documents contain 1,000 or 10,000 repeated multilingual lines, using the same sentence
as the initial text benchmark. Insert/erase runs at the beginning, midpoint, and end;
undo/redo replays one midpoint insertion. Selection replaces approximately ten lines
and undoes it, restoring the original selection. Paste inserts a two-chunk stream
with a split UTF-8 emoji and a newline, then undoes it. Single-line and no-history
workloads use 10,000 ASCII characters and edit at the midpoint. Each operation is an
edit pair. Samples contain 100 pairs for 1,000-line and single-line workloads, or 25
pairs for 10,000-line workloads, after ten warmups. Small nonzero history allocation
rates reflect deque growth and block allocation averaged over the samples.

TextEdit now keeps reusable working text and boundary buffers. It resegments from the
previous newline through the next surviving newline after the edit, including any
lines joined by deletion. LF always resets Unicode grapheme context, so arbitrarily
long regional-indicator, combining, Indic, prepend, and emoji sequences within those
lines remain correct. Prefix boundaries are retained and suffix boundaries are shifted
using a bulk transform. Undo and redo use the same preparation path, including byte
ranges that become internal to a joined grapheme. Identical-byte replacements update
selection/history without rebuilding text or boundaries. Bounded history uses deques,
so dropping the oldest delta does not shift every remaining entry. With history disabled,
removed text is not copied into an unused delta.

Working buffers and history allocation complete before the live document is swapped
and selection is committed. An isolated test executable injects allocation failures
into replacement, undo, and redo and verifies unchanged text, boundaries, selection,
and both history stacks. Building the regular unit target also builds this executable;
CTest runs both. The complete suite contains 130 regular cases and two allocation-fault
cases. Thirty-one non-exception text and widget cases also run under ASan/UBSan with
all TermOx sources instrumented; Escape remains a Release library. Exception and fault
cases run in the normal suite because the installed sanitizer runtime cannot unwind
C++ exceptions. All six pseudo-terminal smoke checks pass.

The editor still copies the text and reconstructs the boundary vector for atomicity;
its total work remains linear in document size, while Unicode segmentation is limited
to the affected logical lines. A document with no newline requires full-line
segmentation. Working buffers retain capacity proportional to the largest edited
document. TextLayout construction is a separate cost and is unchanged by this pass.

## Text layout construction optimization pass

Run the layout-only workloads with:

```sh
./build-release/tests/TermOx.tests.benchmarks --layout
```

This pass compares `64b8719` against the optimized layout code using the same expanded
benchmark source, Release toolchain, flags, and dependencies. The baseline executable
was saved before changing the library. Every workload checksum matches. Raw results:
[layout before](../tests/benchmark-results/layout-before.csv) and
[layout after](../tests/benchmark-results/layout-after.csv).

| Workload | Before µs | After µs | Before allocations/op | After allocations/op |
| --- | ---: | ---: | ---: | ---: |
| layout.resize.1000 | 2632.727 | 251.381 | 50.000 | 0.000 |
| layout.wrap.1000 | 2624.492 | 244.414 | 50.000 | 0.000 |
| layout.tabs.1000 | 2619.607 | 243.763 | 50.000 | 0.000 |
| layout.policy.1000 | 2615.099 | 1582.780 | 50.000 | 0.000 |
| layout.text-change.1000 | 2616.962 | 2038.940 | 50.000 | 0.000 |
| layout.unchanged.1000 | 2.061 | 2.157 | 0.000 | 0.000 |
| layout.cold.1000 | 2611.226 | 2062.524 | 53.000 | 35.000 |
| layout.rows-only.1000 | 2412.984 | 2020.342 | 15.000 | 30.000 |
| layout.resize.10000 | 30803.959 | 2638.639 | 62.000 | 0.000 |
| layout.wrap.10000 | 30776.412 | 2618.416 | 62.000 | 0.000 |
| layout.tabs.10000 | 30698.565 | 2628.595 | 62.000 | 0.000 |
| layout.policy.10000 | 30712.547 | 16173.268 | 62.000 | 0.000 |
| layout.text-change.10000 | 30467.216 | 20695.495 | 62.000 | 0.000 |
| layout.unchanged.10000 | 23.663 | 23.600 | 0.000 | 0.000 |
| layout.cold.10000 | 30449.028 | 23114.771 | 65.000 | 43.000 |
| layout.rows-only.10000 | 24738.077 | 21979.658 | 19.000 | 38.000 |

The same multilingual documents contain 1,000 or 10,000 lines. Resize alternates
widths 40/41; wrapping alternates Word/Any at width 40; tabs alternate stops 4/8;
policy alternates ambiguous width One/Two. Text change alternates the first ASCII
character while retaining its width. Cold construction creates and destroys a fresh
TextLayout each operation. Rows-only calls the pure `layout_text()` function. Each
workload includes a row checksum scan and final caret query, except unchanged updates
and rows-only. Samples contain 50 operations for 1,000 lines and ten for 10,000 lines;
unchanged updates use ten times those counts. Every workload has ten warmups.

The implementation separates UTF-8/grapheme analysis, width measurement, and row
wrapping. Segmentation validates the complete UTF-8 input, preserving global error
offsets; a byte scan then rejects terminal controls without constructing decoded or
normalized strings. Original CR and CRLF byte offsets remain intact. Measured columns
are cached per grapheme. Tabs and line breaks have explicit markers; tab widths are
computed from the current row column, so changes in wrapping and tab stops remain
correct. Word wrapping recognizes complete ASCII space/tab clusters as before.

Geometry and wrapping changes reuse boundaries and measured widths. Policy changes
remeasure widths while retaining segmentation. Text changes redo both analysis stages.
Text, boundaries, measured columns, rows, coordinates, and override storage reuse
capacity. All preparation and allocation completes before swapping live state, so
failed updates preserve text, row storage, mapping, and the owned policy. Copies and
moves own their cached analysis independently.

The regular suite contains 132 cases plus three isolated allocation-fault cases.
All six terminal smoke checks pass. A differential run compared 5,400 combinations of
random Unicode text, widths, wrap modes, tab stops, and policies against the previous
implementation: accepted rows, caret positions, mouse hits, pure layout results, and
rejections all matched. Thirty-two non-exception text/widget cases also pass ASan/UBSan
with all TermOx sources instrumented and Escape linked as a Release library. Exception
and allocation-fault cases run in the normal suite because the installed sanitizer
runtime cannot unwind C++ exceptions.

The largest gains apply when text and policy stay unchanged. Full text changes still
require full segmentation and width measurement; cold layouts still allocate their
metadata. The pure rows-only API constructs temporary analysis and performs more
allocations than the previous streaming implementation, though it is faster in these
workloads. Cached layout keeps one extra integer per grapheme plus reusable working
buffers, with capacity proportional to the largest layout. Unchanged updates still
compare the owned text snapshot, and row construction still visits the full document.

## Idle waiting and row revisions

This pass replaces the 16 ms terminal polling limit with readiness waiting. The host
waits on Escape's borrowed input source and an event-loop wakeup source, bounded by
the earliest loop or Escape deadline. Empty-to-nonempty posts signal once per batch;
queue transfer resets the signal under the queue lock. POSIX uses a nonblocking,
close-on-exec pipe and `poll`; Windows uses a manual-reset event and
`WaitForMultipleObjects`. Escape still owns parsing, buffered input, EOF, and resize
checks, and all terminal I/O and callbacks run on the dispatch thread.

The POSIX PTY comparison runs the former polling algorithm and the new driver against
the same current terminal and queue implementation. Each sample opens a fresh PTY,
paints the initial frame, waits without input for a worker post after one second, and
records loop iterations and post-to-callback latency. Five samples alternate drivers.
Iterations include initial dispatch and exit dispatch. Escape's default 100 ms resize
checks remain enabled, so an idle terminal still wakes about ten times per second.
This measures one-second idle samples and scheduler-dependent latency, not CPU usage.
Raw results: [waiting](../tests/benchmark-results/waiting.csv).

```sh
cmake --build build-release --target TermOx.tests.event_loop_terminal -j 4
python3 tests/benchmark_event_loop.py ./build-release/tests/TermOx.tests.event_loop_terminal
```

Frames now keep a revision per row and an owned identity. Copies and resizes receive
fresh identities, moves transfer identity, and the renderer retains ownership of the
last identity so allocator address reuse is harmless. Mutation records revisions
before changing cells, and revision wrap replaces identity. Identical writes and
restyles avoid mutation and replacement string construction. Rendering skips rows
whose identity and revision match, and checks equality on repainted rows before
encoding. Invalidation and default-color changes still force complete repainting.

The renderer comparison uses baseline `d7f88bb` and the same updated benchmark source
in both Release builds, with the toolchain and dependencies described above. Run only
the rendering workloads with `--renderer`. Checksums match and all warmed workloads
perform zero ordinary C++ allocations. The clear-paint workload includes clearing,
writing one glyph per row, and rendering identical content on every timed iteration;
the full-frame workload includes clearing to alternating styles and rendering.

| Workload | Before µs | After µs |
| --- | ---: | ---: |
| renderer.unchanged.80x24 | 13.169 | 0.047 |
| renderer.single-cell.80x24 | 13.257 | 1.234 |
| renderer.clear-paint.80x24 | 17.763 | 20.394 |
| renderer.full-frame.80x24 | 108.433 | 118.209 |
| renderer.unchanged.200x60 | 82.949 | 0.088 |
| renderer.single-cell.200x60 | 83.931 | 2.740 |
| renderer.clear-paint.200x60 | 108.868 | 116.236 |
| renderer.full-frame.200x60 | 679.695 | 731.611 |

Raw results: [rendering before](../tests/benchmark-results/rendering-before.csv) and
[rendering after](../tests/benchmark-results/rendering-after.csv).

Unchanged retained frames are 280–943 times faster in these workloads, and single-cell
updates including mutation are 10.7–30.6 times faster. Clear-and-paint is 7–15% slower;
full-frame style changes are 8–9% slower. These costs are part of the comparison, not
excluded painting work. Application painting currently clears and redraws the whole
frame, so it cannot get the revision-only skip used by retained-frame updates; it
still compares repainted rows. No end-to-end repaint speedup is claimed. Revisions
add eight bytes per row to each frame and renderer, plus one shared identity allocation
per frame construction/copy/resize. Warm mutation and rendering retain storage.

The waiting samples show median loop iterations falling from 59 to 11 per idle second,
and median delivery latency from 12,115 to 220 microseconds (about 55 times lower).
These are local observations; operating-system scheduling determines delivery latency.

Validation: all 143 Release test cases, including five allocation-fault cases, pass.
The seven PTY checks and idle benchmark cover worker wakeups, standalone Escape
parser deadlines, renderer presentation, five examples, resizing, Ctrl-C, and restoration.
ASan+UBSan passes 37 non-exception frame, renderer, and event-loop cases with every
TermOx source instrumented and the unchanged Escape Release library linked. Deliberate
exception cases run in Release; the installed sanitizer runtime cannot unwind even
an independently reproduced caught exception. The POSIX path was built and exercised
on macOS; the Windows wait implementation was not executed on this host.

## Paint pipeline and complete application frames

This pass compares `e49f72b` with the current changes, using the same expanded benchmark
source, dependencies, and Release toolchain described above. It keeps row revisions,
reduces clearing work for cells that are already blank, and compares single-scalar cell
text directly. Equality still includes text, style, and column width. The renderer skips
Unicode boundary segmentation between printable ASCII neighbors; non-ASCII neighbors
still use Escape's segmentation, including the cursor barrier for clusters that join.
Revision bookkeeping moves before encoding; any encoding failure invalidates the cache.
No widget behavior or public API changes are needed.

The additional commands isolate paint phases and measure complete application frames:

```sh
./build-release/tests/TermOx.tests.benchmarks --paint
./build-release/tests/TermOx.tests.benchmarks --application
python3 tests/benchmark_paint_pipeline.py /path/to/before-benchmarks \
    ./build-release/tests/TermOx.tests.benchmarks /tmp/paint-results
```

The comparison harness runs three fresh processes per version and mode, alternating
version order. Each process uses ten warmups and five timed samples per workload.
The tables report the median of those three process medians. The harness checks matching
workload names, iteration counts, and checksums before writing results. Both binaries
must contain the same benchmark source; for this baseline the expanded
`tests/benchmarks.cpp` was built against the library at `e49f72b`. No builds, tests,
or sanitizer processes ran during the recorded measurements.

Raw process samples: [all samples](../tests/benchmark-results/paint-pipeline-samples.csv).
Aggregate CSVs: [renderer before](../tests/benchmark-results/paint-pipeline-renderer-before.csv),
[renderer after](../tests/benchmark-results/paint-pipeline-renderer-after.csv),
[phases before](../tests/benchmark-results/paint-pipeline-paint-before.csv),
[phases after](../tests/benchmark-results/paint-pipeline-paint-after.csv),
[application before](../tests/benchmark-results/paint-pipeline-application-before.csv),
[application after](../tests/benchmark-results/paint-pipeline-application-after.csv).

### Rendering and paint phases

| Workload | 80×24 before µs | After µs | 200×60 before µs | After µs |
| --- | ---: | ---: | ---: | ---: |
| unchanged | 0.047 | 0.056 | 0.087 | 0.110 |
| single-cell | 1.246 | 1.187 | 2.599 | 2.586 |
| clear-paint | 31.057 | 17.729 | 125.138 | 108.116 |
| full-frame | 119.294 | 83.852 | 708.612 | 540.379 |

At 200×60, clearing and repainting identical content is about 14% faster; full-frame
style changes are about 24% faster. Small timings fluctuate: the 80×24 clear-paint
baseline varied noticeably across processes, so its percentage should not be treated
as a stable gain. Retained-frame skipping remains around 0.05–0.11 µs, and a single-cell
update remains around 1–3 µs. Warm renderer workloads still allocate zero times.

A separate alternating comparison reuses the pre-revision `d7f88bb` Release binary and
the identical renderer workloads. Clear-paint is 17.798 → 17.648 µs at 80×24 and
107.624 → 102.118 µs at 200×60; full-frame styles are 108.323 → 82.884 µs and
672.584 → 509.497 µs. This brings clear-paint back to roughly its pre-revision cost
and improves full-frame encoding while retaining the sparse-update gains. Raw results:
[pre-revision baseline](../tests/benchmark-results/paint-pipeline-renderer-pre-revisions.csv),
[current control](../tests/benchmark-results/paint-pipeline-renderer-reference-after.csv),
[control samples](../tests/benchmark-results/paint-pipeline-renderer-reference-samples.csv).
The same harness reproduces this control with `--modes renderer`.

| Workload | 80×24 before µs | After µs | 200×60 before µs | After µs |
| --- | ---: | ---: | ---: | ---: |
| clear | 4.867 | 3.331 | 30.392 | 20.817 |
| draw | 126.007 | 128.368 | 786.051 | 806.401 |
| compare.equal | 13.753 | 11.927 | 85.730 | 72.252 |
| encode.full | 94.177 | 71.053 | 595.020 | 448.182 |

Clear alternates blank styles; draw toggles every cell between ASCII glyphs and includes
frame validation and mutation. Compare scans equal independent frames. Encode invalidates
the renderer before each call, forcing every cell to be emitted; it includes renderer
bookkeeping, cache updates, cursor/style encoding, and UTF-8 conversion, but no frame
mutation or terminal I/O. These are separate synthetic workloads, not additive timings
for one particular application frame. Construction and initial storage growth are
excluded. The phase workloads perform zero ordinary allocations after warmup.

### Complete application frames

| Workload | 80×24 before µs | After µs | 200×60 before µs | After µs |
| --- | ---: | ---: | ---: | ---: |
| idle | 0.383 | 0.381 | 0.383 | 0.381 |
| repaint | 706.603 | 706.210 | 2290.499 | 2291.857 |
| edit | 2956.587 | 2936.157 | 4580.349 | 4555.491 |
| scroll | 721.401 | 724.135 | 2297.691 | 2337.285 |
| resize | 1039.749 | 949.292 | 3093.929 | 2924.013 |

Each workload uses an editable TextBox with 1,000 numbered multilingual lines containing
wide characters, combining marks, and tabs. Initial construction, focus, layout, and
rendering are outside timing. Timed frames include queued event delivery and tree/focus
synchronization where applicable, model changes, layout, clearing, widget painting,
frame comparison, cursor handling, and encoding. No terminal write or flush is measured.

Idle steps verify that no paint occurs and do not call the renderer. Repaint explicitly
requests a full paint with identical content. Editing alternates a character insertion
and Backspace, restoring the document each pair. Scrolling alternates wheel events down
and up one line; numbered lines ensure the visible content changes. Resizing alternates
both dimensions by one cell and includes frame allocation, reflow, and full encoding.
Every non-idle operation verifies that a paint actually happened. Checksums match across
versions and all recorded processes.

Resize frames improve by about 5–9%. Unchanged text repaints, scrolling, and editing are
mostly unchanged; renderer improvements do not translate into the same end-to-end gains.
Ordinary allocation counts also remain unchanged: forced text repaints still allocate
1,335 times at 80×24 and 3,351 times at 200×60, largely in the drawing path. The existing
`put_text_line` path resegments visible text and constructs display strings and glyph
vectors while drawing. That shared drawing path, and layout reconstruction after edits,
are the next useful targets. Application painting still clears and redraws its frame;
these measurements do not claim incremental widget painting.

Validation: all 157 Release cases, including five allocation-fault cases, pass; all
six examples build and the seven PTY smoke checks pass. ASan+UBSan passes all 27
non-exception frame/renderer cases with every TermOx source instrumented and the
unchanged Escape Release library linked. Deliberate exception cases run in Release
because the installed sanitizer runtime cannot unwind C++ exceptions. New coverage
checks exact cell equality, clearing long clusters and wide continuations, and
repainting equal prefixes before sparse and wide-cell changes.

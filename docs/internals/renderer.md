# Frame rendering

`Renderer` compares completed frames and returns terminal bytes encoded with Escape.
It performs no I/O. `Terminal::commit_changes()` uses it to present frames.

```cpp
#include <ox/core/renderer.hpp>

auto profile = esc::TerminalProfile{};
profile.control = true;
profile.colors = esc::ColorSupport::TrueColor;
auto renderer = ox::Renderer{profile};
auto frame = ox::Frame{{20, 5}};
frame.write({2, 1}, U"界");
auto bytes = renderer.render(frame, ox::Point{2, 1});
// Write bytes in full to a terminal with the supplied profile and frame dimensions.
```

The profile is copied at construction and must support cursor positioning. Escape
adapts styles to its declared color and trait capabilities and applies its strict or
best-effort policy. To change profiles, construct a new renderer. The frame's width
policy must match the terminal's glyph widths.

## Changes and runs

The first frame and every resize paint the entire current rectangle. Later frames
emit only changed leading cells, including spaces needed to erase old glyph footprints.
The frame tracks a revision per row. When its identity and a row's revision match
the last presentation, the renderer skips that row without comparing cells. Identical
writes and restyles preserve revisions. Clear marks every row; a row equality check
handles repainting the same content before entering the encoding loop. Copies and
resizes receive fresh identities, moves preserve identity, and retained identity
ownership prevents address reuse from hiding changes. Revision wrap resets identity.

Scrolling content is compared as changed rows; the renderer needs no terminal scroll
regions or scrolling commands. Shrinking assumes the terminal itself was resized;
columns and rows outside the new dimensions are no longer part of the display.

Adjacent changed graphemes continue at the current position, even across style changes.
Gaps and row boundaries use absolute cursor positioning. Reaching the right margin
invalidates the inferred position, so a pending terminal wrap cannot accidentally
scroll the screen. Neighboring graphemes that would join, such as two regional
indicators, receive an explicit cursor command to preserve their separate footprints.

Styles remain active between frames. Commands are emitted only when the encoded
appearance changes after capability adaptation. The `foreground` and `background`
arguments replace frame cells' default colors; explicit cell colors remain intact.
Changing either default color requests a complete repaint.

A valid cursor is positioned at its footprint's leading cell. Missing or out-of-bounds
cursors request no positioning. The caller manages visibility. A frame and cursor
that have not changed produce an empty result.

## Ownership and failures

The renderer retains its cell cache, row revisions, frame identity, output string,
style buffers, and grapheme buffer.
Only changed cell footprints are copied into the cache; there is no full-frame copy
per presentation. Capacity is retained for subsequent frames and smaller resizes.
`render()` returns a borrowed `std::string_view`, valid until the next render or
renderer destruction. Write it before calling `render()` again. Renderer operations
belong to one thread.

A successful call assumes its bytes will be written in full. Call `invalidate()` after
an output failure, discarded result, or external writes that change terminal state.
This forgets cells, style, and cursor position while preserving storage. The next call
repaints completely. Encoding and allocation failures invalidate automatically and
propagate the exception; no partially encoded result is returned.

`Terminal` owns output, synchronization, visibility, and session restoration. It skips
frame creation and flushing for unchanged frames, changes cursor visibility only when
needed, and invalidates its renderer if presentation fails. On shutdown it resets the
retained style before Escape restores session modes.

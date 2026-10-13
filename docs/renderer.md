# Frame rendering

`Renderer` compares completed frames and returns encoded terminal bytes. It opens no
terminal, reads no input, and performs no I/O. It composes with `Frame` and Escape's
encoding API; `Terminal::commit_changes()` uses this same renderer.

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

The renderer retains its cell cache, output string, style buffers, and grapheme buffer.
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

Regression tests cover exact cursor/style sequences, storage reuse, capability
adaptation, themes, Unicode boundaries and footprint replacement, scrolling, shrinking
and growing, invalidation, failed encoding, and randomized edits replayed into a
headless display. Example smoke tests exercise input, resizing, and restoration
through a pseudo-terminal.

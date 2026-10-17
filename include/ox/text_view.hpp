/** @file
 * Text viewport geometry, caret visibility, and hit testing.
 */
#pragma once

#include <ox/align.hpp>
#include <ox/core/canvas.hpp>
#include <ox/core/text.hpp>

namespace ox {

/** Return the widest display row and display-row count from cached text analysis. */
[[nodiscard]] auto text_content_size(TextLayout const& layout) -> Area;
/** Analyze and measure text under available width, wrapping, and owned width
 * assumptions; never mutate a widget tree.
 */
[[nodiscard]] auto measure_text(std::string_view text,
                                Area available,
                                TextWrap wrap = TextWrap::None,
                                esc::WidthPolicy policy = {}) -> Area;

/** Keep a caret visible, snapping horizontal scroll to complete grapheme boundaries.
 * Scroll coordinates are display columns and rows, not byte offsets.
 * Alignment reserves a final cursor cell when short right-aligned text needs it. */
[[nodiscard]] auto text_scroll(TextLayout const& layout,
                               std::size_t caret,
                               Area size,
                               Align align = Align::Left,
                               Point scroll = {}) -> Point;

/** Map a byte boundary or local mouse position through the same aligned viewport. */
[[nodiscard]] auto text_position(TextLayout const& layout,
                                 std::size_t offset,
                                 Area size,
                                 Align align = Align::Left,
                                 Point scroll = {}) -> Point;
/** Map an aligned, scrolled local cell to a clamped UTF-8 grapheme boundary. */
[[nodiscard]] auto text_offset(TextLayout const& layout,
                               Point at,
                               Area size,
                               Align align = Align::Left,
                               Point scroll = {}) -> std::size_t;

/** Draw visible rows using cached segmentation, widths, and selection boundaries.
 * Does not fill the background. Layout and canvas width policies must match. */
void paint_text(Canvas canvas,
                TextLayout const& layout,
                Align align = Align::Left,
                Point scroll = {},
                Style normal = {},
                Style selected = {},
                std::pair<std::size_t, std::size_t> selection = {0, 0});

/** Apply key input, update layout using its owned width policy, and reveal the caret.
 * Return effects for the caller to apply; read_only suppresses document edits.
 */
[[nodiscard]] auto edit_text(TextEdit& editor,
                             TextLayout& layout,
                             Point& scroll,
                             Area size,
                             TextWrap wrap,
                             Align align,
                             KeyEvent const& key,
                             std::function<bool(char32_t)> const& accept = {},
                             bool read_only = false) -> EventResponse;
/** Apply one input chunk/event, refresh layout, reveal the caret, and return effects;
 * read_only suppresses document edits.
 */
[[nodiscard]] auto edit_text(TextEdit& editor,
                             TextLayout& layout,
                             Point& scroll,
                             Area size,
                             TextWrap wrap,
                             Align align,
                             esc::PasteChunk const& chunk,
                             std::function<bool(char32_t)> const& accept = {},
                             bool read_only = false) -> EventResponse;

}  // namespace ox

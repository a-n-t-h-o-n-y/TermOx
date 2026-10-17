/** @file
 * UTF-8 grapheme analysis, display wrapping, and editable text history.
 */
#pragma once

#include <cstdint>
#include <deque>
#include <functional>
#include <optional>
#include <span>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

#include <esc/layout.hpp>

#include <ox/core/events.hpp>
#include <ox/core/geometry.hpp>

namespace ox {

/** Line breaking policy: no soft wrapping, grapheme-boundary wrapping, or preferred word
 * boundaries.
 */
enum class TextWrap {
    None, ///< Keep logical lines intact; viewports may clip or scroll horizontally.
    Any, ///< Wrap at grapheme boundaries that fit the available columns.
    Word ///< Prefer word boundaries, falling back to grapheme boundaries for long words.
};

/** UTF-8 byte offsets. end excludes a newline; next includes it or equals end at
 * a soft wrap. Columns count terminal cells, never bytes or Unicode scalars. */
struct TextLine {
    /** First UTF-8 byte offset in this display row. */
    std::size_t begin;
    /** Exclusive byte offset of row text, excluding any terminating newline. */
    std::size_t end;
    /** Byte offset after an explicit newline, or end at a soft wrap. */
    std::size_t next;
    /** Measured terminal-column count of the row. */
    int columns;
    /** Compare byte boundaries and measured display width. */
    auto operator==(TextLine const&) const -> bool = default;
};

/** Printable display form; tabs become spaces and orphan marks get a dotted circle. */
[[nodiscard]] auto display_grapheme(std::string_view cluster,
                                    int column,
                                    esc::WidthPolicy policy = {},
                                    int tab_width = 4) -> std::u32string;
/** Analyze UTF-8 into display rows at width; reject invalid UTF-8, negative width, or
 * nonpositive tab width.
 */
[[nodiscard]] auto layout_text(std::string_view text,
                               int width,
                               TextWrap wrap = TextWrap::Word,
                               esc::WidthPolicy policy = {},
                               int tab_width = 4) -> std::vector<TextLine>;

/** Owns a cached text snapshot, offsets, and width assumptions, with no borrowed text.
 */
class TextLayout {
   public:
    /** Create an empty layout containing one empty display row. */
    TextLayout() = default;
    /** Copy cached text, segmentation, and owned width overrides. */
    TextLayout(TextLayout const&) = default;
    /** Move cached analysis while preserving the moved-from object's valid empty state. */
    TextLayout(TextLayout&& other);
    /** Replace cached analysis by copy/move-and-swap. */
    auto operator=(TextLayout other) -> TextLayout&;
    /** Exchange cached text, line analysis, and owned width-policy storage without
     * allocation.
     */
    friend void swap(TextLayout& a, TextLayout& b) noexcept;
    /** Copy and analyze text when content or layout assumptions differ; return whether the cache changed.
     * Invalid UTF-8, negative width, or invalid width/tab assumptions throw before committing new analysis.
     */
    auto update(std::string_view text,
                int width,
                TextWrap wrap = TextWrap::Word,
                esc::WidthPolicy policy = {},
                int tab_width = 4) -> bool;
    /** Append complete LF-terminated lines and discard a complete-line byte prefix.
     * Reuse retained analysis; requires an initialized layout ending at a line
     * boundary. */
    auto append_lines(std::string_view suffix, std::size_t discard_prefix = 0) -> bool;
    /** Borrow cached UTF-8 text until update, append, assignment, or destruction. */
    auto text() const -> std::string_view { return text_; }
    /** Borrow grapheme byte boundaries, including the final text size, until the next cache mutation.
     * widths() uses matching cluster indices: positive columns, tab == 0, newline == -1.
     */
    auto boundaries() const -> std::span<std::size_t const> { return boundaries_; }
    /** Borrow cluster widths corresponding to boundaries(): zero for tabs and -1 for
     * newlines.
     */
    auto widths() const -> std::span<int const> { return columns_; }
    /** Borrow display-row byte ranges and column counts until the next cache mutation. */
    auto lines() const -> std::span<TextLine const> { return lines_; }
    /** Return width assumptions borrowing override storage owned by this layout. */
    auto policy() const -> esc::WidthPolicy;
    /** Return the positive terminal-column tab stop interval. */
    auto tab_width() const -> int { return tab_width_; }
    /** Offset must be a grapheme boundary. Soft-wrap boundaries belong to the next row.
     */
    auto position(std::size_t offset) const -> Point;
    /** Clamp to the nearest row and the leading boundary of the hit grapheme. */
    auto offset(Point point) const -> std::size_t;

   private:
    std::string text_;
    std::vector<TextLine> lines_{{0, 0, 0, 0}};
    std::vector<std::size_t> boundaries_{0};
    std::vector<Point> positions_{{0, 0}};
    // Positive measured columns; zero denotes a tab, minus one a line break.
    std::vector<int> columns_;
    std::string text_storage_;
    std::vector<TextLine> line_storage_;
    std::vector<std::size_t> boundary_storage_;
    std::vector<Point> position_storage_;
    std::vector<int> column_storage_;
    std::vector<esc::ScalarWidthOverride> override_storage_;
    int width_ = -1;
    TextWrap wrap_ = TextWrap::Word;
    esc::WidthPolicy policy_;
    std::vector<esc::ScalarWidthOverride> overrides_;
    int tab_width_ = 4;
};

/** Owned UTF-8 editing state. Caret and anchor always lie on grapheme boundaries.
 * Single-line mode removes line breaks and tabs. Both modes normalize CR/CRLF to LF
 * and reject other controls. History stores bounded edit deltas, not whole documents.
 */
class TextEdit {
   public:
    /** Own normalized UTF-8 text, edit mode, and a bounded undo history.
     * Malformed UTF-8 and unsupported controls throw; history_limit=0 disables retained undo records.
     */
    explicit TextEdit(std::string_view text = {},
                      bool multiline = true,
                      std::size_t history_limit = 100);
    /** Copy text, selection, and independent undo/redo history. */
    TextEdit(TextEdit const&) = default;
    /** Transfer editing state while leaving the source usable as an empty editor. */
    TextEdit(TextEdit&& other);
    /** Replace editing state by copy/move-and-swap. */
    auto operator=(TextEdit other) -> TextEdit&;
    /** Exchange documents, selection, history, and pending paste storage without allocation. */
    friend void swap(TextEdit& a, TextEdit& b) noexcept;
    /** Borrow normalized UTF-8 text until a document mutation or destruction. */
    auto text() const -> std::string const& { return text_; }
    /** Monotonic document-change revision used by presentation caches; caret movement
     * alone does not change it.
     */
    auto revision() const -> std::uint64_t { return revision_; }
    /** Return the caret's UTF-8 byte offset at a grapheme boundary. */
    auto caret() const -> std::size_t { return caret_; }
    /** Return the selection anchor's UTF-8 byte offset at a grapheme boundary. */
    auto anchor() const -> std::size_t { return anchor_; }
    /** Return the sorted half-open byte interval between anchor and caret. */
    auto selection() const -> std::pair<std::size_t, std::size_t>;
    /** Borrow all grapheme byte boundaries, including the final text size. */
    auto boundaries() const -> std::span<std::size_t const> { return boundaries_; }
    /** Return whether normalized line breaks and tabs are retained. */
    auto multiline() const -> bool { return multiline_; }
    /** Replace and normalize the document, resetting selection/history; invalid input
     * leaves the old document intact.
     */
    void set_text(std::string_view text);
    /** Stream complete lines, preserving surviving selection and clearing history. */
    auto append_lines(std::string_view suffix, std::size_t discard_prefix = 0) -> bool;
    /** Move to a valid grapheme boundary and return whether selection changed; extend
     * retains the anchor.
     */
    auto set_caret(std::size_t offset, bool extend = false) -> bool;
    /** Select the whole document and return whether selection changed. */
    auto select_all() -> bool;
    /** Select the word at a valid byte boundary and return whether selection changed. */
    auto select_word(std::size_t at) -> bool;
    /** Select the logical line at a valid byte boundary and return whether selection
     * changed.
     */
    auto select_line(std::size_t at) -> bool;
    /** Move left by one grapheme or word; return whether selection changed, retaining
     * the anchor when extend is true.
     */
    auto move_left(bool extend = false, bool word = false) -> bool;
    /** Move right by one grapheme or word; return whether selection changed, retaining
     * the anchor when extend is true.
     */
    auto move_right(bool extend = false, bool word = false) -> bool;
    /** Move by display rows using layout of this text, preserving the preferred display
     * column across successive vertical moves.
     */
    auto move_vertical(TextLayout const& layout, int rows, bool extend = false) -> bool;
    /** Normalize and filter inserted scalars, replace the selection, and record undo; return whether text changed.
     * Malformed input is rejected before committing. A character predicate filters insertion, not existing text.
     */
    auto insert(std::string_view text, std::function<bool(char32_t)> const& accept = {})
        -> bool;
    /** Delete the selection or the previous grapheme/word; return whether text changed. */
    auto erase_backward(bool word = false) -> bool;
    /** Delete the selection or the following grapheme/word; return whether text changed. */
    auto erase_forward(bool word = false) -> bool;
    /** Restore the last retained text edit and its prior selection; return false when
     * history is empty.
     */
    auto undo() -> bool;
    /** Reapply the last undone edit; return false when redo history is empty. */
    auto redo() -> bool;
    /** Return whether a retained edit can be undone. */
    auto can_undo() const -> bool { return !undo_.empty(); }
    /** Return whether an undone edit can be replayed. */
    auto can_redo() const -> bool { return !redo_.empty(); }
    /** Buffer complete paste atomically; aborted or malformed final chunks do not edit.
     */
    auto paste(esc::PasteChunk const& chunk,
               std::function<bool(char32_t)> const& accept = {}) -> bool;

   private:
    struct Change {
        std::size_t at;
        std::string removed;
        std::string inserted;
        std::size_t before_anchor;
        std::size_t before_caret;
        std::size_t after_caret;
    };
    std::uint64_t revision_ = 0;
    std::string text_;
    std::vector<std::size_t> boundaries_{0};
    std::size_t caret_ = 0;
    std::size_t anchor_ = 0;
    bool multiline_;
    std::size_t history_limit_;
    std::deque<Change> undo_;
    std::deque<Change> redo_;
    std::string text_storage_;
    std::vector<std::size_t> boundary_storage_;
    std::optional<int> preferred_column_;
    std::string paste_;
    bool pasting_ = false;

    auto word_left(std::size_t at) const -> std::size_t;
    auto word_right(std::size_t at) const -> std::size_t;
    auto replace(std::size_t begin, std::size_t end, std::string value) -> bool;
    auto replay(bool backward) -> bool;
    void prepare_replace(std::size_t begin, std::size_t end, std::string_view value);
    void commit_text() noexcept;
};

/** Default editing bindings, independently replaceable by widget code. */
[[nodiscard]] auto edit_key(TextEdit& edit,
                            KeyEvent const& key,
                            TextLayout const& layout,
                            std::function<bool(char32_t)> const& accept = {},
                            bool read_only = false) -> EventResponse;

}  // namespace ox

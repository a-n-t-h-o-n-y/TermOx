#pragma once

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

enum class TextWrap { None, Any, Word };

/** UTF-8 byte offsets. end excludes a newline; next includes it or equals end at
 * a soft wrap. Columns count terminal cells, never bytes or Unicode scalars. */
struct TextLine {
    std::size_t begin;
    std::size_t end;
    std::size_t next;
    int columns;
    auto operator==(TextLine const&) const -> bool = default;
};

/** Printable display form; tabs become spaces and orphan marks get a dotted circle. */
[[nodiscard]] auto display_grapheme(std::string_view cluster,
                                    int column,
                                    esc::WidthPolicy policy = {},
                                    int tab_width = 4) -> std::u32string;
[[nodiscard]] auto layout_text(std::string_view text,
                               int width,
                               TextWrap wrap = TextWrap::Word,
                               esc::WidthPolicy policy = {},
                               int tab_width = 4) -> std::vector<TextLine>;

/** Owns a cached text snapshot, offsets, and width assumptions, with no borrowed text.
 */
class TextLayout {
   public:
    TextLayout() = default;
    TextLayout(TextLayout const&) = default;
    TextLayout(TextLayout&& other);
    auto operator=(TextLayout other) -> TextLayout&;
    friend void swap(TextLayout& a, TextLayout& b) noexcept;
    auto update(std::string_view text,
                int width,
                TextWrap wrap = TextWrap::Word,
                esc::WidthPolicy policy = {},
                int tab_width = 4) -> bool;
    auto text() const -> std::string_view { return text_; }
    auto lines() const -> std::span<TextLine const> { return lines_; }
    auto policy() const -> esc::WidthPolicy;
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
    explicit TextEdit(std::string_view text = {},
                      bool multiline = true,
                      std::size_t history_limit = 100);
    TextEdit(TextEdit const&) = default;
    TextEdit(TextEdit&& other);
    auto operator=(TextEdit other) -> TextEdit&;
    friend void swap(TextEdit& a, TextEdit& b) noexcept;
    auto text() const -> std::string const& { return text_; }
    auto caret() const -> std::size_t { return caret_; }
    auto anchor() const -> std::size_t { return anchor_; }
    auto selection() const -> std::pair<std::size_t, std::size_t>;
    auto boundaries() const -> std::span<std::size_t const> { return boundaries_; }
    auto multiline() const -> bool { return multiline_; }
    void set_text(std::string_view text);
    auto set_caret(std::size_t offset, bool extend = false) -> bool;
    auto select_all() -> bool;
    auto move_left(bool extend = false, bool word = false) -> bool;
    auto move_right(bool extend = false, bool word = false) -> bool;
    auto move_vertical(TextLayout const& layout, int rows, bool extend = false) -> bool;
    auto insert(std::string_view text, std::function<bool(char32_t)> const& accept = {})
        -> bool;
    auto erase_backward(bool word = false) -> bool;
    auto erase_forward(bool word = false) -> bool;
    auto undo() -> bool;
    auto redo() -> bool;
    auto can_undo() const -> bool { return !undo_.empty(); }
    auto can_redo() const -> bool { return !redo_.empty(); }
    /** Buffer complete paste atomically; aborted or malformed final chunks do not edit.
     */
    auto paste(esc::PasteChunk const& chunk,
               std::function<bool(char32_t)> const& accept = {}) -> bool;
    auto key_press(KeyEvent const& key,
                   TextLayout const& layout,
                   std::function<bool(char32_t)> const& accept = {}) -> EventResponse;

   private:
    struct Change {
        std::size_t at;
        std::string removed;
        std::string inserted;
        std::size_t before_anchor;
        std::size_t before_caret;
        std::size_t after_caret;
    };
    std::string text_;
    std::vector<std::size_t> boundaries_{0};
    std::size_t caret_ = 0;
    std::size_t anchor_ = 0;
    bool multiline_;
    std::size_t history_limit_;
    std::vector<Change> undo_;
    std::vector<Change> redo_;
    std::optional<int> preferred_column_;
    std::string paste_;
    bool pasting_ = false;

    auto word_left(std::size_t at) const -> std::size_t;
    auto word_right(std::size_t at) const -> std::size_t;
    auto replace(std::size_t begin, std::size_t end, std::string value) -> bool;
    auto replay(bool backward) -> bool;
};

}  // namespace ox

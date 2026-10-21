/** @file
 * Owned grapheme cells, continuation cells, and frame storage.
 */
#pragma once

#include <cstddef>
#include <cstdint>
#include <memory>
#include <span>
#include <string>
#include <vector>

#include <esc/layout.hpp>

#include <ox/core/geometry.hpp>
#include <ox/core/glyph.hpp>

namespace ox {
namespace detail { class SingleColumnPainter; }
class Canvas;

/** A leading cell owns one grapheme. Continuations have empty text and columns == 0. */
struct Cell {
    /** Owned grapheme on a leading cell; continuation cells hold an empty string. */
    std::u32string text = U" ";
    /** Color and trait state for the entire grapheme footprint. */
    Style style = {};
    /** Leading cell width in terminal columns; zero identifies a continuation cell. */
    std::size_t columns = 1;
    /** Compare text, width, and style for rendering equality. */
    auto operator==(Cell const& other) const -> bool
    {
        // Single scalars (especially blanks) dominate frame comparison. Avoid a
        // general string comparison for those cells while preserving exact equality.
        return columns == other.columns && style == other.style &&
               (text.size() == 1 && other.text.size() == 1
                    ? text.front() == other.text.front()
                    : text == other.text);
    }
};

/** Owns a rectangular grid; all mutation preserves complete grapheme footprints. */
class Frame {
   public:
    /** Allocate blank cells and retain an owned copy of width overrides;
     * negative/overflowing extents throw.
     */
    explicit Frame(Area size = {0, 0}, esc::WidthPolicy policy = {});
    /** Copy cells and width assumptions into a distinct rendering identity. */
    Frame(Frame const& other);
    /** Transfer cells, width assumptions, and rendering identity without allocation. */
    Frame(Frame&& other) noexcept;
    /** Replace storage by copy/move-and-swap; existing canvases retain their geometry
     * and clip against the new frame.
     */
    auto operator=(Frame other) noexcept -> Frame&;

    /** Return the current terminal-cell dimensions. */
    [[nodiscard]] auto size() const -> Area { return size_; }
    /** Return width assumptions whose override storage is borrowed from this frame. */
    [[nodiscard]] auto width_policy() const -> esc::WidthPolicy;
    /** Borrow one cell. @throws std::out_of_range Point lies outside the frame. */
    [[nodiscard]] auto operator[](Point point) const -> Cell const&;
    /** Borrow a read-only row until storage changes. @throws std::out_of_range Invalid row. */
    [[nodiscard]] auto row(int y) const -> std::span<Cell const>;
    /** Return the leading cell of a grapheme, including when point is on its continuation. */
    [[nodiscard]] auto lead_at(Point point) const -> Point;

    /** Reset every cell to a default blank, including when size is unchanged.
     * Invalid sizes and allocation failures leave the original frame intact.
     */
    void resize(Area size);
    /** Replace all cells by single-column spaces with style; reject invalid style colors
     * before mutation.
     */
    void clear(Style style = {});

    /** Place exactly one printable grapheme; false means its full footprint did not
     * fit. */
    auto write(Point at, std::u32string_view grapheme, Style style = {}) -> bool;

   private:
    friend class detail::SingleColumnPainter;
    friend class Canvas;
    friend class Renderer;
    // A retained identity prevents address reuse and equal revisions on unrelated
    // frames from hiding changes. Copies receive their own identity.
    std::shared_ptr<char const> identity_ = std::make_shared<char>();
    void changed(int row);
    Area size_;
    std::vector<Cell> cells_;
    std::vector<std::uint64_t> revisions_;
    esc::WidthPolicy policy_;
    std::vector<esc::ScalarWidthOverride> overrides_;

    // Callers validate style/text once; clip is already intersected with the frame.
    auto write_validated(Point at, std::u32string_view grapheme, Style style,
                         Rect clip, std::size_t width) -> bool;
    auto restyle(Point at, Style style, Rect clip) -> bool;
    auto index(Point point) const -> std::size_t;
};
}  // namespace ox

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
class Canvas;

/** A leading cell owns one grapheme. Continuations have empty text and columns == 0. */
struct Cell {
    std::u32string text = U" ";
    Style style = {};
    std::size_t columns = 1;
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
    explicit Frame(Area size = {0, 0}, esc::WidthPolicy policy = {});
    Frame(Frame const& other);
    Frame(Frame&& other) noexcept;
    auto operator=(Frame other) noexcept -> Frame&;

    [[nodiscard]] auto size() const -> Area { return size_; }
    [[nodiscard]] auto width_policy() const -> esc::WidthPolicy;
    [[nodiscard]] auto operator[](Point point) const -> Cell const&;
    [[nodiscard]] auto row(int y) const -> std::span<Cell const>;
    [[nodiscard]] auto lead_at(Point point) const -> Point;

    /** Reset every cell to a default blank, including when size is unchanged.
     * Invalid sizes and allocation failures leave the original frame intact.
     */
    void resize(Area size);
    void clear(Style style = {});

    /** Place exactly one printable grapheme; false means its full footprint did not
     * fit. */
    auto write(Point at, std::u32string_view grapheme, Style style = {}) -> bool;

   private:
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

    auto write(Point at, std::u32string_view grapheme, Style style, Rect clip) -> bool;
    auto restyle(Point at, Style style, Rect clip) -> bool;
    static void validate_style(Style const& style);
    auto index(Point point) const -> std::size_t;
};
}  // namespace ox

/** @file
 * Exclusive selection among labeled radio choices.
 */
#pragma once

#include <cstddef>
#include <optional>
#include <span>
#include <string>
#include <vector>

#include <signals_light/signal.hpp>

#include <ox/core/text.hpp>
#include <ox/scrollbar.hpp>
#include <ox/widget.hpp>
#include <tuple>

namespace ox {

/**
 * A vertical grouping of labeled radio buttons, with exclusive selection via mouse and
 * keyboard.
 *
 * @details Each entry is a single horizontal line. If Focus is enabled, arrow keys and
 * enter are used to make selections.
 *
 *  .on = U" ⬤"_gs,
 *  .off = U" ◯"_gs,
 */
class RadioGroup : public Widget {
   public:
    using Widget::handle;

    /** Construction-time values; field initializers below define the defaults. */
    struct Options {
        /** Initial single-line UTF-8 radio labels. */
        std::vector<std::string> items = {};
        /** Optional initial selected radio-row index. */
        std::optional<std::size_t> selected_index = std::nullopt;
        /** Base drawing style; explicit colors take precedence over theme defaults. */
        Style style = {};
        /** Style patch applied while the control has focus. */
        StylePatch focused_style = {.background = {},
                                    .foreground = {},
                                    .enabled = Trait::Bold,
                                    .disabled = {}};
        /** Glyph sequence representing a selected radio choice. */
        std::vector<Glyph> on = U"(X)"_gs;
        /** Glyph sequence representing an unselected radio choice. */
        std::vector<Glyph> off = U"( )"_gs;
        /** Style patch for the control's selection or state indicator. */
        StylePatch indicator_style = {};
        /** Style patch for selected content; explicit patches override themed selection
         * defaults.
         */
        StylePatch selected_style = {};
        /** Style patch for choices that are not selected. */
        StylePatch unselected_style = {};
        /** Keyboard- and pointer-focus eligibility for this control. */
        FocusPolicy focus_policy = FocusPolicy::Strong;
        /** Horizontal size bounds and growth weight. */
        SizePolicy width_policy = SizePolicy::flex();
        /** Vertical size bounds and growth weight. */
        SizePolicy height_policy = SizePolicy::flex();
    } static const init; ///< Default options used when no explicit configuration is supplied.

    /** Borrow radio labels until replacement; use set_items() rather than mutating storage. */
    auto items() const -> std::span<std::string const> { return items_; }
    /** Return the committed row, or nullopt when no choice is selected. */
    auto selected_index() const -> std::optional<std::size_t> { return selected_; }
    /** Validate and replace labels, resetting row eligibility and reconciling
     * selection/scrolling; request layout and repaint.
     */
    void set_items(std::vector<std::string> items);
    /** Base drawing style; explicit colors take precedence over theme defaults. */
    Style style;
    /** Style patch applied while the control has focus. */
    StylePatch focused_style;
    /** Style patch for the control's selection or state indicator. */
    StylePatch indicator_style;
    /** Style patch for selected content; explicit patches override themed selection
     * defaults.
     */
    StylePatch selected_style;
    /** Style patch for choices that are not selected. */
    StylePatch unselected_style;
    /** Glyph sequence representing a selected radio choice. */
    std::vector<Glyph> on;
    /** Glyph sequence representing an unselected radio choice. */
    std::vector<Glyph> off;

    /** Return the keyboard navigation row, which may differ from the committed choice. */
    auto highlighted_index() const -> std::optional<std::size_t>
    { return highlighted_; }
    /** Emits the optional newly selected row only when committed selection changes. */
    sl::Signal<void(std::optional<std::size_t>)> on_select;

   public:
    /** Own validated row labels, initial selection, and indicator styles. */
    RadioGroup(Options x = init);
    /** Transfer widget identity, retained state, and signal subscriptions; tracked
     * references follow the move.
     */
    RadioGroup(RadioGroup&& other);
    /** Replace this widget's identity and retained state with the source; previous
     * lifetime views of this identity expire.
     */
    auto operator=(RadioGroup&& other) -> RadioGroup&;

   public:
    /** Commit an enabled row or clear selection with nullopt; reveal it and notify only on selection change.
     * Return whether selection or scrolling changed. Disabled rows return false.
     * @throws std::out_of_range The index is outside items().
     */
    auto select(std::optional<std::size_t> index) -> bool;

    auto measure(Area, esc::WidthPolicy = {}) const -> Area override;
    auto paint(Canvas c) -> Cursor override;

    auto handle(KeyEvent const& k) -> EventResponse override;

    auto handle(MousePress const& m) -> EventResponse override;

    auto handle(MouseWheel const&) -> EventResponse override;
    /** Return the complete radio-list extent in terminal cells. */
    auto content_size() const -> Area;
    /** Return the assigned visible list extent. */
    auto viewport_size() const -> Area { return size(); }
    /** Return vertical row offset; horizontal scrolling is zero. */
    auto scroll_offset() const -> Point { return {0, scroll_}; }
    /** Clamp the requested vertical offset, repaint and notify on change; ignore the
     * horizontal component.
     */
    auto set_scroll_offset(Point) -> bool;
    /** Emits (offset, content extent, viewport extent) when scroll state or geometry
     * changes; units are terminal columns and rows.
     */
    sl::Signal<void(Point, Area, Area)> on_scroll;
    /** Return row eligibility. @throws std::out_of_range Invalid row index. */
    auto item_enabled(std::size_t index) const -> bool
    { return enabled_items_.at(index); }
    /** Change row eligibility, clear an affected selection, and reconcile keyboard
     * highlight; invalid indices throw.
     */
    void set_item_enabled(std::size_t, bool);
    void refresh() override;
    auto focus_in() -> EventResponse override;

    auto focus_out() -> EventResponse override;

   private:
    std::vector<std::string> items_;
    std::optional<std::size_t> selected_;
    std::optional<std::size_t> highlighted_;
    mutable std::vector<TextLayout> measure_layouts_;
    std::vector<bool> enabled_items_;
    int scroll_ = 0;
    std::optional<std::tuple<Point, Area, Area>> notified_scroll_;
    void notify_scroll();

   protected:
    void on_arrange(Rect) override;
};

}  // namespace ox
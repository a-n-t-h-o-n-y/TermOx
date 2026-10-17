/** @file
 * Selectable lists with disabled rows, activation, and scrolling.
 */
#pragma once
#include <chrono>
#include <ox/scrollbar.hpp>
#include <ox/selection.hpp>
#include <ox/widget.hpp>
namespace ox {
/** One owned printable row with input eligibility and an optional style patch. */
struct ListItem {
    /** Printable single-line UTF-8 row label. */
    std::string text;
    /** Whether this row can be selected or activated. */
    bool enabled = true;
    /** Base drawing style; explicit colors take precedence over theme defaults. */
    StylePatch style = {};
};
/** One row per item, explicit single/multiple selection, and an independent viewport.
 */
class ListView : public Widget {
   public:
    using Widget::handle;
    /** Construction-time values; field initializers below define the defaults. */
    struct Options {
        /** Initial owned row labels, eligibility flags, and style patches. */
        std::vector<ListItem> items = {};
        /** Enable toggle/range/multiple selection; otherwise selection commands replace
         * the selected row.
         */
        bool multiple = false;
        /** Base drawing style; explicit colors take precedence over theme defaults. */
        Style style = {};
        /** Semantic theme role used when drawing colors are unspecified. */
        Surface role = Surface::Canvas;
        /** Style patch for selected content; explicit patches override themed selection
         * defaults.
         */
        StylePatch selected_style = {.background = {},
                                     .foreground = {},
                                     .enabled = Trait::Inverse,
                                     .disabled = {}};
        /** Style patch for the current navigation row, independently of multi-selection. */
        StylePatch current_style = {.background = {},
                                    .foreground = {},
                                    .enabled = Trait::Bold,
                                    .disabled = {}};
        /** Style patch applied while the control has focus. */
        StylePatch focused_style = {};
        /** Style patch for the control when it does not have focus. */
        StylePatch blurred_style = {};
        /** Selection patch used when this view does not own focus. */
        StylePatch inactive_selected_style = {};
        /** Style patch for alternating rows when striped is enabled. */
        StylePatch alternate_style = {};
        /** Style patch for pointer hover feedback. */
        StylePatch hover_style = {};
        /** Style patch for the control's selection or state indicator. */
        StylePatch indicator_style = {};
        /** Style patch for the empty-content message. */
        StylePatch empty_style = {};
        /** Enable alternate-row styling; call refresh() after changing view configuration. */
        bool striped = false;
        /** Enable row hover feedback from pointer movement. */
        bool hover_rows = false;
        /** Reserve a selection-indicator gutter when themed styling is enabled. */
        bool show_indicator = true;
        /** Single-line message displayed when the model has no visible rows. */
        std::string empty_text = "";
        /** Keyboard- and pointer-focus eligibility for this control. */
        FocusPolicy focus_policy = FocusPolicy::Strong;
        /** Horizontal size bounds and growth weight. */
        SizePolicy width_policy = SizePolicy::flex();
        /** Vertical size bounds and growth weight. */
        SizePolicy height_policy = SizePolicy::flex();
    };
    /** Enable toggle/range/multiple selection; otherwise selection commands replace the
     * selected row.
     */
    bool multiple;
    /** Base drawing style; explicit colors take precedence over theme defaults. */
    Style style;
    /** Semantic theme role used when drawing colors are unspecified. */
    Surface role;
    /** Style patch for selected content; explicit patches override themed selection
     * defaults.
     */
    StylePatch selected_style;
    /** Style patch for the current navigation row, independently of multi-selection. */
    StylePatch current_style;
    /** Style patch applied while the control has focus. */
    StylePatch focused_style;
    /** Style patch for the control when it does not have focus. */
    StylePatch blurred_style;
    /** Selection patch used when this view does not own focus. */
    StylePatch inactive_selected_style;
    /** Style patch for alternating rows when striped is enabled. */
    StylePatch alternate_style;
    /** Style patch for pointer hover feedback. */
    StylePatch hover_style;
    /** Style patch for the control's selection or state indicator. */
    StylePatch indicator_style;
    /** Style patch for the empty-content message. */
    StylePatch empty_style;
    /** Enable alternate-row styling; call refresh() after changing view configuration. */
    bool striped;
    /** Enable row hover feedback from pointer movement. */
    bool hover_rows;
    /** Reserve a selection-indicator gutter when themed styling is enabled. */
    bool show_indicator;
    /** Single-line message displayed when the model has no visible rows. */
    std::string empty_text;
    /** Return the themed selection gutter width, or zero for plain/indicator-free
     * presentation.
     */
    auto indicator_width() const -> int
    { return theme().styled && show_indicator ? 2 : 0; }
    /** Return the visible row under the stored pointer position, or nullopt. */
    auto hovered_row() const -> std::optional<std::size_t>;
    /** Emits a snapshot of selected row indices when the selected set changes; arguments
     * are borrowed during emission.
     */
    sl::Signal<void(std::vector<std::size_t> const&)> on_select;
    /** Emits the optional current navigation row when it changes. */
    sl::Signal<void(std::optional<std::size_t>)> on_current;
    /** Emits the index of an enabled row activated by command, Enter, or double-click. */
    sl::Signal<void(std::size_t)> on_activate;
    /** Emits (offset, content extent, viewport extent) when scroll state or geometry
     * changes; units are terminal columns and rows.
     */
    sl::Signal<void(Point, Area, Area)> on_scroll;
    /** Create an empty single-selection list with default presentation. */
    ListView();
    /** Own and validate initial items, selection policy, and appearance options. */
    explicit ListView(Options);
    /** Borrow row data until a model mutation replaces or reallocates it. */
    auto items() const -> std::span<ListItem const> { return items_; }
    /** Validate and replace the model, clear selection/current row and scrolling, then
     * refresh and notify changed state.
     */
    void set_items(std::vector<ListItem>);
    /** Return the item-mutation revision, independent of selection and scrolling. */
    auto items_revision() const -> std::uint64_t { return items_revision_; }
    /** Validate and replace one row, reconciling eligibility, cached text, and
     * notifications; invalid indices throw.
     */
    void set_item(std::size_t, ListItem);
    /** Insert a validated row, shifting affected selection indices; index may equal item
     * count.
     */
    void insert_item(std::size_t, ListItem);
    /** Erase one row and reconcile selection/current indices and viewport; invalid
     * indices throw.
     */
    void erase_item(std::size_t);
    /** Borrow sorted selected row indices until selection or model mutation. */
    auto selected_rows() const -> std::span<std::size_t const>
    { return selection_.rows; }
    /** Return the independent keyboard navigation row, if any. */
    auto current_row() const -> std::optional<std::size_t>
    { return selection_.current; }
    /** Apply replacement, range, toggle, or cursor-only selection; nullopt clears selection/current state.
     * Reveal eligible targets, repaint changed state, and emit selection/current/scroll notifications.
     * Single-selection mode converts non-Move actions to Replace. Invalid indices throw.
     */
    auto select(std::optional<std::size_t>, SelectionAction = SelectionAction::Replace)
        -> bool;
    /** Select all enabled rows in multiple mode; return false in single mode or when the
     * set is unchanged.
     */
    auto select_all() -> bool;
    /** Emit on_activate for an enabled valid row; return false when ineligible and throw
     * for an invalid index.
     */
    auto activate(std::size_t) -> bool;
    /** Return clamped content offset in terminal columns and rows. */
    auto scroll_offset() const -> Point { return offset_; }
    /** Return the full analyzed row extent, including any selection-indicator gutter. */
    auto content_size() const -> Area { return content_; }
    /** Return this list's assigned terminal-cell extent. */
    auto viewport_size() const -> Area { return size(); }
    /** Clamp both axes; repaint and emit on_scroll only when effective scroll state changes. */
    auto set_scroll_offset(Point) -> bool;
    void refresh() override;
    auto measure(Area, esc::WidthPolicy = {}) const -> Area override;
    auto paint(Canvas) -> Cursor override;
    /** Navigate with Up/Down, Home/End, and PageUp/PageDown; Enter activates.
     * In multiple mode Shift extends ranges, Ctrl moves only the cursor, Space toggles,
     * and Ctrl+A selects enabled rows. Printable text performs incremental prefix search.
     */
    auto handle(KeyEvent const&) -> EventResponse override;
    auto handle(MousePress const&) -> EventResponse override;
    auto handle(MouseWheel const&) -> EventResponse override;
    auto handle(MouseMove const&) -> EventResponse override;
    auto mouse_leave() -> EventResponse override;

   protected:
    void on_arrange(Rect) override;

   private:
    std::vector<ListItem> items_;
    std::uint64_t items_revision_ = 0;
    std::optional<std::size_t> click_row_;
    std::chrono::steady_clock::time_point click_time_ = {};
    RowSelection selection_;
    std::optional<Point> hover_;
    TextLayout empty_layout_;
    Point offset_ = {};
    Area content_ = {};
    mutable std::vector<TextLayout> layouts_;
    std::vector<std::size_t> notified_rows_;
    std::optional<std::size_t> notified_current_;
    std::optional<std::tuple<Point, Area, Area>> notified_scroll_;
    std::u32string search_;
    std::chrono::steady_clock::time_point search_time_ = {};
    void update_viewport();
    void notify();
};
}  // namespace ox

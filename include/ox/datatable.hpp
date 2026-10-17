/** @file
 * Column definitions, row selection, sorting, and table scrolling.
 */
#pragma once
#include <ox/scrollbar.hpp>

#include <functional>
#include <optional>
#include <span>
#include <string>
#include <string_view>
#include <tuple>
#include <vector>

#include <ox/align.hpp>
#include <ox/core/text.hpp>
#include <ox/widget.hpp>

namespace ox {
class ScrollBar;

/** Width includes cell padding; separators are additional. */
struct TableColumn {
    /** Printable single-line UTF-8 column heading. */
    std::string heading;
    /** Column allocation policy including horizontal cell padding; separators are
     * additional.
     */
    SizePolicy size = SizePolicy::min(8);
    /** Text alignment along its display direction. */
    Align align = Align::Left;
    /** Base drawing style; explicit colors take precedence over theme defaults. */
    StylePatch style = {};
    /** Additional style patch applied to this column's heading. */
    StylePatch heading_style = {};
};

/** Rectangular UTF-8 data, fixed headers, row selection, and a two-axis viewport.
 * Cells are single-line printable text. Data mutations validate before committing.
 * Styling/configuration is public; call refresh() after changing configuration. */
class DataTable : public Widget {
   public:
    using Widget::handle;
    /** Construction-time values; field initializers below define the defaults. */
    struct Options {
        /** Initial column descriptions; widths, alignments, and styles are validated
         * before replacement.
         */
        std::vector<TableColumn> columns = {};
        /** Initial owned rectangular table data; each row must match the column count. */
        std::vector<std::vector<std::string>> rows = {};
        /** Base drawing style; explicit colors take precedence over theme defaults. */
        Style style = {};
        /** Style for table rules and separators. */
        Style line_style = {};
        /** Style patch for table heading cells. */
        StylePatch header_style = {.background = {},
                                   .foreground = {},
                                   .enabled = Trait::Bold,
                                   .disabled = {}};
        /** Style patch for alternating rows when striped is enabled. */
        StylePatch alternate_style = {};
        /** Style patch for selected content; explicit patches override themed selection
         * defaults.
         */
        StylePatch selected_style = {.background = {},
                                     .foreground = {},
                                     .enabled = Trait::Inverse,
                                     .disabled = {}};
        /** Style patch applied while the control has focus. */
        StylePatch focused_style = {};
        /** Style patch for the control when it does not have focus. */
        StylePatch blurred_style = {};
        /** Selection patch used when this view does not own focus. */
        StylePatch inactive_selected_style = {};
        /** Style patch for pointer hover feedback. */
        StylePatch hover_style = {};
        /** Style patch for the empty-content message. */
        StylePatch empty_style = {};
        /** Enable alternate-row styling; call refresh() after changing view configuration. */
        bool striped = false;
        /** Enable row hover feedback from pointer movement. */
        bool hover_rows = false;
        /** Draw a rule beneath the fixed header; affects viewport geometry. */
        bool header_separator = true;
        /** Draw separators between body rows; affects content height. */
        bool row_lines = false;
        /** Show the last explicit sort direction in the relevant heading. */
        bool show_sort_indicator = true;
        /** Nonnegative cells of horizontal padding inside each table cell. */
        int cell_padding = 1;
        /** Positive terminal-row height allocated to each table body row. */
        int row_height = 1;
        /** Pure visible-cell styling; selection/hover patches apply afterwards. */
        std::function<StylePatch(std::size_t, std::size_t)> cell_style = {};
        /** Keep column headings above the vertically scrolling body. */
        bool show_header = true;
        /** Draw separators between table columns; separators consume additional width. */
        bool column_lines = true;
        /** Allow row selection when true; setting false prevents new nonempty selections. */
        bool selectable = true;
        /** Single-line message displayed when the model has no visible rows. */
        std::string empty_text = "No rows";
        /** Keyboard- and pointer-focus eligibility for this control. */
        FocusPolicy focus_policy = FocusPolicy::Strong;
        /** Horizontal size bounds and growth weight. */
        SizePolicy width_policy = SizePolicy::flex();
        /** Vertical size bounds and growth weight. */
        SizePolicy height_policy = SizePolicy::flex();
    } static const init; ///< Default options used when no explicit configuration is supplied.

    /** Base drawing style; explicit colors take precedence over theme defaults. */
    Style style;
    /** Style for table rules and separators. */
    Style line_style;
    /** Style patch for table heading cells. */
    StylePatch header_style;
    /** Style patch for alternating rows when striped is enabled. */
    StylePatch alternate_style;
    /** Style patch for selected content; explicit patches override themed selection
     * defaults.
     */
    StylePatch selected_style;
    /** Style patch applied while the control has focus. */
    StylePatch focused_style;
    /** Style patch for the control when it does not have focus. */
    StylePatch blurred_style;
    /** Selection patch used when this view does not own focus. */
    StylePatch inactive_selected_style;
    /** Style patch for pointer hover feedback. */
    StylePatch hover_style;
    /** Style patch for the empty-content message. */
    StylePatch empty_style;
    /** Enable alternate-row styling; call refresh() after changing view configuration. */
    bool striped;
    /** Enable row hover feedback from pointer movement. */
    bool hover_rows;
    /** Draw a rule beneath the fixed header; affects viewport geometry. */
    bool header_separator;
    /** Draw separators between body rows; affects content height. */
    bool row_lines;
    /** Show the last explicit sort direction in the relevant heading. */
    bool show_sort_indicator;
    /** Nonnegative cells of horizontal padding inside each table cell. */
    int cell_padding;
    /** Positive terminal-row height allocated to each table body row. */
    int row_height;
    /** Pure callback returning a patch for a visible body cell (row, column); must not
     * mutate the table.
     */
    std::function<StylePatch(std::size_t, std::size_t)> cell_style;
    /** Keep column headings above the vertically scrolling body. */
    bool show_header;
    /** Draw separators between table columns; separators consume additional width. */
    bool column_lines;
    /** Allow row selection when true; setting false prevents new nonempty selections. */
    bool selectable;
    /** Single-line message displayed when the model has no visible rows. */
    std::string empty_text;

    /** Index snapshots; callbacks may mutate or destroy the table. */
    sl::Signal<void(std::optional<std::size_t>)> on_select;
    /** Emits the activated body-row index; callbacks may mutate or destroy the table. */
    sl::Signal<void(std::size_t)> on_activate;
    /** Emits a clicked column index; sorting is an explicit application decision.
     * To toggle ascending/descending order on heading clicks:
     * @code{.cpp}
     * auto table = ox::DataTable{{
     *     .columns = {{.heading = "Name"}},
     *     .rows = {{"Birch"}, {"Alder"}},
     * }};
     * ox::Connection{
     *     .signal = table.on_header,
     *     .slot = [](std::size_t column, ox::DataTable& table) {
     *         auto order = table.sort_order();
     *         bool ascending = !order || order->first != column || !order->second;
     *         table.sort_rows(column, ascending);
     *     },
     * }(table);
     * @endcode
     */
    sl::Signal<void(std::size_t)> on_header;
    /** offset, content extent, viewport extent. Also notifies on data/geometry changes.
     */
    sl::Signal<void(Point, Area, Area)> on_scroll;

    /** Own validated schema and rectangular rows; initially no row is selected. */
    DataTable(Options x = init);
    /** Borrow validated schema until set_data() replaces it. */
    [[nodiscard]] auto columns() const -> std::span<TableColumn const>
    { return columns_; }
    /** Borrow owned rows until model mutation; row and cell views must not survive mutation. */
    [[nodiscard]] auto rows() const -> std::span<std::vector<std::string> const>
    { return rows_; }
    /** Return the current row index, or nullopt for no selection. */
    [[nodiscard]] auto selected_row() const -> std::optional<std::size_t>
    { return selected_; }
    /** Return the body viewport offset; the fixed header does not scroll vertically. */
    [[nodiscard]] auto scroll_offset() const -> Point { return offset_; }
    /** Return scrollable content extent in terminal cells. */
    [[nodiscard]] auto content_size() const -> Area { return content_; }
    /** Return the body viewport excluding fixed header rows. */
    [[nodiscard]] auto viewport_size() const -> Area { return viewport_; }

    /** Last explicit sort {column, ascending}; insertion/key edits/replacement clear
     * it. */
    auto sort_order() const -> std::optional<std::pair<std::size_t, bool>>
    { return sort_; }
    /** Return the body row at the pointer, or nullopt outside visible body cells. */
    auto hovered_row() const -> std::optional<std::size_t>;
    /** Validate and replace schema and rows atomically; clear selection, sort state, hover, and both scroll offsets.
     * @throws std::invalid_argument Invalid UTF-8, row shape, column policy, alignment, or style.
     * @throws std::length_error Content dimensions cannot be represented.
     */
    void set_data(std::vector<TableColumn> columns,
                  std::vector<std::vector<std::string>> rows);
    /** Replace validated rows against the existing schema; clear selection/sort/hover
     * and reset vertical scrolling. Validation precedes replacement.
     */
    void set_rows(std::vector<std::vector<std::string>> rows);
    /** Insert a validated row, preserving existing selected-row identity and viewport;
     * clear sort state. Index may equal row count.
     */
    void insert_row(std::size_t index, std::vector<std::string> row);
    /** Append through insert_row(); validate column count and printable UTF-8 before
     * mutation.
     */
    void add_row(std::vector<std::string> row);
    /** Erase a valid row, preserve surviving selection identity, and clamp the viewport;
     * an invalid index throws std::out_of_range.
     */
    void erase_row(std::size_t index);
    /** Validate and replace one printable cell; repaint on change and clear sort state
     * only when editing the sort column. Invalid indices throw.
     */
    void set_cell(std::size_t row, std::size_t column, std::string text);
    /** Stable sort; preserve the selected row. Empty comparator means lexical order.
     * Comparators must not mutate the table. Exceptions leave rows/selection unchanged.
     */
    void sort_rows(std::size_t column,
                   bool ascending = true,
                   std::function<bool(std::string_view, std::string_view)> less = {});
    /** Recompute widths, clamp scroll/selection, schedule paint and publish changes. */
    void refresh() override;
    /** Select and reveal a row, or clear with nullopt; invalid/nonselectable rows return
     * false. Emit only changed selection/scroll state.
     */
    auto select(std::optional<std::size_t> row) -> bool;
    /** Clamp body scroll offsets and repaint/notify on effective change; return whether
     * the offset changed.
     */
    auto set_scroll_offset(Point position) -> bool;

    auto measure(Area, esc::WidthPolicy = {}) const -> Area override;
    auto paint(Canvas) -> Cursor override;
    /** Navigate rows with Up/Down, Home/End, and PageUp/PageDown; Left/Right scroll horizontally.
     * Enter/Space activates the current row. With selectable=false, navigation scrolls
     * instead. Releases and modified keys are ignored.
     */
    auto handle(KeyEvent const&) -> EventResponse override;
    auto handle(MousePress const&) -> EventResponse override;
    auto handle(MouseWheel const&) -> EventResponse override;
    auto handle(MouseMove const&) -> EventResponse override;
    auto mouse_leave() -> EventResponse override;

   protected:
    void on_arrange(Rect) override;

   private:
    std::vector<TableColumn> columns_;
    std::vector<std::vector<std::string>> rows_;
    std::optional<std::size_t> selected_;
    Point offset_ = {};
    Area content_ = {};
    Area viewport_ = {};
    std::vector<int> widths_;
    TextLayout cell_layout_;
    std::optional<std::size_t> notified_selection_;
    std::optional<std::tuple<Point, Area, Area>> notified_scroll_;
    std::optional<std::pair<std::size_t, bool>> sort_;
    std::optional<Point> hover_;
    auto row_stride() const -> int;
    auto row_extent(std::size_t count) const -> int;
    auto row_at(int y) const -> std::optional<std::size_t>;
    void paint_separator(Canvas, int y, Style);
    [[nodiscard]] auto header_height() const -> int;
    void update_geometry();
    void update_viewport();
    void data_changed();
    void notify();
    void paint_cell(Canvas, std::string_view, Align, Style);
};

}  // namespace ox

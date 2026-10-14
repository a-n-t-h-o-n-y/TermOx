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

/** Width includes one padding cell at either side; separators are additional. */
struct TableColumn {
    std::string heading;
    SizePolicy size = SizePolicy::min(8);
    Align align = Align::Left;
    StylePatch style = {};
};

/** Rectangular UTF-8 data, fixed headers, row selection, and a two-axis viewport.
 * Cells are single-line printable text. Data mutations validate before committing.
 * Styling/configuration is public; call refresh() after changing configuration. */
class DataTable : public Widget {
   public:
    using Widget::handle;
    struct Options {
        std::vector<TableColumn> columns = {};
        std::vector<std::vector<std::string>> rows = {};
        Style style = {};
        Style line_style = {};
        StylePatch header_style = {.background = {},
                                   .foreground = {},
                                   .enabled = Trait::Bold,
                                   .disabled = {}};
        StylePatch alternate_style = {};
        StylePatch selected_style = {.background = {},
                                     .foreground = {},
                                     .enabled = Trait::Inverse,
                                     .disabled = {}};
        bool show_header = true;
        bool column_lines = true;
        bool selectable = true;
        std::string empty_text = "No rows";
        FocusPolicy focus_policy = FocusPolicy::Strong;
        SizePolicy width_policy = SizePolicy::flex();
        SizePolicy height_policy = SizePolicy::flex();
    } static const init;

    Style style;
    Style line_style;
    StylePatch header_style;
    StylePatch alternate_style;
    StylePatch selected_style;
    bool show_header;
    bool column_lines;
    bool selectable;
    std::string empty_text;

    /** Index snapshots; callbacks may mutate or destroy the table. */
    sl::Signal<void(std::optional<std::size_t>)> on_select;
    sl::Signal<void(std::size_t)> on_activate;
    sl::Signal<void(std::size_t)> on_header;
    /** offset, content extent, viewport extent. Also notifies on data/geometry changes.
     */
    sl::Signal<void(Point, Area, Area)> on_scroll;

    DataTable(Options x = init);
    [[nodiscard]] auto columns() const -> std::span<TableColumn const>
    { return columns_; }
    [[nodiscard]] auto rows() const -> std::span<std::vector<std::string> const>
    { return rows_; }
    [[nodiscard]] auto selected_row() const -> std::optional<std::size_t>
    { return selected_; }
    [[nodiscard]] auto scroll_offset() const -> Point { return offset_; }
    [[nodiscard]] auto content_size() const -> Area { return content_; }
    [[nodiscard]] auto viewport_size() const -> Area { return viewport_; }

    void set_data(std::vector<TableColumn> columns,
                  std::vector<std::vector<std::string>> rows);
    void set_rows(std::vector<std::vector<std::string>> rows);
    void insert_row(std::size_t index, std::vector<std::string> row);
    void add_row(std::vector<std::string> row);
    void erase_row(std::size_t index);
    void set_cell(std::size_t row, std::size_t column, std::string text);
    /** Stable sort; preserve the selected row. Empty comparator means lexical order.
     * Comparators must not mutate the table. Exceptions leave rows/selection unchanged.
     */
    void sort_rows(std::size_t column,
                   bool ascending = true,
                   std::function<bool(std::string_view, std::string_view)> less = {});
    /** Recompute widths, clamp scroll/selection, schedule paint and publish changes. */
    void refresh() override;
    auto select(std::optional<std::size_t> row) -> bool;
    auto set_scroll_offset(Point position) -> bool;

    auto measure(Area, esc::WidthPolicy = {}) const -> Area override;
    auto paint(Canvas) -> Cursor override;
    auto handle(KeyEvent const&) -> EventResponse override;
    auto handle(MousePress const&) -> EventResponse override;
    auto handle(MouseWheel const&) -> EventResponse override;

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
    [[nodiscard]] auto header_height() const -> int;
    void update_geometry();
    void update_viewport();
    void data_changed();
    void notify();
    void paint_cell(Canvas, std::string_view, Align, Style);
};

}  // namespace ox

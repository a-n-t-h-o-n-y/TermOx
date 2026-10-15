#pragma once
#include <chrono>
#include <ox/scrollbar.hpp>
#include <ox/selection.hpp>
#include <ox/widget.hpp>
namespace ox {
struct ListItem {
    std::string text;
    bool enabled = true;
    StylePatch style = {};
};
/** One row per item, explicit single/multiple selection, and an independent viewport.
 */
class ListView : public Widget {
   public:
    using Widget::handle;
    struct Options {
        std::vector<ListItem> items = {};
        bool multiple = false;
        Style style = {};
        Surface role = Surface::Canvas;
        StylePatch selected_style = {.background = {},
                                     .foreground = {},
                                     .enabled = Trait::Inverse,
                                     .disabled = {}};
        StylePatch current_style = {.background = {},
                                    .foreground = {},
                                    .enabled = Trait::Bold,
                                    .disabled = {}};
        StylePatch focused_style = {};
        StylePatch blurred_style = {};
        StylePatch inactive_selected_style = {};
        StylePatch alternate_style = {};
        StylePatch hover_style = {};
        StylePatch indicator_style = {};
        StylePatch empty_style = {};
        bool striped = false;
        bool hover_rows = false;
        bool show_indicator = true;
        std::string empty_text = "";
        FocusPolicy focus_policy = FocusPolicy::Strong;
        SizePolicy width_policy = SizePolicy::flex();
        SizePolicy height_policy = SizePolicy::flex();
    };
    bool multiple;
    Style style;
    Surface role;
    StylePatch selected_style;
    StylePatch current_style;
    StylePatch focused_style;
    StylePatch blurred_style;
    StylePatch inactive_selected_style;
    StylePatch alternate_style;
    StylePatch hover_style;
    StylePatch indicator_style;
    StylePatch empty_style;
    bool striped;
    bool hover_rows;
    bool show_indicator;
    std::string empty_text;
    auto indicator_width() const -> int
    { return theme().styled && show_indicator ? 2 : 0; }
    auto hovered_row() const -> std::optional<std::size_t>;
    sl::Signal<void(std::vector<std::size_t> const&)> on_select;
    sl::Signal<void(std::optional<std::size_t>)> on_current;
    sl::Signal<void(std::size_t)> on_activate;
    sl::Signal<void(Point, Area, Area)> on_scroll;
    ListView();
    explicit ListView(Options);
    auto items() const -> std::span<ListItem const> { return items_; }
    void set_items(std::vector<ListItem>);
    auto items_revision() const -> std::uint64_t { return items_revision_; }
    void set_item(std::size_t, ListItem);
    void insert_item(std::size_t, ListItem);
    void erase_item(std::size_t);
    auto selected_rows() const -> std::span<std::size_t const>
    { return selection_.rows; }
    auto current_row() const -> std::optional<std::size_t>
    { return selection_.current; }
    auto select(std::optional<std::size_t>, SelectionAction = SelectionAction::Replace)
        -> bool;
    auto select_all() -> bool;
    auto activate(std::size_t) -> bool;
    auto scroll_offset() const -> Point { return offset_; }
    auto content_size() const -> Area { return content_; }
    auto viewport_size() const -> Area { return size(); }
    auto set_scroll_offset(Point) -> bool;
    void refresh() override;
    auto measure(Area, esc::WidthPolicy = {}) const -> Area override;
    auto paint(Canvas) -> Cursor override;
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

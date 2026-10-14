#include <ox/datatable.hpp>

#include <algorithm>
#include <cstdint>
#include <limits>
#include <numeric>
#include <stdexcept>
#include <utility>

#include <ox/controls.hpp>
#include <ox/put.hpp>
#include <ox/scroll.hpp>
#include <ox/scrollbar.hpp>
#include <ox/selection.hpp>

namespace {
void check_text(std::string_view text)
{
    for (auto scalar : esc::utf8_to_utf32(text)) {
        if (scalar < U' ' || (scalar >= U'\x7f' && scalar <= U'\x9f')) {
            throw std::invalid_argument{
                "Table cells require single-line printable text"};
        }
    }
}
void check_row(std::span<std::string const> row, std::size_t count)
{
    if (row.size() != count) {
        throw std::invalid_argument{"Table row has wrong column count"};
    }
    for (auto const& text : row) {
        check_text(text);
    }
}
void check_data(std::span<ox::TableColumn const> columns,
                std::span<std::vector<std::string> const> rows)
{
    if (rows.size() > (std::size_t)std::numeric_limits<int>::max() ||
        columns.size() > (std::size_t)std::numeric_limits<int>::max()) {
        throw std::length_error{"Table exceeds coordinate range"};
    }
    auto policies = std::vector<ox::SizePolicy>{};
    auto total = columns.empty() ? std::int64_t{0} : (std::int64_t)columns.size() - 1;
    for (auto const& column : columns) {
        check_text(column.heading);
        policies.push_back(column.size);
        total += column.size.minimum;
    }
    if (total > std::numeric_limits<int>::max()) {
        throw std::length_error{"Table columns exceed coordinate range"};
    }
    (void)ox::distribute_length(policies, 0);
    for (auto const& row : rows) {
        check_row(row, columns.size());
    }
}
}  // namespace

namespace ox {
DataTable::Options const DataTable::init = {};
DataTable::DataTable(Options x)
    : Widget{x.focus_policy, x.width_policy, x.height_policy},
      style{x.style},
      line_style{x.line_style},
      header_style{x.header_style},
      alternate_style{x.alternate_style},
      selected_style{x.selected_style},
      show_header{x.show_header},
      column_lines{x.column_lines},
      selectable{x.selectable},
      empty_text{std::move(x.empty_text)}
{
    check_data(x.columns, x.rows);
    check_text(empty_text);
    columns_ = std::move(x.columns);
    rows_ = std::move(x.rows);
    update_geometry();
}
auto DataTable::header_height() const -> int
{ return show_header && !columns_.empty() ? 2 : 0; }
void DataTable::update_geometry()
{
    auto policies = std::vector<SizePolicy>{};
    auto minimum = 0;
    auto const gaps = column_lines && !columns_.empty() ? (int)columns_.size() - 1 : 0;
    for (auto const& column : columns_) {
        policies.push_back(column.size);
        minimum += column.size.minimum;
    }
    auto widths = distribute_length(policies, std::max(minimum, size().width - gaps));
    widths_.swap(widths);
    update_viewport();
}
void DataTable::update_viewport()
{
    viewport_ = {size().width, std::max(0, size().height - header_height())};
    auto const gaps = column_lines && !columns_.empty() ? (int)columns_.size() - 1 : 0;
    content_ = {std::accumulate(widths_.begin(), widths_.end(), gaps),
                (int)rows_.size()};
    offset_ = {clamp_scroll(offset_.x, content_.width, viewport_.width),
               clamp_scroll(offset_.y, content_.height, viewport_.height)};
    if (selected_ && *selected_ >= rows_.size()) { selected_.reset(); }
    if (!selectable) { selected_.reset(); }
}
void DataTable::data_changed()
{
    request_layout();
    update_viewport();
    request_repaint();
    notify();
}
void DataTable::notify()
{
    auto lifetime = track(*this);
    if (notified_selection_ != selected_) {
        auto const value = selected_;
        notified_selection_ = value;
        on_select(value);
    }
    // A selection callback may move, mutate, or destroy the table.
    auto* table = lifetime.try_get();
    if (!table) { return; }
    auto const value = std::tuple{table->offset_, table->content_, table->viewport_};
    if (table->notified_scroll_ == value) { return; }
    table->notified_scroll_ = value;
    table->on_scroll(std::get<0>(value), std::get<1>(value), std::get<2>(value));
}
void DataTable::refresh()
{
    check_text(empty_text);
    update_geometry();
    request_repaint();
    notify();
}
void DataTable::on_arrange(Rect) { refresh(); }
void DataTable::set_data(std::vector<TableColumn> columns,
                         std::vector<std::vector<std::string>> rows)
{
    check_data(columns, rows);
    // Stage geometry before changing the live data as well.
    auto policies = std::vector<SizePolicy>{};
    auto const gaps = column_lines && !columns.empty() ? (int)columns.size() - 1 : 0;
    auto minimum = 0;
    for (auto const& column : columns) {
        policies.push_back(column.size);
        minimum += column.size.minimum;
    }
    auto widths = distribute_length(policies, std::max(minimum, size().width - gaps));
    columns_.swap(columns);
    rows_.swap(rows);
    widths_.swap(widths);
    selected_.reset();
    offset_ = {};
    data_changed();
}
void DataTable::set_rows(std::vector<std::vector<std::string>> rows)
{
    check_data(columns_, rows);
    rows_.swap(rows);
    selected_.reset();
    offset_.y = 0;
    data_changed();
}
void DataTable::insert_row(std::size_t index, std::vector<std::string> row)
{
    if (index > rows_.size()) { throw std::out_of_range{"Table row index"}; }
    if (rows_.size() == (std::size_t)std::numeric_limits<int>::max()) {
        throw std::length_error{"Too many table rows"};
    }
    check_row(row, columns_.size());
    rows_.insert(rows_.begin() + (std::ptrdiff_t)index, std::move(row));
    if (selected_ && *selected_ >= index) { ++*selected_; }
    if (offset_.y > 0 && index <= (std::size_t)offset_.y) { ++offset_.y; }
    data_changed();
}
void DataTable::add_row(std::vector<std::string> row)
{ insert_row(rows_.size(), std::move(row)); }
void DataTable::erase_row(std::size_t index)
{
    if (index >= rows_.size()) { throw std::out_of_range{"Table row index"}; }
    rows_.erase(rows_.begin() + (std::ptrdiff_t)index);
    if (index < (std::size_t)offset_.y) { --offset_.y; }
    if (selected_ == index) { selected_.reset(); }
    else if (selected_ && *selected_ > index) {
        --*selected_;
    }
    data_changed();
}
void DataTable::set_cell(std::size_t row, std::size_t column, std::string text)
{
    if (row >= rows_.size() || column >= columns_.size()) {
        throw std::out_of_range{"Table cell index"};
    }
    check_text(text);
    if (rows_[row][column] == text) { return; }
    rows_[row][column] = std::move(text);
    request_repaint();
}
void DataTable::sort_rows(std::size_t column,
                          bool ascending,
                          std::function<bool(std::string_view, std::string_view)> less)
{
    if (column >= columns_.size()) { throw std::out_of_range{"Table column index"}; }
    auto order = std::vector<std::size_t>(rows_.size());
    std::iota(order.begin(), order.end(), std::size_t{0});
    std::stable_sort(order.begin(), order.end(), [&](auto a, auto b) {
        if (!ascending) { std::swap(a, b); }
        return less ? less(rows_[a][column], rows_[b][column])
                    : rows_[a][column] < rows_[b][column];
    });
    auto rows = std::vector<std::vector<std::string>>{};
    rows.reserve(rows_.size());
    for (auto index : order) {
        rows.push_back(std::move(rows_[index]));
    }
    if (selected_) {
        selected_ = (std::size_t)(std::ranges::find(order, *selected_) - order.begin());
    }
    rows_.swap(rows);
    if (selected_) {
        offset_.y = scroll_to_show(offset_.y, (int)*selected_, 1, content_.height,
                                   viewport_.height);
    }
    data_changed();
}
auto DataTable::select(std::optional<std::size_t> row) -> bool
{
    if (row && (!selectable || *row >= rows_.size())) { return false; }
    auto const before = selected_;
    auto const old = offset_;
    selected_ = row;
    if (row) {
        offset_.y =
            scroll_to_show(offset_.y, (int)*row, 1, content_.height, viewport_.height);
    }
    auto const changed = before != selected_ || old != offset_;
    if (changed) { request_repaint(); }
    notify();
    return changed;
}
auto DataTable::set_scroll_offset(Point position) -> bool
{
    position = {clamp_scroll(position.x, content_.width, viewport_.width),
                clamp_scroll(position.y, content_.height, viewport_.height)};
    auto const changed = std::exchange(offset_, position) != position;
    if (changed) { request_repaint(); }
    notify();
    return changed;
}
void DataTable::paint_cell(Canvas canvas,
                           std::string_view text,
                           Align align,
                           Style appearance)
{ paint_row_text(canvas, text, align, appearance, cell_layout_); }
auto DataTable::measure(Area available, esc::WidthPolicy) const -> Area
{
    validate(available);
    auto width =
        std::int64_t{column_lines && !columns_.empty() ? (int)columns_.size() - 1 : 0};
    for (auto const& column : columns_) {
        width += column.size.minimum;
    }
    auto const height = (std::int64_t)rows_.size() + header_height();
    if (width > std::numeric_limits<int>::max() ||
        height > std::numeric_limits<int>::max()) {
        throw std::length_error{"Table extent overflow"};
    }
    return {(int)width, (int)height};
}
auto DataTable::paint(Canvas canvas) -> Cursor
{
    auto base = appearance(style);
    auto lines = appearance(line_style, Surface::Border);
    fill(canvas, base);
    auto const header = header_height();
    auto const body = canvas.subcanvas({0, header}, viewport_);
    auto const [first, last] = visible_rows(body, offset_.y, rows_.size());
    auto x = -(std::int64_t)offset_.x;
    for (std::size_t column = 0; column < columns_.size(); ++column) {
        if (x > std::numeric_limits<int>::max()) { break; }
        auto const at = (int)x;
        auto const width = widths_[column];
        auto const& spec = columns_[column];
        if (header) {
            auto heading = canvas.subcanvas({at, 0}, {width, 1});
            fill(heading, base | spec.style | header_style);
            if (heading.visible().size.width > 0) {
                paint_cell(heading.subcanvas({std::min(1, width), 0},
                                             {std::max(0, width - 2), 1}),
                           spec.heading, spec.align, base | spec.style | header_style);
            }
        }
        for (auto row = first; row < last; ++row) {
            auto cell = body.subcanvas({at, row - offset_.y}, {width, 1});
            if (cell.visible().size.width == 0) { continue; }
            auto appearance = base | spec.style;
            if (row % 2) { appearance = appearance | alternate_style; }
            if (selected_ == (std::size_t)row) {
                appearance = selection_appearance(appearance, selected_style);
            }
            fill(cell, appearance);
            paint_cell(
                cell.subcanvas({std::min(1, width), 0}, {std::max(0, width - 2), 1}),
                rows_[(std::size_t)row][column], spec.align, appearance);
        }
        x += width;
        if (column_lines && column + 1 < columns_.size() &&
            x <= std::numeric_limits<int>::max()) {
            put(canvas, {(int)x, 0},
                shape::VLine{.symbol = theme().ascii ? U'|' : U'│'},
                canvas.size().height, lines);
            ++x;
        }
    }
    if (header && canvas.size().height > 1) {
        put(canvas, {0, 1}, shape::HLine{.symbol = theme().ascii ? U'-' : U'─'},
            canvas.size().width, lines);
    }
    if (rows_.empty()) {
        paint_cell(body.subcanvas({}, {viewport_.width, 1}), empty_text, Align::Center,
                   base);
    }
    return {};
}
auto DataTable::handle(KeyEvent const& event) -> EventResponse
{
    if (event.action == KeyAction::Release) { return {}; }
    if (selectable && !rows_.empty() && activates(event)) {
        auto const row = selected_.value_or(0);
        auto lifetime = track(*this);
        auto const changed = select(row);
        if (auto* table = lifetime.try_get(); table && table->selected_ == row) {
            table->on_activate(row);
        }
        return {.handled = true, .repaint = changed};
    }
    auto matches = [&](KeyCode code) { return event.matches(code, {}, event.action); };
    if (matches(KeyCode::ArrowLeft) || matches(KeyCode::ArrowRight)) {
        auto const changed = set_scroll_offset(
            {scroll_by(offset_.x, matches(KeyCode::ArrowLeft) ? -1 : 1, content_.width,
                       viewport_.width),
             offset_.y});
        return {.handled = true, .repaint = changed};
    }
    if (event.modifiers != Modifiers{}) { return {}; }
    auto const movement = row_navigation(event, viewport_.height);
    if (!movement) { return {}; }
    auto const delta = *movement;
    if (rows_.empty()) { return {}; }
    auto changed = false;
    if (selectable) {
        auto const index = row_target(selected_, rows_.size(), delta);
        changed = select(index);
    }
    else {
        changed =
            set_scroll_offset({offset_.x, scroll_by(offset_.y, delta, content_.height,
                                                    viewport_.height)});
    }
    return {.handled = true, .repaint = changed};
}
auto DataTable::handle(MousePress const& event) -> EventResponse
{
    if (!activates(event) || !contains({{}, size()}, event.at)) { return {}; }
    if (header_height() && event.at.y == 0) {
        auto x = std::int64_t{event.at.x} + offset_.x;
        for (std::size_t column = 0; column < widths_.size(); ++column) {
            if (x < widths_[column]) {
                on_header(column);
                return {.handled = true};
            }
            x -= widths_[column] + (column_lines ? 1 : 0);
            if (x < 0) { return {}; }
        }
        return {};
    }
    if (!selectable || event.at.y < header_height()) { return {}; }
    auto const row = std::int64_t{event.at.y} - header_height() + offset_.y;
    if (row >= content_.height) { return {}; }
    auto const changed = select((std::size_t)row);
    return {.handled = true, .repaint = changed};
}
auto DataTable::handle(MouseWheel const& event) -> EventResponse
{
    if (event.dx == 0 && event.dy == 0) { return {}; }
    auto const changed = set_scroll_offset(
        {scroll_by(offset_.x, event.dx, content_.width, viewport_.width),
         scroll_by(offset_.y, event.dy, content_.height, viewport_.height)});
    return {.handled = true, .repaint = changed};
}
}  // namespace ox

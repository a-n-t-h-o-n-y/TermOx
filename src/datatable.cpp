#include <ox/datatable.hpp>
#include <ox/rule.hpp>

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
auto checked_height(std::size_t count, int stride, bool row_lines, int header) -> int
{
    auto height = (std::int64_t)count * stride - (row_lines && count ? 1 : 0);
    if (height + header > std::numeric_limits<int>::max())
        throw std::length_error{"Table extent"};
    return (int)height;
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
      focused_style{x.focused_style},
      blurred_style{x.blurred_style},
      inactive_selected_style{x.inactive_selected_style},
      hover_style{x.hover_style},
      empty_style{x.empty_style},
      striped{x.striped},
      hover_rows{x.hover_rows},
      header_separator{x.header_separator},
      row_lines{x.row_lines},
      show_sort_indicator{x.show_sort_indicator},
      cell_padding{x.cell_padding},
      row_height{x.row_height},
      cell_style{std::move(x.cell_style)},
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
{ return show_header && !columns_.empty() ? 1 + (int)header_separator : 0; }
auto DataTable::row_stride() const -> int
{
    if (row_height < 1 || cell_padding < 0)
        throw std::invalid_argument{"Invalid table spacing"};
    auto stride = (std::int64_t)row_height + (int)row_lines;
    if (stride > std::numeric_limits<int>::max())
        throw std::length_error{"Table row extent"};
    return (int)stride;
}
auto DataTable::row_extent(std::size_t count) const -> int
{ return checked_height(count, row_stride(), row_lines, header_height()); }
auto DataTable::row_at(int y) const -> std::optional<std::size_t>
{
    if (y < header_height() || y >= size().height) return {};
    auto at = (std::int64_t)y - header_height() + offset_.y;
    auto stride = row_stride();
    if (at % stride >= row_height || at / stride >= (std::int64_t)rows_.size())
        return {};
    return (std::size_t)(at / stride);
}
auto DataTable::hovered_row() const -> std::optional<std::size_t>
{
    return hover_rows && hover_ && contains({{}, size()}, *hover_) ? row_at(hover_->y)
                                                                   : std::nullopt;
}
void DataTable::update_geometry()
{
    (void)row_extent(rows_.size());
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
    auto height = row_extent(rows_.size());
    auto const gaps = column_lines && !columns_.empty() ? (int)columns_.size() - 1 : 0;
    viewport_ = {size().width, std::max(0, size().height - header_height())};
    content_ = {std::accumulate(widths_.begin(), widths_.end(), gaps), height};
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
    request_layout();
    request_repaint();
    notify();
}
void DataTable::on_arrange(Rect)
{
    check_text(empty_text);
    update_geometry();
    request_repaint();
    notify();
}
void DataTable::set_data(std::vector<TableColumn> columns,
                         std::vector<std::vector<std::string>> rows)
{
    check_data(columns, rows);
    auto header = show_header && !columns.empty() ? 1 + (int)header_separator : 0;
    (void)checked_height(rows.size(), row_stride(), row_lines, header);
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
    sort_.reset();
    hover_.reset();
    offset_ = {};
    data_changed();
}
void DataTable::set_rows(std::vector<std::vector<std::string>> rows)
{
    check_data(columns_, rows);
    (void)row_extent(rows.size());
    rows_.swap(rows);
    selected_.reset();
    sort_.reset();
    hover_.reset();
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
    (void)row_extent(rows_.size() + 1);
    rows_.insert(rows_.begin() + (std::ptrdiff_t)index, std::move(row));
    if (selected_ && *selected_ >= index) { ++*selected_; }
    if (offset_.y > 0 && (std::int64_t)index * row_stride() <= offset_.y) {
        offset_.y = (int)std::min((std::int64_t)std::numeric_limits<int>::max(),
                                  (std::int64_t)offset_.y + row_stride());
    }
    sort_.reset();
    data_changed();
}
void DataTable::add_row(std::vector<std::string> row)
{ insert_row(rows_.size(), std::move(row)); }
void DataTable::erase_row(std::size_t index)
{
    if (index >= rows_.size()) { throw std::out_of_range{"Table row index"}; }
    rows_.erase(rows_.begin() + (std::ptrdiff_t)index);
    if ((std::int64_t)index * row_stride() < offset_.y) {
        offset_.y = std::max(0, offset_.y - row_stride());
    }
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
    if (sort_ && sort_->first == column) sort_.reset();
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
    sort_ = {{column, ascending}};
    if (selected_) {
        offset_.y = scroll_to_show(offset_.y, (int)*selected_ * row_stride(),
                                   row_height, content_.height, viewport_.height);
    }
    data_changed();
}
auto DataTable::select(std::optional<std::size_t> row) -> bool
{
    if (row && (!selectable || *row >= rows_.size())) { return false; }
    (void)row_extent(rows_.size());
    auto const before = selected_;
    auto const old = offset_;
    selected_ = row;
    if (row) {
        offset_.y = scroll_to_show(offset_.y, (int)*row * row_stride(), row_height,
                                   content_.height, viewport_.height);
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
    auto const height = (std::int64_t)row_extent(rows_.size()) + header_height();
    if (width > std::numeric_limits<int>::max() ||
        height > std::numeric_limits<int>::max()) {
        throw std::length_error{"Table extent overflow"};
    }
    return {(int)width, (int)height};
}
void DataTable::paint_separator(Canvas canvas, int y, Style lines)
{
    if (theme().styled || row_lines) {
        paint_rule(canvas.subcanvas({0, y}, {canvas.size().width, 1}), 0, 0, lines,
                   lines, false, theme().ascii);
        if (column_lines) {
            auto boundary = -(std::int64_t)offset_.x;
            for (std::size_t col = 0; col + 1 < columns_.size(); ++col) {
                boundary += widths_[col];
                if (boundary >= 0 && boundary < canvas.size().width) {
                    auto junction = canvas.size().height > y + 1 ? U'┼' : U'┴';
                    if (boundary == 0)
                        junction = canvas.size().height > y + 1 ? U'├' : U'└';
                    else if (boundary == canvas.size().width - 1)
                        junction = canvas.size().height > y + 1 ? U'┤' : U'┘';
                    if (theme().ascii) junction = U'+';
                    canvas.write({(int)boundary, y}, std::u32string_view{&junction, 1},
                                 lines);
                }
                ++boundary;
            }
        }
    }
    else
        put(canvas, {0, y}, shape::HLine{}, canvas.size().width, lines);
}
auto DataTable::paint(Canvas canvas) -> Cursor
{
    auto base = appearance(style | (is_focused() ? focused_style : blurred_style));
    auto lines = appearance(line_style, Surface::Border);
    fill(canvas, U' ' | base);
    auto const header = header_height();
    auto const body = canvas.subcanvas({0, header}, viewport_);
    auto clip = body.visible();
    auto stride = row_stride();
    auto first = std::clamp(((std::int64_t)offset_.y + clip.at.y) / stride,
                            std::int64_t{0}, (std::int64_t)rows_.size());
    auto last = std::clamp(
        ((std::int64_t)offset_.y + clip.at.y + clip.size.height + stride - 1) / stride,
        first, (std::int64_t)rows_.size());
    auto hovered = hovered_row();
    auto x = -(std::int64_t)offset_.x;
    for (std::size_t column = 0; column < columns_.size(); ++column) {
        if (x > std::numeric_limits<int>::max()) break;
        auto at = (int)x;
        auto width = widths_[column];
        auto const& spec = columns_[column];
        auto padding = std::min(cell_padding, width);
        auto inner = (int)std::max(
            std::int64_t{0}, (std::int64_t)width - 2 * (std::int64_t)cell_padding);
        if (header) {
            auto heading = canvas.subcanvas({at, 0}, {width, 1});
            auto heading_style =
                appearance(style | (is_focused() ? focused_style : blurred_style),
                           Surface::Muted) |
                header_style | spec.heading_style;
            fill(heading, heading_style);
            if (heading.visible().size.width > 0) {
                auto sorted = show_sort_indicator && sort_ && sort_->first == column;
                auto label_width = std::max(0, inner - (sorted ? 2 : 0));
                paint_cell(heading.subcanvas({padding, 0}, {label_width, 1}),
                           spec.heading, spec.align, heading_style);
                if (sorted && inner) {
                    auto indicator = sort_->second ? (theme().ascii ? U'^' : U'↑')
                                                   : (theme().ascii ? U'v' : U'↓');
                    auto indicator_style = heading_style;
                    if (theme().styled)
                        indicator_style.foreground = theme().palette.accent;
                    heading.write({padding + inner - 1, 0},
                                  std::u32string_view{&indicator, 1}, indicator_style);
                }
            }
        }
        for (auto row = first; row < last; ++row) {
            auto cell = body.subcanvas({at, (int)(row * stride - offset_.y)},
                                       {width, row_height});
            if (cell.visible().size.width == 0 || cell.visible().size.height == 0)
                continue;
            auto normal = base | spec.style;
            if (row % 2) {
                if (striped && theme().styled)
                    normal.background =
                        mix_color(normal.background, theme().palette.selection, .18);
                normal = normal | alternate_style;
            }
            if (cell_style) normal = normal | cell_style((std::size_t)row, column);
            if (hovered == (std::size_t)row) {
                if (theme().styled)
                    normal.background =
                        mix_color(normal.background, theme().palette.selection, .4);
                normal = normal | hover_style;
            }
            if (selected_ == (std::size_t)row) {
                normal = theme_selection(normal, selected_style, theme(), is_focused());
                if (!is_focused()) normal = normal | inactive_selected_style;
            }
            fill(cell, normal);
            paint_cell(cell.subcanvas({padding, (row_height - 1) / 2}, {inner, 1}),
                       rows_[(std::size_t)row][column], spec.align, normal);
        }
        x += width;
        if (column_lines && column + 1 < columns_.size() &&
            x <= std::numeric_limits<int>::max()) {
            if (theme().styled)
                paint_rule(canvas.subcanvas({(int)x, 0}, {1, canvas.size().height}), 0,
                           0, lines, lines, true, theme().ascii);
            else
                put(canvas, {(int)x, 0}, shape::VLine{}, canvas.size().height, lines);
            ++x;
        }
    }
    if (header_separator && header && canvas.size().height > 1)
        paint_separator(canvas, 1, lines);
    if (row_lines) {
        for (auto row = first; row < last && row + 1 < (std::int64_t)rows_.size();
             ++row) {
            auto y = row * stride + row_height - offset_.y + header;
            if (y >= header && y < canvas.size().height)
                paint_separator(canvas, (int)y, lines);
        }
    }
    if (rows_.empty())
        paint_cell(body.subcanvas({}, {viewport_.width, 1}), empty_text, Align::Center,
                   appearance(style | (is_focused() ? focused_style : blurred_style),
                              Surface::Muted) |
                       empty_style);
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
    auto const movement =
        row_navigation(event, std::max(1, viewport_.height / row_stride()));
    if (!movement) { return {}; }
    auto const delta = *movement;
    if (rows_.empty()) { return {}; }
    auto changed = false;
    if (selectable) {
        auto const index = row_target(selected_, rows_.size(), delta);
        changed = select(index);
    }
    else {
        changed = set_scroll_offset(
            {offset_.x,
             scroll_by(offset_.y,
                       (int)std::clamp((std::int64_t)delta * row_stride(),
                                       (std::int64_t)std::numeric_limits<int>::min(),
                                       (std::int64_t)std::numeric_limits<int>::max()),
                       content_.height, viewport_.height)});
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
    auto row = row_at(event.at.y);
    if (!row) return {};
    auto const changed = select(row);
    return {.handled = true, .repaint = changed};
}
auto DataTable::handle(MouseMove const& event) -> EventResponse
{
    auto before = hovered_row();
    hover_ = contains({{}, size()}, event.at) ? std::optional{event.at} : std::nullopt;
    auto changed = before != hovered_row();
    if (changed) request_repaint();
    return {.repaint = changed};
}
auto DataTable::mouse_leave() -> EventResponse
{
    auto changed = hovered_row().has_value();
    hover_.reset();
    if (changed) request_repaint();
    return {.repaint = changed};
}
auto DataTable::handle(MouseWheel const& event) -> EventResponse
{
    if (event.dx == 0 && event.dy == 0) { return {}; }
    auto const changed = set_scroll_offset(
        {scroll_by(offset_.x, event.dx, content_.width, viewport_.width),
         scroll_by(offset_.y,
                   (int)std::clamp((std::int64_t)event.dy * row_stride(),
                                   (std::int64_t)std::numeric_limits<int>::min(),
                                   (std::int64_t)std::numeric_limits<int>::max()),
                   content_.height, viewport_.height)});
    return {.handled = true, .repaint = changed};
}
}  // namespace ox

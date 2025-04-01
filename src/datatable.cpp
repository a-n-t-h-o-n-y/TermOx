#include <ox/datatable.hpp>

#include <algorithm>
#include <span>
#include <stdexcept>
#include <string>
#include <vector>

#include <ox/align.hpp>
#include <ox/core/core.hpp>
#include <ox/put.hpp>

namespace ox {

DataTable::Options const DataTable::init = {};

DataTable::DataTable(Options x)
    : Widget{FocusPolicy::None, x.size_policy},
      background{std::move(x.background)},
      foreground_cell{std::move(x.foreground_cell)},
      foreground_line{std::move(x.foreground_line)}
{}

void DataTable::add_column(std::string heading, Align align, Color foreground)
{
    headings_.children.push_back({{
        .text = std::move(heading),
        .align = align,
        .brush = {.foreground = foreground, .traits = Trait::Bold},
    }});
    auto const existing_size = columns_.empty() ? 0 : columns_.front().size();
    columns_.push_back(std::vector<std::string>(existing_size, ""));
}

void DataTable::add_row(std::vector<std::string> row)
{
    if (row.size() != columns_.size()) {
        throw std::runtime_error{
            "DataTable::add_row: row size does not match column count."};
    }
    for (auto i = std::size_t{0}; i < columns_.size(); ++i) {
        columns_[i].push_back(std::move(row[i]));
    }
}

void DataTable::paint(Canvas c)
{
    if (this->size.width == 0 || this->size.height == 0 || columns_.empty()) { return; }

    auto const border_area = Area{
        .width = c.size.width,
        .height = std::min(c.size.height,
                           ((int)columns_.front().size() - (int)this->offset) * 2 + 3),
    };

    // Fill with Background
    for (auto x = 0; x < c.size.width; ++x) {
        for (auto y = 0; y < c.size.height; ++y) {
            c[{x, y}].brush.background = background;
        }
    }

    // Border
    put(c, {0, 0},
        shape::Box{
            .corners = shape::Box::round_corners,
            .size = border_area,
        },
        foreground_line);

    // Header Line
    if (this->size.height > 2) {
        put(c, {.x = 0, .y = 2}, U'╞' | fg(foreground_line));
        put(c, {.x = 1, .y = 2},
            shape::HLine{
                .length = std::max(c.size.width - 2, 0),
                .symbol = U'═',
            },
            foreground_line);
        put(c, {.x = c.size.width - 1, .y = 2}, U'╡' | fg(foreground_line));
    }

    auto const table_y = 3;

    // Cell Text
    for (std::size_t column_index = 0; column_index < columns_.size(); ++column_index) {
        auto const& column = columns_[column_index];

        auto cell_point = Point{
            .x = headings_.children[column_index].at.x + 1,
            .y = table_y,
        };
        auto const cell_width = headings_.children[column_index].size.width;

        for (auto const& text : std::span{column}.subspan(this->offset)) {
            if (cell_point.y + 1 >= border_area.height) { break; }
            if (text.size() + 1 > (std::size_t)cell_width) {
                put(c, cell_point,
                    text.substr(0, (std::size_t)std::max(cell_width - 2, 0)) + "…");
            }
            else {
                put(c, cell_point, text);
            }
            cell_point.y += 2;
        }
    }

    // Border Intersections
    for (auto y = 4; y + 1 < border_area.height; y += 2) {
        put(c, {0, y}, U'├' | fg(foreground_line));
        put(c, {c.size.width - 1, y}, U'┤' | fg(foreground_line));
    }
    for (auto i = std::size_t{0}; i + 1 < headings_.children.size(); ++i) {
        auto const& heading = headings_.children[i];
        auto const x = heading.at.x + heading.size.width;
        put(c, {x, 2}, U'╤' | fg(foreground_line));
        put(c, {x, std::max(border_area.height - 1, 0)}, U'┴' | fg(foreground_line));
    }

    // Horizontal Lines
    for (auto y = 4; y + 1 < border_area.height; y += 2) {
        for (auto column_index = std::size_t{0}; column_index < columns_.size();
             ++column_index) {
            auto const& heading = headings_.children[column_index];
            auto x = heading.at.x;
            put(c, {x + 1, y},
                shape::HLine{
                    .length = heading.size.width,
                    .symbol = U'─',
                },
                foreground_line);
        }
    }

    // Vertical Lines and Intersection
    for (auto y = 3; y + 1 < border_area.height; y += 2) {
        for (auto column_index = std::size_t{0}; column_index + 1 < columns_.size();
             ++column_index) {
            auto const& heading = headings_.children[column_index];
            auto x = heading.at.x + heading.size.width;
            put(c, {x, y}, U'│' | fg(foreground_line));
            if (y + 2 < border_area.height) {
                put(c, {x, y + 1}, U'┼' | fg(foreground_line));
            }
        }
    }
}

void DataTable::mouse_wheel(Mouse m)
{
    if (m.button == Mouse::Button::ScrollUp) {
        offset = (std::size_t)std::max(0, (int)offset - 1);
    }
    else if (m.button == Mouse::Button::ScrollDown) {
        offset = (std::size_t)std::min(
            columns_.empty() ? 0 : (int)columns_.front().size() - 1, (int)offset + 1);
    }
}

void DataTable::resize(Area)
{
    auto const old_size = headings_.size;
    headings_.at = {
        .x = std::min(1, std::max(size.width - 1, 0)),
        .y = std::min(1, std::max(size.height - 1, 0)),
    };
    headings_.size = {
        .width = std::max(size.width - 2, 0),
        .height = std::min(1, size.height),
    };
    headings_.resize(old_size);
}

auto DataTable::get_children() -> zzz::Generator<Widget&> { co_yield headings_; }

auto DataTable::get_children() const -> zzz::Generator<Widget const&>
{
    co_yield headings_;
}

}  // namespace ox